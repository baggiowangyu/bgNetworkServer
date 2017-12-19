
// bgCarEndpointDlg.h : 头文件
//

#pragma once
#include "afxwin.h"

class bgHttpClientImp;


// CbgCarEndpointDlg 对话框
class CbgCarEndpointDlg : public CDialogEx
{
// 构造
public:
	CbgCarEndpointDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_BGCARENDPOINT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
