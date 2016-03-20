// WorkerThreadsView.h : interface of the CWorkerThreadsView class
//


#pragma once


class CWorkerThreadsView : public CView
{

    CArray<CWinThread *, CWinThread *> m_ThreadArray;


protected: // create from serialization only
    CWorkerThreadsView();
    DECLARE_DYNCREATE(CWorkerThreadsView)

    // Attributes
public:
    CWorkerThreadsDoc* GetDocument() const;

    // Operations
public:

    // Overrides
public:
    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

    // Implementation
public:
    virtual ~CWorkerThreadsView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:

    // Generated message map functions
protected:
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnDestroy();
};

#ifndef _DEBUG  // debug version in WorkerThreadsView.cpp
inline CWorkerThreadsDoc* CWorkerThreadsView::GetDocument() const
{
    return reinterpret_cast<CWorkerThreadsDoc*>(m_pDocument);
}
#endif

