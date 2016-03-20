// WorkerThreadsDoc.h : interface of the CWorkerThreadsDoc class
//


#pragma once

class CWorkerThreadsDoc : public CDocument
{
protected: // create from serialization only
    CWorkerThreadsDoc();
    DECLARE_DYNCREATE(CWorkerThreadsDoc)

    // Attributes
public:

    // Operations
public:

    // Overrides
public:
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);

    // Implementation
public:
    virtual ~CWorkerThreadsDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:

    // Generated message map functions
protected:
    DECLARE_MESSAGE_MAP()
};


