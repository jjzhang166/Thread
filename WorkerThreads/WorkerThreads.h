// WorkerThreads.h : main header file for the WorkerThreads application
//
#pragma once

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// CWorkerThreadsApp:
// See WorkerThreads.cpp for the implementation of this class
//

class CWorkerThreadsApp : public CWinApp
{
public:
    CWorkerThreadsApp();


    // Overrides
public:
    virtual BOOL InitInstance();

    // Implementation
    afx_msg void OnAppAbout();
    DECLARE_MESSAGE_MAP()
};

extern CWorkerThreadsApp theApp;