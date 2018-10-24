/*

    Image Uploader -  free application for uploading images/files to the Internet

    Copyright 2007-2018 Sergey Svistunov (zenden2k@yandex.ru)

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

*/

#include "CoreUtils.h"

#include <time.h>
#include <locale>
#include <cstdio>
#include <map>
#include <string>
#include <cmath>

#ifdef _WIN32
    #include <io.h>
    #include "Core/Utils/utils_Win.h"
    #include <WinSock.h>
#else
    #include <boost/filesystem.hpp>
    #ifdef __APPLE__
        #include <sys/uio.h>
    #else
        #include <sys/io.h>
    #endif
    #include <sys/stat.h>
    #include "Core/Utils/utils_unix.h"
#endif
#if defined(_MSC_VER) && _MSC_VER < 1800 
long round(float number){
    return number < 0.0 ? ceil(number - 0.5) : floor(number + 0.5);
}
#endif

#ifdef _WIN32
/* FILETIME of Jan 1 1970 00:00:00. */
static const unsigned __int64 epoch = ((unsigned __int64)116444736000000000ULL);

/*
* PostgreSQL's implementation of gettimeofday for windows:
*
* timezone information is stored outside the kernel so tzp isn't used anymore.
*
* Note: this function is not for Win32 high precision timing purpose. See
* elapsed_time().
*/
int
gettimeofday(struct timeval * tp, struct timezone * tzp)
{
    FILETIME    file_time;
    SYSTEMTIME  system_time;
    ULARGE_INTEGER ularge;

    GetSystemTime(&system_time);
    SystemTimeToFileTime(&system_time, &file_time);
    ularge.LowPart = file_time.dwLowDateTime;
    ularge.HighPart = file_time.dwHighDateTime;

    tp->tv_sec = static_cast<long>((ularge.QuadPart - epoch) / 10000000L);
    tp->tv_usec = static_cast<long>(system_time.wMilliseconds * 1000);

    return 0;
}
#endif

namespace IuCoreUtils {

FILE * fopen_utf8(const char * filename, const char * mode)
{
#ifdef _MSC_VER
    return _wfopen(Utf8ToWstring(filename).c_str(), Utf8ToWstring(mode).c_str());
#else
    return fopen(Utf8ToSystemLocale(filename).c_str(), mode);
#endif
}

int fseek_64(FILE* stream, int64_t offset, int origin)
{
#ifdef _MSC_VER
    return _fseeki64(stream, offset, origin);
#else
    return fseeko64(stream, offset, origin);
#endif
}

bool FileExists(const std::string& fileName)
{
    #ifdef WIN32
    DWORD res = GetFileAttributes(Utf8ToWstring(fileName).c_str());
    if (res == static_cast<DWORD>(-1)) {
        switch (GetLastError()) {
        case ERROR_FILE_NOT_FOUND:
            return false;
        case ERROR_PATH_NOT_FOUND:
            return false;
        case ERROR_ACCESS_DENIED:
            return true;
        default:
            return false;
        }
    }
    #else
        return boost::filesystem::exists(fileName);
    #endif
    return true;
}

typedef std::codecvt_base::result res;
typedef std::codecvt<wchar_t, char, mbstate_t> codecvt_type;
std::mbstate_t state;

std::codecvt_base::result fromWstring (const std::wstring & str,
   const std::locale & loc, std::string & out)
{
  const codecvt_type& cdcvt = std::use_facet<codecvt_type>(loc);
  std::codecvt_base::result r;

  const wchar_t *in_next = 0;
  char *out_next = 0;

  std::wstring::size_type len = str.size () << 2;

  char * chars = new char [len + 1];

  r = cdcvt.out (state, str.c_str (), str.c_str () + str.size (), in_next,
                 chars, chars + len, out_next);
  *out_next = '\0';
  out = chars;

  delete [] chars;

  return r;
}

std::codecvt_base::result toWstring (const std::string & str, 
   const std::locale & loc, std::wstring & out)
{ 
    const codecvt_type& cdcvt = std::use_facet<codecvt_type>(loc);
    std::codecvt_base::result r;
  
    wchar_t * wchars = new wchar_t[str.size() + 1];
 
    const char *in_next = 0;
    wchar_t *out_next = 0;
  
    r = cdcvt.in (state, str.c_str (), str.c_str () + str.size (), in_next,
                wchars, wchars + str.size () + 1, out_next);
    *out_next = '\0';
    out = wchars;    
  
    delete [] wchars;
  
    return r;
}

std::string SystemLocaleToUtf8(const std::string& str)
{
    std::locale const oloc = std::locale ("");
   std::wstring out;
   std::wstring wideStr;
    toWstring (str, oloc, wideStr);
    return WstringToUtf8(wideStr);
}

std::string Utf8ToSystemLocale(const std::string& str)
{
   std::wstring wideStr = Utf8ToWstring(str);
   std::locale const oloc = std::locale ("");
   std::string out;
   /*std::codecvt_base::result r = */fromWstring (wideStr, oloc, out);
   return out;
}

std::string ExtractFileName(const std::string fileName)
{
        std::string temp = fileName;
        int Qpos = temp.find_last_of('?'); //FIXME
        if(Qpos>=0) temp = temp.substr(0, Qpos-1);
        int i,len = temp.length();
        for(i=len-1; i>=0; i--)
        {
            if(temp[i] == '\\' || temp[i]=='/')
                break;
        }
        return temp.substr(i+1);
}

std::string ExtractFileExt(const std::string fileName)
{
    int nLen = fileName.length();

    std::string result;
    for( int i=nLen-1; i>=0; i-- )
    {
        if(fileName[i] == '.')
        {
            result = fileName.substr(i + 1);
            break;
        }
        else if(fileName[i] == '\\' || fileName[i] == '/') break;
    }
    return result;
}

const std::string ExtractFilePath(const std::string& fileName)
{
    int i, len = fileName.length();
    for(i = len-1; i >= 0; i--)
    {
        if(fileName[i] == '\\' || fileName[i]=='/')
        {
            return fileName.substr(0, i+1);
        }
            
    }
    return "";
}

const std::string ExtractFileNameNoExt(const std::string& fileName)
{
    std::string result = ExtractFileName(fileName);
    size_t Qpos = result.find_last_of('.');
    if (Qpos != std::string::npos) {
        result = result.substr(0, Qpos);
    }
    return result;
}

std::string ExtractFileNameFromUrl(const std::string fileName)
{
    size_t questionMarkPos = fileName.find_last_of('?');

    std::string result;
    if (questionMarkPos != std::string::npos)  {
        return ExtractFileName(fileName.substr(0, questionMarkPos));
    } 

    return ExtractFileName(fileName);
}

std::string incrementFileName(const std::string& originalFileName, int counter) {
    std::string ext = ExtractFileExt(originalFileName);
    std::string name = ExtractFileNameNoExt(originalFileName);
    name += "(" + toString(counter) + ")";
    if (!ext.empty()) {
        name += "." + ext;
    }
    return name;
}


std::string toString(int value)
{
    char buffer[256];
    sprintf(buffer, "%d", value);
    return buffer;
}

std::string toString(unsigned int value)
{
    char buffer[256];
    sprintf(buffer, "%u", value);
    return buffer;
}

std::string StrReplace(std::string text, std::string s, std::string d)
{
    for(size_t index=0; index=text.find(s, index), index!=std::string::npos;)
    {
        text.replace(index, s.length(), d);
        index += d.length();
    }
    return text;
}

bool ReadUtf8TextFile(std::string utf8Filename, std::string& data)
{
    FILE *stream = fopen_utf8(utf8Filename.c_str(), "rb");
    if(!stream) return false;
    int size = static_cast<int>(getFileSize(utf8Filename));
    unsigned char buf[3];
    fread(buf, 1, 3, stream);    

    if(buf[0] == 0xEF || buf[1] == 0xBB || buf[2] == 0xBF) // UTF8 Byte Order Mark (BOM)
    {    
        size -= 3;    
    }
    else if(buf[0] == 0xFF && buf[1] == 0xFE ) {
        // UTF-16LE encoding
        size -= 2;
        fseek( stream, 2L,  SEEK_SET );
        std::wstring res;
        int charCount = size/2;
        res.resize(charCount);
        size_t charsRead = fread(&res[0], 2, charCount, stream);    
        res[charsRead]=0;
        fclose(stream);
        data = WstringToUtf8(res);
        return true;
    } 
    else {
        // no BOM was found; seeking backward
        fseek( stream, 0L,  SEEK_SET );
    }
    try {
        data.resize(size);
    } catch ( std::exception& ex ) {
        LOG(ERROR) << ex.what();
        fclose(stream);
        return false;
    }
   
    /*size_t bytesRead = */fread(&data[0], 1, size, stream);    
    //data[bytesRead] = 0;
    fclose(stream);
    return true;
}

bool PutFileContents(const std::string& utf8Filename, const std::string& content)
{
    FILE *stream = fopen_utf8(utf8Filename.c_str(), "wb");
    if(!stream) return false;
    fwrite(content.c_str(), content.length(),1, stream);
    fclose(stream);
    return true;
}

const std::string GetFileContents(const std::string& filename) {
    std::string data;
    FILE *stream = IuCoreUtils::fopen_utf8(filename.c_str(), "rb");
    if (!stream) return std::string();
    int size = static_cast<int>(IuCoreUtils::getFileSize(filename));

    try {
        data.resize(size);
    } catch (std::exception& ex) {
        LOG(ERROR) << "Unable to allocate " << size << " bytes:" << ex.what();
        fclose(stream);
        return std::string();
    }

    /*size_t bytesRead = */fread(&data[0], 1, size, stream);
    //data[bytesRead] = 0;
    fclose(stream);
    return data;
}

const std::string timeStampToString(time_t t)
{
    // TODO: add system locale support
    tm * timeinfo = localtime ( &t );
    char buf[50];
    sprintf(buf, "%02d.%02d.%04d %02d:%02d:%02d", (int)timeinfo->tm_mday,(int) timeinfo->tm_mon+1, (int)1900+timeinfo->tm_year, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec); 
    return buf;
}

std::string ulonglongToStr(int64_t l, int base)
{
    char buff[67]; // length of MAX_ULLONG in base 2
    buff[66] = 0;
    char *p = buff + 66;
    const char _zero = '0';

    if (base != 10) {
        while (l != 0) {
            int c = l % base;

            --p;

            if (c < 10)
                *p = '0' + c;
            else
                *p = c - 10 + 'a';

            l /= base;
        }
    }
    else {
        while (l != 0) {
            int c = l % base;

            *(--p) = _zero + c;

            l /= base;
        }
    }

    return p;
}

std::string longlongtoStr(int64_t l, int base)
{
   std::string res = ulonglongToStr(l<0 ? -l: l, base);
   if (l < 0)
     res = "-" + res;
   return res;
}


std::string int64_tToString(int64_t value)
{
    return std::to_string(value);
    /*if ( !value ) {
        return "0";
    }
    return longlongtoStr(value, 10);*/
}

#ifndef LLONG_MIN
    #define LLONG_MIN (-9223372036854775807-1)
    #define LLONG_MAX (-9223372036854775807-1)
#endif 

static int64_t zstrtoll(const char *nptr, const char **endptr, register int base, bool *ok)
{
    register const char *s;
    register  uint64_t acc;
    register unsigned char c;
    register  uint64_t qbase, cutoff;
    register int neg, any, cutlim;

    /*
     * Skip white space and pick up leading +/- sign if any.
     * If base is 0, allow 0x for hex and 0 for octal, else
     * assume decimal; if base is already 16, allow 0x.
     */
    s = nptr;
    do {
        c = *s++;
    } while (isspace(c));
    if (c == '-') {
        neg = 1;
        c = *s++;
    } else {
        neg = 0;
        if (c == '+')
            c = *s++;
    }
    if ((base == 0 || base == 16) &&
        c == '0' && (*s == 'x' || *s == 'X')) {
        c = s[1];
        s += 2;
        base = 16;
    }
    if (base == 0)
        base = c == '0' ? 8 : 10;

    qbase = unsigned(base);
    cutoff = neg ? ((int64_t)(0-(LLONG_MIN + LLONG_MAX))) + LLONG_MAX : LLONG_MAX;
    cutlim = static_cast<int>(cutoff % qbase);
    cutoff /= qbase;
    for (acc = 0, any = 0;; c = *s++) {
        if (!isascii(c))
            break;
        if (isdigit(c))
            c -= '0';
        else if (isalpha(c))
            c -= isupper(c) ? 'A' - 10 : 'a' - 10;
        else
            break;
        if (c >= base)
            break;
        if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
            any = -1;
        else {
            any = 1;
            acc *= qbase;
            acc += c;
        }
    }
    if (any < 0) {
        acc = neg ? LLONG_MIN : LLONG_MAX;
        if (ok != 0)
            *ok = false;
    } else if (neg) {
        acc = (~acc) + 1;
    }
    if (endptr != 0)
        *endptr = (any >= 0 ? s - 1 : nptr);

    if (ok != 0)
        *ok = any > 0;

    return acc;
}

int64_t stringToInt64(const std::string fileName)
{
    return zstrtoll(fileName.c_str(), 0, 10 , 0);
}

int64_t getFileSize(std::string utf8Filename)
{
#ifdef _WIN32
   #ifdef _MSC_VER
	struct _stat64  stats;
   #else
      _stati64 stats;
   #endif
      memset(&stats, 0, sizeof(stats));
      stats.st_size = -1;
   _wstati64(Utf8ToWstring(utf8Filename).c_str(), &stats);
#else
   struct stat64 stats;
   std::string path = Utf8ToSystemLocale(utf8Filename);
   if(-1 == stat64(path.c_str(), &stats))
   {
      return -1;
   }
#endif
     return stats.st_size;
}

// �������������� ������� ����� � ������
std::string fileSizeToString(int64_t nBytes)
{
    double number = 0;
    std::string postfix;
    int precision = 0;
    if(nBytes < 0)
    {
        return "n/a";
    }

    if(nBytes < 1024)
    {
        number = double(nBytes);
        postfix = "bytes";
    }
    else if( nBytes < 1048576)
    {
        number = (double)nBytes / 1024.0;
        postfix = "KB";
    }
    else if(nBytes<((int64_t)1073741824)) /*< 1 GB*/
    {
        postfix= "MB";
        number= (double)nBytes / 1048576.0;
        precision = 1;
    }
    else if(nBytes>=1073741824)
    {
        postfix= "GB";
        precision = 1;
        number = (double)nBytes / 1073741824.0;
    }
    return toString(number, precision) + " " + postfix;
}

std::string toString(double value, int precision)
{
    char buffer[100];
    sprintf(buffer, ("%0." + toString(precision) + "f").c_str(), value);
    return buffer;
}

std::string GetDefaultExtensionForMimeType(const std::string mimeType) {
    std::map<std::string, std::string> mimeToExt;
    mimeToExt["image/gif"] = "gif";
    mimeToExt["image/png"] = "png";
    mimeToExt["image/jpeg"] = "jpg";

    std::map<std::string, std::string>::iterator found = mimeToExt.find(mimeType);
    if ( found != mimeToExt.end() ) {
        return found->second;
    }
    return "";
}

std::string ThreadIdToString(const std::thread::id& id)
{
    std::stringstream threadIdSS;
    threadIdSS << id;
    return threadIdSS.str();
}

} // end of namespace IuCoreUtils
