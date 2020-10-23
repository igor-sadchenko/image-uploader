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
#ifndef THUMBSVIEW_H
#define THUMBSVIEW_H


#pragma once
#include "atlheaders.h"
#include "3rdpart/thread.h"
#include "Gui/Controls/ImageView.h"
#include "Gui/CommonDefines.h"

// CThumbsView

struct ThumbsViewItem
{
    CString FileName;
    BOOL ThumbOutDate;
};



class CThumbsView :
    public CWindowImpl<CThumbsView, CListViewCtrl>, public CThreadImpl<CThumbsView>, public CImageViewCallback
{
public:
    
    CImageList ImageList;
    CThumbsView();
    ~CThumbsView();
    DECLARE_WND_SUPERCLASS(_T("CThumbsView"), CListViewCtrl::GetWndClassName())
    
    BEGIN_MSG_MAP(CThumbsView)
        MESSAGE_HANDLER(WM_MBUTTONUP, OnMButtonUp)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        MSG_WM_KEYDOWN(OnKeyDown)
        MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
        REFLECTED_NOTIFY_CODE_HANDLER(LVN_BEGINDRAG, OnLvnBeginDrag)
        REFLECTED_NOTIFY_CODE_HANDLER(LVN_DELETEITEM, OnDeleteItem)
        REFLECTED_NOTIFY_CODE_HANDLER(LVN_DELETEALLITEMS, OnDeleteItem)
        REFLECTED_NOTIFY_CODE_HANDLER(LVN_ITEMCHANGED, OnItemChanged)
    END_MSG_MAP()

    // Handler prototypes:
    //  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    //  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    //  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
    LRESULT OnLvnBeginDrag(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
    LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnItemChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
    typedef std::function<void(CThumbsView*, bool)> ItemCountChangedCallback;
    void SetOnItemCountChanged(ItemCountChangedCallback&& callback);
    bool m_NeedUpdate;
    CAutoCriticalSection ImageListCS;
    LRESULT OnMButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    int maxwidth,maxheight;
    void Init(bool Extended=false);
    int AddImage(LPCTSTR FileName, LPCTSTR Title, bool ensureVisible =false, Gdiplus::Image* Img=NULL);
    bool MyDeleteItem(int ItemIndex);
    int DeleteSelected();
    void UpdateImageIndexes(int StartIndex = 0);
    void MyDeleteAllItems();
    bool SimpleDelete(int ItemIndex, bool DeleteThumb = true, bool deleteFile = false);
    LPCTSTR GetFileName(int ItemIndex);
    LRESULT OnKeyDown(TCHAR vk, UINT cRepeat, UINT flags);
    bool LoadThumbnail(int ItemID, Gdiplus::Image *Img=NULL);
    int GetImageIndex(int ItemIndex);
    CImageView ImageView;
    LRESULT OnLButtonDblClk(UINT Flags, CPoint Pt);
    DWORD Run();
    void LoadThumbnails();
    void StopLoadingThumbnails();
    void ViewSelectedImage();
    bool ExtendedView;
    void OutDateThumb(int nIndex);
    void UpdateOutdated();
    void LockImagelist(bool bLock = true);
    bool StopBackgroundThread(bool wait = false);
    void SelectLastItem();
    bool CopySelectedItemsToClipboard();
    LRESULT OnDeleteItem(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
    void SetDeletePhysicalFiles(bool doDelete);
    CImageViewItem getNextImgViewItem(CImageViewItem currentItem) override;
    CImageViewItem getPrevImgViewItem(CImageViewItem currentItem) override;
protected:
    ItemCountChangedCallback callback_;
    DWORD callbackLastCallTime_;
    bool deletePhysicalFiles_;
    void NotifyItemCountChanged(bool selected = true);
};



#endif // THUMBSVIEW_H