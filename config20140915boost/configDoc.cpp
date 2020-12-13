// configDoc.cpp : CconfigDoc 类的实现
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


// CconfigDoc 构造/析构

CconfigDoc::CconfigDoc()
{
	// TODO: 在此添加一次性构造代码

}

CconfigDoc::~CconfigDoc()
{
}

BOOL CconfigDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)
	SetTitle("配置软件");
	return TRUE;
}




// CconfigDoc 序列化

void CconfigDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CconfigDoc 诊断

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


// CconfigDoc 命令
