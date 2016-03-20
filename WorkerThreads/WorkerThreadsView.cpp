// WorkerThreadsView.cpp : implementation of the CWorkerThreadsView class
//

#include "stdafx.h"
#include "WorkerThreads.h"

#include "WorkerThreadsDoc.h"
#include "WorkerThreadsView.h"
#include ".\workerthreadsview.h"
#include "Threads.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// manual-reset event
CEvent g_eventEnd(FALSE, TRUE);


// CWorkerThreadsView

IMPLEMENT_DYNCREATE(CWorkerThreadsView, CView)

BEGIN_MESSAGE_MAP(CWorkerThreadsView, CView)
    ON_WM_LBUTTONDOWN()
    ON_WM_DESTROY()
END_MESSAGE_MAP()

// CWorkerThreadsView construction/destruction

CWorkerThreadsView::CWorkerThreadsView()
{
    // TODO: add construction code here

}

CWorkerThreadsView::~CWorkerThreadsView()
{
}

BOOL CWorkerThreadsView::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: Modify the Window class or styles here by modifying
    //  the CREATESTRUCT cs

    return CView::PreCreateWindow(cs);
}

// CWorkerThreadsView drawing

void CWorkerThreadsView::OnDraw(CDC* /*pDC*/)
{
    CWorkerThreadsDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    // TODO: add draw code for native data here
}


// CWorkerThreadsView diagnostics

#ifdef _DEBUG
void CWorkerThreadsView::AssertValid() const
{
    CView::AssertValid();
}

void CWorkerThreadsView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CWorkerThreadsDoc* CWorkerThreadsView::GetDocument() const // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWorkerThreadsDoc)));
    return (CWorkerThreadsDoc*)m_pDocument;
}
#endif //_DEBUG


// CWorkerThreadsView message handlers

void CWorkerThreadsView::OnLButtonDown(UINT nFlags, CPoint point)
{
    THREADINFO *pInfo = new THREADINFO;
    pInfo->hWnd = GetSafeHwnd();
    pInfo->point = point;

    CWinThread *pThread = AfxBeginThread(ThreadDraw, (PVOID)pInfo, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
    pThread->m_bAutoDelete = FALSE;
    pThread->ResumeThread();
    m_ThreadArray.Add(pThread);
}

void CWorkerThreadsView::OnDestroy()
{
    g_eventEnd.SetEvent();

    /*  // first way
        for (int j = 0; j < m_ThreadArray.GetSize(); j ++)
        {
            ::WaitForSingleObject(m_ThreadArray[j]->m_hThread, INFINITE);
            delete m_ThreadArray[j];
        }
    */

    //second way
    int nSize = m_ThreadArray.GetSize();
    HANDLE *p = new HANDLE[nSize];

    for (int j = 0; j < nSize; j++)
    {
        p[j] = m_ThreadArray[j]->m_hThread;
    }

    ::WaitForMultipleObjects(nSize, p, TRUE, INFINITE);

    for (int j = 0; j < nSize; j++)
    {
        delete m_ThreadArray[j];
    }

    delete[] p;


    TRACE("- CWorkerThreadsView::OnDestroy: finished!\n");
}
