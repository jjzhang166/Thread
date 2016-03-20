// WorkerThreadsDoc.cpp : implementation of the CWorkerThreadsDoc class
//

#include "stdafx.h"
#include "WorkerThreads.h"

#include "WorkerThreadsDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWorkerThreadsDoc

IMPLEMENT_DYNCREATE(CWorkerThreadsDoc, CDocument)

BEGIN_MESSAGE_MAP(CWorkerThreadsDoc, CDocument)
END_MESSAGE_MAP()


// CWorkerThreadsDoc construction/destruction

CWorkerThreadsDoc::CWorkerThreadsDoc()
{
    // TODO: add one-time construction code here

}

CWorkerThreadsDoc::~CWorkerThreadsDoc()
{
}

BOOL CWorkerThreadsDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    // TODO: add reinitialization code here
    // (SDI documents will reuse this document)

    return TRUE;
}




// CWorkerThreadsDoc serialization

void CWorkerThreadsDoc::Serialize(CArchive& ar)
{
    if (ar.IsStoring())
    {
        // TODO: add storing code here
    }
    else
    {
        // TODO: add loading code here
    }
}


// CWorkerThreadsDoc diagnostics

#ifdef _DEBUG
void CWorkerThreadsDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CWorkerThreadsDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG


// CWorkerThreadsDoc commands
