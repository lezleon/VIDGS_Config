// configDoc.h : CconfigDoc ��Ľӿ�
//


#pragma once


class CconfigDoc : public CDocument
{
protected: // �������л�����
	CconfigDoc();
	DECLARE_DYNCREATE(CconfigDoc)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ʵ��
public:
	virtual ~CconfigDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
};


