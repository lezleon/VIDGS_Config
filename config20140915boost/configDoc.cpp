// configDoc.cpp : CconfigDoc ���ʵ��
//

#include "stdafx.h"
#include "config.h"

#include "configDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CconfigDoc

IMPLEMENT_DYNCREATE(CconfigDoc, CDocument)

BEGIN_MESSAGE_MAP(CconfigDoc, CDocument)
END_MESSAGE_MAP()


// CconfigDoc ����/����

CconfigDoc::CconfigDoc()
{
	// TODO: �ڴ����һ���Թ������

}

CconfigDoc::~CconfigDoc()
{
}

BOOL CconfigDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)
	SetTitle("�������");
	return TRUE;
}




// CconfigDoc ���л�

void CconfigDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}


// CconfigDoc ���

#ifdef _DEBUG
void CconfigDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CconfigDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CconfigDoc ����
