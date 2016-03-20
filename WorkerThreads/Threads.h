#pragma once

struct THREADINFO
{
    HWND hWnd;
    POINT point;
};

UINT ThreadDraw(PVOID pParam);
