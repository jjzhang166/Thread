#include "StdAfx.h"
#include "Threads.h"


extern CEvent g_eventEnd;

UINT ThreadDraw(PVOID pParam)
{
    static int snCount = 0;
    snCount++;
    TRACE(TEXT("- ThreadDraw %d: started...\n"), snCount);

    THREADINFO *pInfo = reinterpret_cast<THREADINFO *> (pParam);

    CWnd *pWnd = CWnd::FromHandle(pInfo->hWnd);

    CClientDC dc(pWnd);

    int x = pInfo->point.x;
    int y = pInfo->point.y;

    srand((UINT)time(NULL));
    CRect rectEllipse(x - 25, y - 25, x + 25, y + 25);

    CSize sizeOffset(1, 1);

    CBrush brush(RGB(rand() % 256, rand() % 256, rand() % 256));
    CBrush *pOld = dc.SelectObject(&brush);
    while (WAIT_TIMEOUT == ::WaitForSingleObject(g_eventEnd, 0))
    {
        CRect rectClient;
        pWnd->GetClientRect(rectClient);

        if (rectEllipse.left < rectClient.left || rectEllipse.right > rectClient.right)
        {
            sizeOffset.cx *= -1;
        }

        if (rectEllipse.top < rectClient.top || rectEllipse.bottom > rectClient.bottom)
        {
            sizeOffset.cy *= -1;
        }

        dc.FillRect(rectEllipse, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));

        rectEllipse.OffsetRect(sizeOffset);

        dc.Ellipse(rectEllipse);
        Sleep(25);
    }

    dc.SelectObject(pOld);

    delete pInfo;

    TRACE(TEXT("- ThreadDraw %d: exiting.\n"), snCount--);
    return 0;
}
