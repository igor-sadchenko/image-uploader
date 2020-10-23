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

#include "myutils.h"

#include "atlheaders.h"
#include <string>
#include "Core/Utils/CoreUtils.h"
#include "Core/Utils/StringUtils.h"
#include "Core/Video/VideoUtils.h"
#include "Func/WinUtils.h"

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
typedef BOOL (WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);

int GetFontSizeInTwips(int nFontSize)
{
   return MulDiv(nFontSize, 1440, 72);
}

bool IsVideoFile(LPCTSTR szFileName)
{
    std::string ext = IuStringUtils::toLower( IuCoreUtils::ExtractFileExt(IuCoreUtils::WstringToUtf8(szFileName)) );
    std::vector<std::string>& extensions = VideoUtils::Instance().videoFilesExtensions;
    if(std::find(extensions.begin(), extensions.end(), ext) != extensions.end()) {
        return true;
    } else {
    return false;
    }
}

int GetSavingFormat(LPCTSTR szFileName)
{
    if(!szFileName) return -1;
    LPCTSTR FileType = WinUtils::GetFileExt(szFileName);
    if(WinUtils::IsStrInList(FileType,_T("jpg\0jpeg\0\0")))
        return 0;
    else if(WinUtils::IsStrInList(FileType,_T("png\0\0")))
        return 1;
    else if(WinUtils::IsStrInList(FileType,_T("gif\0\0")))
        return 2;
    else return 0;
}

LPTSTR fgetline(LPTSTR buf,int num,FILE *f)
{
    LPTSTR Result;
    LPTSTR cur;
    Result=_fgetts(buf,num,f);
    int n=lstrlen(buf)-1;

    for(cur=buf+n;cur>=buf;cur--)
    {
        if(*cur==13||*cur==10||*cur==_T('\n'))
            *cur=0;
        else break;
    }
    return Result;
}

CString TrimString(const CString& source, int nMaxLen)
{
    int nLen = source.GetLength();
    if(nLen <= nMaxLen) return source;

    int PartSize = (nMaxLen-3) / 2;
    return source.Left(PartSize)+_T("...")+source.Right(PartSize);
}

LPCTSTR  CopyToStartOfW(LPCTSTR szString,LPCTSTR szPattern,LPTSTR szBuffer,int nBufferSize)
{
    int nLen=0;
    if(!szString || !szPattern ||!szBuffer || nBufferSize<0) return 0;

    LPCTSTR szStart = (LPTSTR) _tcsstr(szString, szPattern);

    if(!szStart) 
    {
        nLen = lstrlen(szString);
        szStart = szString + nLen-1;
    }
    else nLen = szStart - szString;

    if(nLen > nBufferSize-1) nLen = nBufferSize-1;
    lstrcpyn(szBuffer, szString, nLen+1);
    szBuffer[nLen]=0;

    return szStart+1;
}

#undef PixelFormat8bppIndexed 
#define PixelFormat8bppIndexed (3 | ( 8 << 8) | PixelFormatIndexed | PixelFormatGDI)

/* MakeFontBold
    MakeFontUnderLine

    -----------------------
    These functions create bold/underlined fonts based on given font
*/
HFONT MakeFontBold(HFONT font)
{
    if(!font) return 0;

    LOGFONT alf;

    bool ok = ::GetObject(font, sizeof(LOGFONT), &alf) == sizeof(LOGFONT);

    if(!ok) return 0;

    alf.lfWeight = FW_BOLD;

    HFONT NewFont = CreateFontIndirect(&alf);
    return NewFont;
}

HFONT MakeFontUnderLine(HFONT font)
{
    if(!font) return 0;

    LOGFONT alf;

    bool ok = ::GetObject(font, sizeof(LOGFONT), &alf) == sizeof(LOGFONT);

    if(!ok) return 0;

    alf.lfUnderline = 1;
    HFONT NewFont = CreateFontIndirect(&alf);

    return NewFont;
}


LPTSTR MoveToEndOfW(LPTSTR szString,LPTSTR szPattern)
{
    size_t nLen;

    if(!szString || !szPattern) return szString;

    nLen = wcslen(szPattern);
    if(!nLen) return szString;
    
    LPTSTR szStart = wcsstr(szString, szPattern);

    if(!szStart) return szString;
    else szString = szStart+nLen;

    return szString;
}

bool CheckFileName(const CString& fileName)
{
    return (fileName.FindOneOf(_T("\\/:*?\"<>|")) < 0);
}