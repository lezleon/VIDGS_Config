// MainFrm.h : CMainFrame ��Ľӿ�
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
	
protected: // �������л�����
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// ����
public:
	

// ����
public:
	CDrawDlgBar m_wndDrawBar;
	HMENU m_hMenu1;
// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// ʵ��
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // �ؼ���Ƕ���Ա
	CStatusBar  m_wndStatusBar;

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	
};


