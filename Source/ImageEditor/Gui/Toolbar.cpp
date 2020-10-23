#include "Toolbar.h"

#include "Core/Logging.h"
#include "3rdpart/GdiplusH.h"
#include "Gui/GuiTools.h"
#include "3rdpart/RoundRect.h"
#include "resource.h"
#include "Core/Images/Utils.h"
#include "Func/WinUtils.h"
#include "../Canvas.h"

namespace ImageEditor {

Toolbar::Toolbar(Toolbar::Orientation orientation)
{
    orientation_ = orientation;
    selectedItemIndex_ = -1;
    trackMouse_ = false;
    m_hWnd = 0;
    dropDownIcon_ = ImageUtils::BitmapFromResource(GetModuleHandle(0), MAKEINTRESOURCE(IDB_DROPDOWNICONPNG),_T("PNG")); //(HICON)LoadImage(GetModuleHandle(0),  MAKEINTRESOURCE(IDI_DROPDOWN), IMAGE_ICON, 16,16,0);
    dpiScaleX_ = 1.0f;
    dpiScaleY_ = 1.0f;
    transparentColor_ = Color(255,50,56);
    if ( !WinUtils::IsWine() ) {
        subpanelColor_ = Color(252,252,252);
    } else {
        subpanelColor_.SetFromCOLORREF(GetSysColor(COLOR_BTNFACE));
    }
    
    subpanelBrush_.CreateSolidBrush(subpanelColor_.ToCOLORREF());
    memset(&buttonsRect_, 0, sizeof(buttonsRect_));
    font_ = 0;
    textRenderingHint_ = Gdiplus::TextRenderingHintAntiAlias;
    oldSelectedBm_ = nullptr;
}

Toolbar::~Toolbar()
{
    delete font_;
    if (!backBufferDc_.IsNull()) {
        backBufferDc_.SelectBitmap(oldSelectedBm_);
    }
    if (!backBuffer_.IsNull()) {
        backBuffer_.DeleteObject();
    }
}

bool Toolbar::Create(HWND parent, bool child )
{
    RECT rc = {0, 0, 1,1};

    if ( orientation_ == orHorizontal ) {
        rc.left = 60;
        rc.right = 70;
    } else {
        rc.top = 60;
        rc.bottom = 70;
    }

    // Move toolbars to the top left corner of parent window in fullscreen mode
    if (!child) {
        CRect parentRect;
        ::GetWindowRect(parent, parentRect);
        OffsetRect(&rc, parentRect.left, parentRect.top);
    }
    HWND wnd = TParent::Create(parent, rc, _T("test"), (child ? WS_CHILD : WS_POPUP | WS_CLIPCHILDREN), child ? 0 : (WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST) /*|WS_EX_TOOLWINDOW*/);
    if ( !wnd ) {
        LOG(ERROR) << WinUtils::GetLastErrorAsString();
        return false;
    }

    
    return true;
}

int Toolbar::addButton(const Item& item)
{
    buttons_.push_back(item);
    return buttons_.size()-1;
}

int Toolbar::getItemAtPos(int clientX, int clientY)
{
    POINT pt = {clientX, clientY};
    for (size_t i = 0; i < buttons_.size(); i++) {
        if ( PtInRect(&buttons_[i].rect,pt ) ) {
            return i;
        }
    }
    return -1;
}

int Toolbar::getItemIndexByCommand(int command)
{
    for (size_t i = 0; i < buttons_.size(); i++) {
        if (buttons_[i].command == command ) {
            return i;
        }
    }
    return -1;
}

Toolbar::Item* Toolbar::getItem(int index)
{
    return &buttons_[index];
}

void Toolbar::repaintItem(int index)
{
    Item item = buttons_[index];
    InvalidateRect(&item.rect, false);
}

void Toolbar::clickButton(int index)
{
    if ( index < 0 || index >= static_cast<int>(buttons_.size()) ) {
        LOG(ERROR) << "Out of range";
    }
    Item& item = buttons_[index];
    selectedItemIndex_ = index;
    if ( item.checkable ) {

        item.isChecked = item.group!=-1|| !item.isChecked;
        //item.state = item.isChecked ? isChecked : isNormal;
    } else {
        //item.state = isNormal;
    }
    if ( item.group != -1 ) {

        // Uncheck all other buttons with same group id
        for (size_t i = 0; i < buttons_.size(); i++) {
            if ( i != static_cast<size_t>(index) && buttons_[i].group == item.group && buttons_[i].checkable && buttons_[i].isChecked ) {
                buttons_[i].isChecked  = false;
                buttons_[i].state = isNormal;
                InvalidateRect(&buttons_[i].rect, FALSE);
            }
        }
    }

    InvalidateRect(&item.rect, FALSE);
    
}

LRESULT Toolbar::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    LONG lStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
    if ( lStyle & WS_CHILD ) {
        transparentColor_.SetFromCOLORREF(GetSysColor(COLOR_APPWORKSPACE));
    } else {
        SetLayeredWindowAttributes(m_hWnd, RGB(transparentColor_.GetR(),transparentColor_.GetG(),transparentColor_.GetB()),0,LWA_COLORKEY);

    }
    //lStyle &= ~(WS_CAPTION /*| WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU*/);
    //::SetWindowLong(m_hWnd, GWL_STYLE, lStyle);
    CDC hdc = GetDC();
    dpiScaleX_ = GetDeviceCaps(hdc, LOGPIXELSX) / 96.0f;
    dpiScaleY_ = GetDeviceCaps(hdc, LOGPIXELSY) / 96.0f;
    backBufferDc_.CreateCompatibleDC(hdc);

    systemFont_ = GuiTools::GetSystemDialogFont();
    LOGFONT logFont;
    systemFont_.GetLogFont(&logFont);
    if ( logFont.lfQuality & CLEARTYPE_QUALITY ) {
        textRenderingHint_ = Gdiplus::TextRenderingHintClearTypeGridFit;
    } else if ( logFont.lfQuality & ANTIALIASED_QUALITY ) {
        textRenderingHint_ = Gdiplus::TextRenderingHintAntiAliasGridFit;
    }

    enum {kItemMargin = 3, kItemHorPadding = 5, kItemVertPadding = 3, kIconSize = 20};
    itemMargin_ = static_cast<int>(kItemMargin *dpiScaleX_);
    itemHorPadding_ = static_cast<int>(kItemHorPadding * dpiScaleX_);
    itemVertPadding_ = static_cast<int>(kItemVertPadding * dpiScaleY_);
    iconSizeX_ = static_cast<int>(kIconSize * dpiScaleX_);
    iconSizeY_ = static_cast<int>(kIconSize * dpiScaleY_);
    font_ = new Gdiplus::Font(hdc, systemFont_);
    subpanelHeight_ = static_cast<int>(25 * dpiScaleY_);
    subpanelLeftOffset_ = static_cast<int>(50 * dpiScaleX_);
    RECT sliderRect = { 0, 0, static_cast<LONG>(100 * dpiScaleX_), static_cast<LONG>(subpanelHeight_ - 2 * dpiScaleY_ ) };
    if ( orientation_ == orHorizontal ) {
        penSizeSlider_.Create(m_hWnd, sliderRect, 0, WS_CHILD|WS_VISIBLE|TBS_NOTICKS);
        createHintForSliders(penSizeSlider_.m_hWnd, TR("Line thickness"));
        RECT pixelLabelRect = { 0, 0, static_cast<LONG>(45 * dpiScaleX_), static_cast<LONG>(subpanelHeight_ - 5 * dpiScaleY_) };
        pixelLabel_.Create(m_hWnd, pixelLabelRect, L"px", WS_CHILD|WS_VISIBLE);
        pixelLabel_.SetFont(systemFont_);
        //createHintForSliders(pixelLabel_.m_hWnd, TR("Line thickness"));

        RECT radiusSliderRect = { 0, 0, static_cast<LONG>(100 * dpiScaleX_), static_cast<LONG>(subpanelHeight_ - 2 * dpiScaleY_ ) };
        roundRadiusSlider_.Create(m_hWnd, radiusSliderRect, 0, WS_CHILD|TBS_NOTICKS);
        createHintForSliders(roundRadiusSlider_.m_hWnd, TR("Rounding radius"));
        //RECT radiusLabelRect = { 0, 0, static_cast<LONG>(45 * dpiScaleX_), static_cast<LONG>(subpanelHeight_ - 5 * dpiScaleY_) };
        roundRadiusLabel_.Create(m_hWnd, pixelLabelRect, L"px", WS_CHILD);
        roundRadiusLabel_.SetFont(systemFont_);
        //createHintForSliders(roundRadiusLabel_.m_hWnd, TR("Rounding radius"));
    }
    return 0;
}

LRESULT Toolbar::OnHScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    if ( (HWND)lParam == penSizeSlider_.m_hWnd ||   (HWND)lParam == roundRadiusSlider_.m_hWnd ) {
        ::SendMessage(GetParent(),uMsg, wParam, lParam);
    }
    return 0;
}

LRESULT Toolbar::OnColorStatic(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if ( WinUtils::IsWine() ) {
        HDC dc = (HDC) wParam;
        SetBkMode(dc, TRANSPARENT);
        return (LRESULT)GetSysColorBrush(COLOR_BTNFACE);
    }
    return (LRESULT)(HBRUSH)subpanelBrush_;
}

LRESULT Toolbar::OnActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    /*if ( wParam == WA_CLICKACTIVE  || wParam == WA_ACTIVE) {
        ::SetActiveWindow((HWND)lParam);
        bHandled = true;
    }*/
    return 0;
}


LRESULT Toolbar::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    using namespace Gdiplus;
    CDC &dc = backBufferDc_;
   
    CRect clientRect;
    bHandled = true;
    GetClientRect(&clientRect);
    Gdiplus::Graphics gr(dc);
    gr.SetInterpolationMode(InterpolationModeHighQualityBicubic );
    gr.SetPageUnit(Gdiplus::UnitPixel);

    Rect rect( 0, 0, clientRect.right, clientRect.bottom);
    SolidBrush br1(transparentColor_);
    gr.FillRectangle(&br1, rect);
        Pen p(Color(1,87,124));
    LinearGradientBrush br (RectF(float(0), float(-0.5 ), float(buttonsRect_.right),
            /*rect.top+*/ float(buttonsRect_.bottom) ), Color(252,252,252), Color(
            200,200,200), orientation_ == orHorizontal ? LinearGradientModeVertical : LinearGradientModeHorizontal);

    rect = Rect(subpanelLeftOffset_, static_cast<INT>(buttonsRect_.bottom - 2 * dpiScaleY_), static_cast<INT>(kSubpanelWidth*dpiScaleX_), clientRect.bottom);
    rect.Height -= rect.Y;

    SolidBrush br2 (subpanelColor_);
    rect.Width--;
    rect.Height --;
    ImageUtils::DrawRoundedRectangle(&gr,rect,7,&p, &br2);


    rect = Rect( 0, 0, buttonsRect_.right, buttonsRect_.bottom);
    rect.Width--;
    rect.Height --;
    
    //Gdiplus::SolidBrush br(Color(200,2,146,209));

    gr.SetSmoothingMode(GetStyle()&WS_CHILD ? SmoothingModeAntiAlias : SmoothingModeDefault);
    ImageUtils::DrawRoundedRectangle(&gr,rect,7,&p, &br);

    gr.SetSmoothingMode(SmoothingModeAntiAlias);

    //gr.FillRectangle(&br,rect);
    int x = itemMargin_;
    int y = itemMargin_;
    for (size_t i = 0; i < buttons_.size(); i++) {
        SIZE s = CalcItemSize(i);
        drawItem(i, &gr, x, y);

        if ( orientation_ == orHorizontal ) {
            x += s.cx + itemMargin_;
        } else {
            y += s.cy + itemMargin_;
        }
        //y+= s.cy;cli
    }

    if ( !(GetStyle()&WS_CHILD)  ) { // fix artefacts
        SolidBrush br(transparentColor_);
        gr.SetPixelOffsetMode(PixelOffsetModeHighQuality);
        gr.FillRectangle(&br, Rect(buttonsRect_.right-1, buttonsRect_.bottom-1, 1,1)); 
    }

    CPaintDC realDc(m_hWnd);
    realDc.BitBlt(0, 0, clientRect.Width(), clientRect.Height(), dc, 0, 0, SRCCOPY);

    return 0;
}

LRESULT Toolbar::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
    int xPos = GET_X_LPARAM(lParam); 
    int yPos = GET_Y_LPARAM(lParam); 
    
    if(!trackMouse_) // Capturing mouse
    {
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(TRACKMOUSEEVENT);
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = m_hWnd;
        ::_TrackMouseEvent(&tme); // We want to receive WM_MOUSELEAVE message
        trackMouse_ = true;
    }

    int oldSelectedIndex  = selectedItemIndex_;
    
    selectedItemIndex_ = getItemAtPos(xPos, yPos);

    if (  oldSelectedIndex != selectedItemIndex_  ) {
        if ( selectedItemIndex_ != -1 ) {
            buttons_[selectedItemIndex_].state = isHover;
            if ( selectedItemIndex_ != -1 ) {
                buttons_[selectedItemIndex_].state = isHover;
                InvalidateRect(&buttons_[selectedItemIndex_].rect, false);
                //Invalidate();
            }
        }
        if ( oldSelectedIndex != -1 ) {
            buttons_[oldSelectedIndex].state = isNormal;
            InvalidateRect(&buttons_[oldSelectedIndex].rect, false);
        }
    } 
    return 0;
}

LRESULT Toolbar::OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
    /*int xPos = GET_X_LPARAM(lParam); 
    int yPos = GET_Y_LPARAM(lParam); */
    trackMouse_ = false;
    if ( selectedItemIndex_ != -1 ) {
        buttons_[selectedItemIndex_].state = isNormal;
        
        InvalidateRect(&buttons_[selectedItemIndex_].rect, false);
        selectedItemIndex_ = -1;
        trackMouse_ = false;
    }

    return 0;
}

LRESULT Toolbar::OnLButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    int xPos = GET_X_LPARAM(lParam); 
    int yPos = GET_Y_LPARAM(lParam); 
    if ( selectedItemIndex_ != -1 ) {
        Item& item = buttons_[selectedItemIndex_];
        if ( item.type == Toolbar::itComboButton && xPos >  static_cast<int>(item.rect.right - dropDownIcon_->GetWidth() - itemMargin_)  ) {
            item.state = isDropDown;
        } else if ( item.type == Toolbar::itTinyCombo ) {
            if ( xPos >  item.rect.right - 6*dpiScaleX_ - itemMargin_ && yPos >   item.rect.bottom - 6*dpiScaleY_ - itemMargin_ ) {
                item.state = isDropDown;
            } else {
                SetTimer(kTinyComboDropdownTimer, 600);
                item.state = isDown;
            }
        }
        else {
            item.state = isDown;
        }
        
        InvalidateRect(&item.rect, false);
        
    }
    
    return 0;
}

LRESULT Toolbar::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    if ( wParam  == kTinyComboDropdownTimer && selectedItemIndex_ != -1 ) {
        Item& item = buttons_[selectedItemIndex_];
        if (  item.type == Toolbar::itTinyCombo ) {
            ::PostMessage(GetParent(), MTBM_DROPDOWNCLICKED, (WPARAM)&item,(LPARAM)m_hWnd);
        }
    }
    KillTimer(kTinyComboDropdownTimer);
    return 0;
}


LRESULT Toolbar::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = true;
    return ::SendMessage(GetParent(), uMsg, wParam, lParam);
}

LRESULT Toolbar::OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = true;
    return ::SendMessage(GetParent(), uMsg, wParam, lParam);
}

LRESULT Toolbar::OnLButtonUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    int xPos = GET_X_LPARAM(lParam); 
    int yPos = GET_Y_LPARAM(lParam); 
    RECT clientRect;
    KillTimer(kTinyComboDropdownTimer);
    GetClientRect(&clientRect);
    POINT pt = { xPos, yPos };
    if ( !::PtInRect(&clientRect, pt)  )  {
        return 0;
    }
    if ( selectedItemIndex_ != -1 ) {
        
        selectedItemIndex_ = getItemAtPos(xPos, yPos);
        Item& item = buttons_[selectedItemIndex_];

        

        if ( item.itemDelegate ) {
            clickButton(selectedItemIndex_);
            item.itemDelegate->OnClick(xPos, yPos, dpiScaleX_, dpiScaleY_);
        } else {
            HWND parent = GetParent();
            int command = item.command;
            if ( item.type == Toolbar::itComboButton && xPos > static_cast<int>( item.rect.right - dropDownIcon_->GetWidth() - itemMargin_ ) ) {
                ::SendMessage(parent, MTBM_DROPDOWNCLICKED, (WPARAM)&item,(LPARAM)m_hWnd);
            } else if ( item.type == Toolbar::itTinyCombo && xPos >  item.rect.right - 6*dpiScaleX_ - itemMargin_ && yPos >   item.rect.bottom - 6*dpiScaleY_ - itemMargin_  ) {
                ::SendMessage(parent, MTBM_DROPDOWNCLICKED, (WPARAM)&item,(LPARAM)m_hWnd);
            }else {
                ::SendMessage(parent, WM_COMMAND, MAKEWPARAM(command,BN_CLICKED),(LPARAM)m_hWnd);
            }

            
            selectedItemIndex_ = -1;
            OnMouseMove(WM_MOUSEMOVE, wParam, lParam, bHandled);
        }
    }
    
    return 0;
}

LRESULT Toolbar::OnRButtonUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    int xPos = GET_X_LPARAM(lParam); 
    int yPos = GET_Y_LPARAM(lParam); 
    if ( selectedItemIndex_ != -1 ) {

        selectedItemIndex_ = getItemAtPos(xPos, yPos);
        Item& item = buttons_[selectedItemIndex_];

        if ( item.type == Toolbar::itTinyCombo ) {
            HWND parent = GetParent();
            ::SendMessage(parent, MTBM_DROPDOWNCLICKED, (WPARAM)&item,(LPARAM)m_hWnd);
            selectedItemIndex_ = -1;
            OnMouseMove(WM_MOUSEMOVE, wParam, lParam, bHandled);
        }
    }
    return 0;
}

LRESULT Toolbar::OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    return 1; // avoid flicker
}

LRESULT Toolbar::OnNcHitTest(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    bHandled = false;
    if ( ::GetKeyState(VK_MENU) & 0x8000 ) {
        bHandled = true;
        return HTCAPTION;
    }
    return 0;
}

SIZE Toolbar::CalcItemSize(int index)
{
    using namespace Gdiplus;
    SIZE res={0,0};
    Item &item = buttons_[index];
    
    if ( item.itemDelegate ) {
        return item.itemDelegate->CalcItemSize(item,dpiScaleX_, dpiScaleY_);
    }

    if (  item.title.GetLength()) {
        CWindowDC dc(m_hWnd);
        Gdiplus::Graphics gr(dc);
        PointF origin(0,0);
        RectF textBoundingBox;
        if (  gr.MeasureString(item.title, item.title.GetLength(), font_, origin, &textBoundingBox) == Ok ) {
            res.cx = static_cast<LONG>(textBoundingBox.Width);
            res.cy = static_cast<LONG>(textBoundingBox.Height);
        }
    }

    if ( item.icon ) {
        res.cx += iconSizeX_  + (item.title.IsEmpty() ? 0 :itemHorPadding_ ) ;
        res.cy = std::max<int>(iconSizeY_, res.cy);
    }

    if ( item.type == itComboButton ) {
        res.cx += static_cast<int>(dropDownIcon_ ->GetWidth()*dpiScaleX_) + itemHorPadding_;
    }/*else if ( item.type == itTinyCombo ) {
        res.cx += 5*dpiScaleX;
    }*/

    res.cx += itemHorPadding_ * 2;
    res.cy  += itemVertPadding_ * 2;
    RECT rc = buttonsRect_;
    if ( orientation_ == orVertical ) {
        res.cx =  max( res.cx, rc.right - itemMargin_*2);
    } else {
        res.cy =  max( res.cy, rc.bottom - itemMargin_*2);
    }
    return res;
}


int Toolbar::AutoSize()
{
    int x = itemMargin_;
    int y = itemMargin_;
    int width = 0;
    int height = 0;
    for ( int j = 0; j < 1; j ++ ) {
        x = itemMargin_;
        y = itemMargin_;
        for (size_t i = 0; i < buttons_.size(); i++) {
            SIZE s = CalcItemSize(i);
            Item& item = buttons_[i];
            RectF bounds(static_cast<Gdiplus::REAL>(x), static_cast<Gdiplus::REAL>(y), float(s.cx), float(s.cy));
            item.rect.left = x;
            item.rect.top = y;
            item.rect.right = s.cx + x;
            item.rect.bottom = s.cy + y;

            if ( orientation_ == orHorizontal ) {
                x+= s.cx + itemMargin_;
                width = std::max<int>(x, subpanelLeftOffset_ + static_cast<int>((kSubpanelWidth + 20 )*dpiScaleX_));
                height = std::max<int>(s.cy + itemMargin_*2, height);
            } else {
                y+= s.cy + itemMargin_;
                height = y;
                width = std::max<int>(s.cx+itemMargin_*2, width);
            }
        }
        if ( j == 0 ) {
            SetWindowPos(0, 0,0,width,height,SWP_NOMOVE);
        }
    }

    GetClientRect(&buttonsRect_);

    if ( orientation_ == orHorizontal ) {
        SetWindowPos(0, 0,0,width,height + subpanelHeight_,SWP_NOMOVE);
        penSizeSlider_.SetWindowPos(0, subpanelLeftOffset_ + static_cast<int>(3 * dpiScaleX_), static_cast<int>(buttonsRect_.bottom + dpiScaleY_), 0, 0, SWP_NOSIZE);
        penSizeSlider_.SetRange(1,Canvas::kMaxPenSize);
        RECT penSizeSliderRect;
        penSizeSlider_.GetClientRect(&penSizeSliderRect);
        penSizeSlider_.ClientToScreen(&penSizeSliderRect);
        ScreenToClient(&penSizeSliderRect);
        pixelLabel_.SetWindowPos(0, penSizeSliderRect.right, static_cast<int>(buttonsRect_.bottom + 3 * dpiScaleY_), 0, 0, SWP_NOSIZE);


        roundRadiusSlider_.SetWindowPos(0, subpanelLeftOffset_ + static_cast<int>(150 * dpiScaleX_), static_cast<int>(buttonsRect_.bottom + 1 * dpiScaleY_), 0, 0, SWP_NOSIZE);
        roundRadiusSlider_.SetRange(1,Canvas::kMaxRoundingRadius);
        RECT radiusSliderRect;
        roundRadiusSlider_.GetClientRect(&radiusSliderRect);
        roundRadiusSlider_.ClientToScreen(&radiusSliderRect);
        ScreenToClient(&radiusSliderRect);
        roundRadiusLabel_.SetWindowPos(0, radiusSliderRect.right, buttonsRect_.bottom + static_cast<int>(3 * dpiScaleY_), 0, 0, SWP_NOSIZE);
    }

    for (size_t i = 0; i < buttons_.size(); i++) {
        CreateToolTipForItem(i);
    }

    CRect clientRect;
    GetClientRect(&clientRect);
    CWindowDC dc(m_hWnd);
    if (backBuffer_.m_hBitmap) {
        backBufferDc_.SelectBitmap(oldSelectedBm_);
        backBuffer_.DeleteObject();
    }

    backBuffer_.CreateCompatibleBitmap(dc, clientRect.Width(), clientRect.Height());
    oldSelectedBm_ = backBufferDc_.SelectBitmap(backBuffer_);
    return 1;
}

void Toolbar::drawItem(int itemIndex, Gdiplus::Graphics* gr, int x, int y)
{
    using namespace Gdiplus;
    SIZE size = CalcItemSize(itemIndex);
    
    Item& item = buttons_[itemIndex];

    if ( item.itemDelegate ) {
        item.itemDelegate->DrawItem(item, gr, x, y, dpiScaleX_, dpiScaleY_);
        return;
    }

    RectF bounds(static_cast<Gdiplus::REAL>(x), static_cast<Gdiplus::REAL>(y), float(size.cx), float(size.cy));
    item.rect.left = x;
    item.rect.top = y;
    item.rect.right = size.cx + x;
    item.rect.bottom = size.cy + y;
    SolidBrush brush(Color(0,0,0));
    
    if ( item.state == isHover ||  item.state == isDown ||  item.state == isDropDown || item.isChecked) {
            Pen p(Color(198,196,197));
            Color gradientColor1 = item.isChecked ?  Color(170,170,170) : Color(232,232,232);
            Color gradientColor2 = item.isChecked ? Color(130,130,130) : Color(170,170,170);
            LinearGradientBrush br (RectF(float(x), float(y ), float( x+size.cx),
                /*rect.top+*/ float(y+size.cy ) ), gradientColor1, gradientColor2, LinearGradientModeVertical);

            CRoundRect roundRect;
            roundRect.FillRoundRect(gr,&br,Rect(x, y, size.cx, size.cy),Color(198,196,197),4);

            if ( item.type == itComboButton ) {
                gr->DrawLine(&p, bounds.X + bounds.Width - dropDownIcon_->GetWidth()-3 ,  bounds.Y+1 , bounds.X + bounds.Width - dropDownIcon_->GetWidth()-3, bounds.Y + bounds.Height -1 );
            }
        
    } /*else if ( item.state == isChecked ) {
        Pen p(Color(198,196,197));
        Color gradientColor1 = Color(200,200,200);
        Color gradientColor2 = Color(140,140,140);
        LinearGradientBrush br (RectF(float(0), float(0.5 ), float( size.cx),
            /*rect.top+* float(size.cy ) ), gradientColor1, gradientColor2, LinearGradientModeVertical);
        //    gr->FillRectangle( &brush, Rect(x, y, size.cx, size.cy));
        //br.TranslateTransform(x,y);
        //br.SetWrapMode(WrapModeTile);
        /*CRoundRect roundRect;
        roundRect.FillRoundRect(gr,&br,Rect(x, y, size.cx, size.cy),Color(198,196,197),4);
        //roundRect.DrawRoundRect(gr,Rect(x, y, size.cx, size.cy),Color(198,196,197),7, 1);
        //DrawRoundedRectangle(gr,Rect(x, y, size.cx, size.cy),8,&p, 0);
        /*if ( item.type == itComboButton ) {
            gr->DrawLine(&p, bounds.X + bounds.Width - dropDownIcon_->GetWidth()-3 ,  bounds.Y+1 , bounds.X + bounds.Width - dropDownIcon_->GetWidth()-3, bounds.Y + bounds.Height -1 );
        }*/

//    }

    if ( item.type == itComboButton ) {
        gr->DrawImage(dropDownIcon_.get(), bounds.X + bounds.Width - 16*dpiScaleX_+ (item.state == isDropDown ? 1 : 0), bounds.Y + (bounds.Height -16*dpiScaleY_ )/2 + (item.state == isDropDown ? 1 : 0), (int)16*dpiScaleX_, (int)16*dpiScaleY_);
    } else if ( item.type == itTinyCombo ) {
        gr->DrawImage(dropDownIcon_.get(), bounds.X + bounds.Width - 8*dpiScaleX_+ (item.state == isDropDown ? 1 : 0), bounds.Y + bounds.Height - 8*dpiScaleY_ + (item.state == isDropDown ? 1 : 0), (int)10*dpiScaleX_, (int)10*dpiScaleY_);
    }

    if (  item.icon ) {
        gr->DrawImage(item.icon.get(),(int) (itemHorPadding_ + bounds.X+ (item.state == isDown ? 1 : 0)), (int)(bounds.Y+ (item.state == isDown ? 1 : 0)+(bounds.Height -iconSizeY_)/2),iconSizeX_, iconSizeY_);

    }

    StringFormat format;
    format.SetAlignment(StringAlignmentCenter);
    format.SetLineAlignment(StringAlignmentCenter);
    //gr->SetTextRenderingHint(textRenderingHint_);
    
    int iconWidth = (item.icon ? iconSizeX_/*+ itemHorPadding_*/:0 ) ;
    int textOffsetX =  iconWidth+ itemHorPadding_;
    RectF textBounds( textOffsetX + bounds.X + (item.state == isDown ? 1 : 0), bounds.Y+ (item.state == isDown ? 1 : 0), bounds.Width - textOffsetX - (item.type == itComboButton ? 16*dpiScaleX_ : 0), bounds.Height);
    if ( orientation_ == orVertical ) {
        //LOG(INFO) << "textBounds x="<<textBounds.X << " y = " << textBounds.Y << "   "<<item.title;
    }

    gr->DrawString(item.title, -1, font_, textBounds, &format, &brush);
}


void Toolbar::createHintForSliders(HWND slider, CString hint)
{
    // Create a tooltip.
    HWND hwndTT = ::CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL, 
        WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, 
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
        slider, NULL, _Module.GetModuleInstance(),NULL);

    ::SetWindowPos(hwndTT, HWND_TOPMOST, 0, 0, 0, 0, 
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    // Set up "tool" information. In this case, the "tool" is the entire parent window.

    TOOLINFO ti = { 0 };
    ti.cbSize   = sizeof(TOOLINFO);
    ti.uFlags   = TTF_SUBCLASS;
    ti.hwnd     = slider;
    ti.hinst    = _Module.GetModuleInstance();
    TCHAR* textBuffer = new TCHAR[hint.GetLength()+1];
    lstrcpy(textBuffer, hint);
    ti.lpszText = textBuffer;
    ::GetClientRect(slider, &ti.rect);

    // Associate the tooltip with the "tool" window.
    SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);    

    delete[] textBuffer;
}

void Toolbar::CreateToolTipForItem(size_t index)
{
    Item& item = buttons_[index];
    if ( item.tooltipWnd ) {
        ::DestroyWindow(item.tooltipWnd);
        item.tooltipWnd = 0;
    }
    if ( item.hint.IsEmpty() ) {
        return;
    }
    // Create a tooltip.
    HWND hwndTT = ::CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL, 
        WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, 
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
        m_hWnd, NULL, _Module.GetModuleInstance(),NULL);

    ::SetWindowPos(hwndTT, HWND_TOPMOST, 0, 0, 0, 0, 
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    // Set up "tool" information. In this case, the "tool" is the entire parent window.

    TOOLINFO ti = { 0 };
    ti.cbSize   = sizeof(TOOLINFO);
    ti.uFlags   = TTF_SUBCLASS;
    ti.hwnd     = m_hWnd;
    ti.hinst    = _Module.GetModuleInstance();
    TCHAR* textBuffer = new TCHAR[item.hint.GetLength()+1];
    lstrcpy(textBuffer, item.hint);
    ti.lpszText = textBuffer;
    ti.rect  = item.rect;

    // Associate the tooltip with the "tool" window.
    SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);    
    item.tooltipWnd = hwndTT;
    delete[] textBuffer;
} 

}