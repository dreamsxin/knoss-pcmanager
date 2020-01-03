#ifndef SKINGDI_INC_
#define SKINGDI_INC_

//////////////////////////////////////////////////////////////////////////

inline void GetBitmapSize(HBITMAP hBitmap, int& nWidth, int& nHeight)
{
    BITMAP bm;
    ::GetObject(hBitmap, sizeof(BITMAP), (PSTR)&bm); 
    nWidth = bm.bmWidth;
    nHeight = bm.bmHeight;
}

inline void DrawGradualColorRect(
    HDC dc,
    const RECT& drawRC, 
    COLORREF clrFirst = RGB(255, 255, 255),
    COLORREF clrEnd = RGB(0, 0, 0),
    BOOL fVertical = TRUE
    ) 
{
    if (fVertical)
    {
        GRADIENT_RECT gRect = {0, 1};

        TRIVERTEX vert[2] = {
            {drawRC.left , drawRC.top   , 0, 0, 0, 0}, 
            {drawRC.right, drawRC.bottom, 0, 0, 0, 0} };

            vert[0].Red     = GetRValue(clrFirst) << 8;
            vert[0].Green   = GetGValue(clrFirst) << 8;
            vert[0].Blue    = GetBValue(clrFirst) << 8;
            vert[1].Red     = GetRValue(clrEnd) << 8;
            vert[1].Green   = GetGValue(clrEnd) << 8;
            vert[1].Blue    = GetBValue(clrEnd) << 8;

            ::GradientFill(dc, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_V);
    }
    else
    {
        GRADIENT_RECT gRect = {0, 1};

        TRIVERTEX vert[2] = {
            {drawRC.left , drawRC.top   , 0, 0, 0, 0}, 
            {drawRC.right, drawRC.bottom, 0, 0, 0, 0} };

            vert[0].Red     = GetRValue(clrFirst) << 8;
            vert[0].Green   = GetGValue(clrFirst) << 8;
            vert[0].Blue    = GetBValue(clrFirst) << 8;
            vert[1].Red     = GetRValue(clrEnd) << 8;
            vert[1].Green   = GetGValue(clrEnd) << 8;
            vert[1].Blue    = GetBValue(clrEnd) << 8;

            ::GradientFill(dc, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_H);
    }
}
/*
inline void DrawFrame(CPaintDC& dc, RECT& rcFrame, const CString& strTitle) {
    RECT rcLeft = { rcFrame.left, rcFrame.top, rcFrame.left + 1, rcFrame.bottom };
    RECT rcTop = { rcFrame.left, rcFrame.top, rcFrame.right, rcFrame.top + 1 };
    RECT rcRight = { rcFrame.right - 1, rcFrame.top, rcFrame.right, rcFrame.bottom };
    RECT rcBottom = { rcFrame.left, rcFrame.bottom -1, rcFrame.right, rcFrame.bottom };

    dc.FillSolidRect(&rcLeft, RGB(163, 186, 210));
    dc.FillSolidRect(&rcTop, RGB(163, 186, 210));
    dc.FillSolidRect(&rcRight, RGB(163, 186, 210));
    dc.FillSolidRect(&rcBottom, RGB(163, 186, 210));

    RECT rcHeaderSolid1 = { rcFrame.left + 1, rcFrame.top + 1, rcFrame.right - 1, rcFrame.top + 12 + 1};
    RECT rcHeaderSolid2 = { rcFrame.left + 1, rcFrame.top + 12 + 1, rcFrame.right - 1, rcFrame.top + 24 - 1 };
    RECT rcHeaderBottom = { rcFrame.left + 1, rcFrame.top + 23, rcFrame.right, rcFrame.top + 24 };

    dc.FillSolidRect(&rcHeaderBottom, RGB(210, 210, 210));
    dc.FillSolidRect(&rcHeaderSolid1, RGB(255, 255, 255));
    SkinDrawGradualColorRect(dc, rcHeaderSolid2, RGB(247, 248, 250), RGB(241, 242, 244));

    RECT rcHeader = { rcFrame.left + 1, rcFrame.top + 1, rcFrame.right - 1, rcFrame.top + 23 };
    rcHeader.left += 12;
    dc.SetBkMode(TRANSPARENT);
    dc.SetTextColor(RGB(0, 0, 0));
    dc.DrawText(strTitle, strTitle.GetLength(), &rcHeader, DT_VCENTER|DT_SINGLELINE);

    RECT rcMain = { rcFrame.left + 1, rcFrame.top + 24, rcFrame.right - 1, rcFrame.bottom - 1};
    dc.FillSolidRect(&rcMain, RGB(0xFF, 0xFF, 0xFF));
}
*/

//////////////////////////////////////////////////////////////////////////

#endif	// SKINGDI_INC_
