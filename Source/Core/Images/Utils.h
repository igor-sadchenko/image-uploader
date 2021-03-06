#ifndef IU_CORE_IMAGES_UTILS_H
#define IU_CORE_IMAGES_UTILS_H

#include <memory>
#include <string>
#include "atlheaders.h"
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
void ApplyPixelateEffect(Gdiplus::Bitmap* bm, int xPos, int yPos, int w, int h, int blockSize);
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
void ChangeAlphaChannel(Gdiplus::Bitmap& source, Gdiplus::Bitmap& dest, int sourceChannel, int destChannel);
Gdiplus::Rect MeasureDisplayString(Gdiplus::Graphics& graphics, CString text, Gdiplus::RectF boundingRect, Gdiplus::Font& font);
CRect CenterRect(CRect r1, const CRect& intoR2);
std::unique_ptr<Gdiplus::Bitmap> GetThumbnail(Gdiplus::Image* bm, int width, int height, Gdiplus::Size* realSize = 0);
std::unique_ptr<Gdiplus::Bitmap> GetThumbnail(const CString& filename, int width, int height, Gdiplus::Size* realSize = 0);
Gdiplus::Size AdaptProportionalSize(const Gdiplus::Size& szMax, const Gdiplus::Size& szReal);
std::unique_ptr<Gdiplus::Bitmap> BitmapFromMemory(BYTE* data, size_t size);
bool CopyBitmapToClipboardInDataUriFormat(Gdiplus::Bitmap* bm, int Format, int Quality, bool html = false);
bool CopyFileToClipboardInDataUriFormat(const CString& fileName, int Format, int Quality, bool html);
bool SaveImageFromCliboardDataUriFormat(const CString& clipboardText, CString& fileName);
// Load image from file with webp support
std::unique_ptr<Gdiplus::Bitmap> LoadImageFromFileExtended(const CString& fileName);
CString GdiplusStatusToString(Gdiplus::Status statusID);
bool ExUtilReadFile(const wchar_t* const file_name, uint8_t** data, size_t* data_size);
short GetImageOrientation(Gdiplus::Image* img);
bool RotateAccordingToOrientation(short orient, Gdiplus::Image* img, bool removeTag = false);
ImageInfo GetImageInfo(const wchar_t* fileName);
CString GdiplusStatusToString(Gdiplus::Status statusID);

}

#endif
