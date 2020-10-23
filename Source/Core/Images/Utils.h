#ifndef IU_CORE_IMAGES_UTILS_H
#define IU_CORE_IMAGES_UTILS_H

#include <memory>
#include <string>
#include "atlheaders.h"
#include <windows.h>
#include "3rdpart/GdiplusH.h"

namespace ImageUtils {

enum SaveImageFormat {
    sifJPEG, sifPNG, sifGIF, sifDetectByExtension
};

struct ImageInfo {
    int width, height;
    ImageInfo() {
        width = 0;
        height = 0;
    }
};
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

std::unique_ptr<Gdiplus::Bitmap> BitmapFromResource(HINSTANCE hInstance, LPCTSTR szResName, LPCTSTR szResType);
void PrintRichEdit(HWND hwnd, Gdiplus::Graphics* graphics, Gdiplus::Bitmap* background, Gdiplus::Rect layoutArea);
void DrawRoundedRectangle(Gdiplus::Graphics* gr, Gdiplus::Rect r, int d, Gdiplus::Pen* p, Gdiplus::Brush*br);
bool SaveImage(Gdiplus::Image* img, const CString& szFilename, SaveImageFormat Format, int Quality);
std::unique_ptr<Gdiplus::Bitmap> IconToBitmap(HICON ico);
void ApplyGaussianBlur(Gdiplus::Bitmap* bm, int x, int y, int w, int h, int radius);
void BlurCleanup();
std::unique_ptr<Gdiplus::Bitmap> LoadImageFromFileWithoutLocking(const WCHAR* fileName);
Gdiplus::Color StringToColor(const std::string& str);
CComPtr<IStream> CreateMemStream(const BYTE *pInit, UINT cbInit);
bool CopyBitmapToClipboard(HWND hwnd, HDC dc, Gdiplus::Bitmap* bm, bool preserveAlpha = true);
void Gdip_RemoveAlpha(Gdiplus::Bitmap& source, Gdiplus::Color color);
bool MySaveImage(Gdiplus::Image* img, const CString& szFilename, CString& szBuffer, int Format, int Quality,
    LPCTSTR Folder = 0);
bool SaveImageToFile(Gdiplus::Image* img, const CString& fileName, IStream* stream, int Format, int Quality, CString* mimeType = nullptr);
void DrawGradient(Gdiplus::Graphics& gr, Gdiplus::Rect rect, Gdiplus::Color& Color1, Gdiplus::Color& Color2);
void DrawStrokedText(Gdiplus::Graphics& gr, LPCTSTR Text, Gdiplus::RectF Bounds, const Gdiplus::Font& font,
    const Gdiplus::Color& ColorText, const Gdiplus::Color& ColorStroke, int HorPos = 0, int VertPos = 0,
    int width = 1);
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
void changeAplhaChannel(Bitmap& source, Bitmap& dest, int sourceChannel, int destChannel);
Rect MeasureDisplayString(Graphics& graphics, CString text, RectF boundingRect, Gdiplus::Font& font);
CRect CenterRect(CRect r1, CRect intoR2);
void DrawRect(Bitmap& gr, Color& color, Rect rect);
std::unique_ptr<Gdiplus::Bitmap> GetThumbnail(Gdiplus::Image* bm, int width, int height, Gdiplus::Size* realSize = 0);
std::unique_ptr<Gdiplus::Bitmap> GetThumbnail(const CString& filename, int width, int height, Gdiplus::Size* realSize = 0);
Size AdaptProportionalSize(const Size& szMax, const Size& szReal);
std::unique_ptr<Bitmap> BitmapFromMemory(unsigned char* data, unsigned size);
bool CopyBitmapToClipboardInDataUriFormat(Bitmap* bm, int Format, int Quality, bool html = false);
bool CopyFileToClipboardInDataUriFormat(const CString& fileName, int Format, int Quality, bool html);

// Load image from file with webp support
std::unique_ptr<Bitmap> LoadImageFromFileExtended(const CString& fileName);
CString GdiplusStatusToString(Gdiplus::Status statusID);
bool ExUtilReadFile(const wchar_t* const file_name, uint8_t** data, size_t* data_size);
short GetImageOrientation(Image* img);
bool RotateAccordingToOrientation(short orient, Image* img, bool removeTag = false);
ImageInfo GetImageInfo(const wchar_t* fileName);

}

#endif
