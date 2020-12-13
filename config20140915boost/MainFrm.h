// MainFrm.h : CMainFrame 类的接口
//


#pragma once


#include <afxpriv.h>
#include <afxtempl.h>
#include "Digistatic\Digistatic.h"
#include "ButtonST\BtnST.h"
#include "TypeChoose.h"
class CDrawDlgBar : public CDialogBar
{
	DECLARE_DYNAMIC(CDrawDlgBar)

public:
	CDrawDlgBar();
	virtual ~CDrawDlgBar();
public:
	//HMENU m_hMenu1;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg LRESULT OnInitDialog ( WPARAM, LPARAM );
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
};


class CMainFrame : public CFrameWnd
{
	
protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 属性
public:
	

// 操作
public:
	CDrawDlgBar m_wndDrawBar;
	HMENU m_hMenu1;
// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	CStatusBar  m_wndStatusBar;

// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	
};


