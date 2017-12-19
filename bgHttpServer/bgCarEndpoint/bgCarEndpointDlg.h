
// bgCarEndpointDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"

class bgHttpClientImp;


// CbgCarEndpointDlg �Ի���
class CbgCarEndpointDlg : public CDialogEx
{
// ����
public:
	CbgCarEndpointDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_BGCARENDPOINT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListBox m_cControlInfo;
	CEdit m_cServerIp;
	CEdit m_cServerPort;

public:
	afx_msg void OnBnClickedBtnUp();
	afx_msg void OnBnClickedBtnRightUp();
	afx_msg void OnBnClickedBtnRight();
	afx_msg void OnBnClickedBtnRightDown();
	afx_msg void OnBnClickedBtnDown();
	afx_msg void OnBnClickedBtnLeftDown();
	afx_msg void OnBnClickedBtnLeft();
	afx_msg void OnBnClickedBtnLeftUp();
	afx_msg void OnBnClickedBtnShotPlus();
	afx_msg void OnBnClickedBtnShotMinus();
	afx_msg void OnBnClickedBtnFocalDistansPlus();
	afx_msg void OnBnClickedBtnFocalDistanceMinus();
	afx_msg void OnBnClickedBtnAperyurePlus();
	afx_msg void OnBnClickedBtnAperyureMinus();
	afx_msg void OnBnClickedBtnConnect();

private:
	bgHttpClientImp *http_client_;

public:
	afx_msg void OnBnClickedBtnRealvideo();
};
