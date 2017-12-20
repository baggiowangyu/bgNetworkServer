
// bgCarEndpointDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "bgCarEndpoint.h"
#include "bgCarEndpointDlg.h"
#include "afxdialogex.h"

#include "bgHttpClientImp.h"
#include "bgCommand.h"

#include "json/json.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CbgCarEndpointDlg �Ի���




CbgCarEndpointDlg::CbgCarEndpointDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CbgCarEndpointDlg::IDD, pParent)
	, http_client_(new bgHttpClientImp())
	
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CbgCarEndpointDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_INFO, m_cControlInfo);
	DDX_Control(pDX, IDC_EDIT_SERVER_IP, m_cServerIp);
	DDX_Control(pDX, IDC_EDIT_SERVER_PORT, m_cServerPort);
}

BEGIN_MESSAGE_MAP(CbgCarEndpointDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_UP, &CbgCarEndpointDlg::OnBnClickedBtnUp)
	ON_BN_CLICKED(IDC_BTN_RIGHT_UP, &CbgCarEndpointDlg::OnBnClickedBtnRightUp)
	ON_BN_CLICKED(IDC_BTN_RIGHT, &CbgCarEndpointDlg::OnBnClickedBtnRight)
	ON_BN_CLICKED(IDC_BTN_RIGHT_DOWN, &CbgCarEndpointDlg::OnBnClickedBtnRightDown)
	ON_BN_CLICKED(IDC_BTN_DOWN, &CbgCarEndpointDlg::OnBnClickedBtnDown)
	ON_BN_CLICKED(IDC_BTN_LEFT_DOWN, &CbgCarEndpointDlg::OnBnClickedBtnLeftDown)
	ON_BN_CLICKED(IDC_BTN_LEFT, &CbgCarEndpointDlg::OnBnClickedBtnLeft)
	ON_BN_CLICKED(IDC_BTN_LEFT_UP, &CbgCarEndpointDlg::OnBnClickedBtnLeftUp)
	ON_BN_CLICKED(IDC_BTN_SHOT_PLUS, &CbgCarEndpointDlg::OnBnClickedBtnShotPlus)
	ON_BN_CLICKED(IDC_BTN_SHOT_MINUS, &CbgCarEndpointDlg::OnBnClickedBtnShotMinus)
	ON_BN_CLICKED(IDC_BTN_FOCAL_DISTANS_PLUS, &CbgCarEndpointDlg::OnBnClickedBtnFocalDistansPlus)
	ON_BN_CLICKED(IDC_BTN_FOCAL_DISTANCE_MINUS, &CbgCarEndpointDlg::OnBnClickedBtnFocalDistanceMinus)
	ON_BN_CLICKED(IDC_BTN_APERYURE_PLUS, &CbgCarEndpointDlg::OnBnClickedBtnAperyurePlus)
	ON_BN_CLICKED(IDC_BTN_APERYURE_MINUS, &CbgCarEndpointDlg::OnBnClickedBtnAperyureMinus)
	ON_BN_CLICKED(IDC_BTN_CONNECT, &CbgCarEndpointDlg::OnBnClickedBtnConnect)
	ON_BN_CLICKED(IDC_BTN_REALVIDEO, &CbgCarEndpointDlg::OnBnClickedBtnRealvideo)
	ON_BN_CLICKED(IDC_BTN_REALVIDEO_STOP, &CbgCarEndpointDlg::OnBnClickedBtnRealvideoStop)
END_MESSAGE_MAP()


// CbgCarEndpointDlg ��Ϣ�������

BOOL CbgCarEndpointDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_cServerIp.SetWindowText(_T("127.0.0.1"));
	m_cServerPort.SetWindowText(_T("9876"));

	CWnd *pCwnd = GetDlgItem(IDC_RADIO1);
	CButton *b = (CButton *)pCwnd;
	b->SetCheck(1);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CbgCarEndpointDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CbgCarEndpointDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CbgCarEndpointDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CbgCarEndpointDlg::PreTranslateMessage(MSG* pMsg)
{
	std::string request_data = "";
	if(pMsg->message == WM_LBUTTONDOWN)
	{
		if(pMsg->hwnd == GetDlgItem(IDC_BTN_UP)->m_hWnd)
		{
			request_data = bgCommand::GeneratePTZCommand("direction", "dh", "Up", "Start", 7);
			http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)request_data.c_str(), request_data.size());
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_BTN_RIGHT_UP)->m_hWnd)
		{
			request_data = bgCommand::GeneratePTZCommand("direction", "dh", "UpRight", "Start", 7);
			http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)request_data.c_str(), request_data.size());
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_BTN_RIGHT)->m_hWnd)
		{
			request_data = bgCommand::GeneratePTZCommand("direction", "dh", "Right", "Start", 7);
			http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)request_data.c_str(), request_data.size());	
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_BTN_RIGHT_DOWN)->m_hWnd)
		{
			request_data = bgCommand::GeneratePTZCommand("direction", "dh", "DownRight", "Start", 7);
			http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)request_data.c_str(), request_data.size());
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_BTN_DOWN)->m_hWnd)
		{
			request_data = bgCommand::GeneratePTZCommand("direction", "dh", "Down", "Start", 7);
			http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)request_data.c_str(), request_data.size());
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_BTN_LEFT_DOWN)->m_hWnd)
		{
			request_data = bgCommand::GeneratePTZCommand("direction", "dh", "DownLeft", "Start", 7);
			http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)request_data.c_str(), request_data.size());
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_BTN_LEFT)->m_hWnd)
		{
			request_data = bgCommand::GeneratePTZCommand("direction", "dh", "Left", "Start", 7);
			http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)request_data.c_str(), request_data.size());
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_BTN_LEFT_UP)->m_hWnd)
		{
			request_data = bgCommand::GeneratePTZCommand("direction", "dh", "UpLeft", "Start", 7);
			http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)request_data.c_str(), request_data.size());
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_BTN_SHOT_PLUS)->m_hWnd)
		{
			request_data = bgCommand::GeneratePTZCommand("camera", "dh", "Plus", "Start", 7);
			http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)request_data.c_str(), request_data.size());
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_BTN_SHOT_MINUS)->m_hWnd)
		{
			request_data = bgCommand::GeneratePTZCommand("camera", "dh", "Minus", "Start", 7);
			http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)request_data.c_str(), request_data.size());
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_BTN_FOCAL_DISTANS_PLUS)->m_hWnd)
		{
			request_data = bgCommand::GeneratePTZCommand("focus", "dh", "Plus", "Start", 7);
			http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)request_data.c_str(), request_data.size());
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_BTN_FOCAL_DISTANCE_MINUS)->m_hWnd)
		{
			request_data = bgCommand::GeneratePTZCommand("focus", "dh", "Minus", "Start", 7);
			http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)request_data.c_str(), request_data.size());
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_BTN_APERYURE_PLUS)->m_hWnd)
		{
			request_data = bgCommand::GeneratePTZCommand("aperture", "dh", "Plus", "Start", 7);
			http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)request_data.c_str(), request_data.size());
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_BTN_APERYURE_MINUS)->m_hWnd)
		{
			request_data = bgCommand::GeneratePTZCommand("aperture", "dh", "Minus", "Start", 7);
			http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)request_data.c_str(), request_data.size());
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_BTN_REALVIDEO)->m_hWnd)
		{
			
		}
	}
	else if(pMsg->message == WM_LBUTTONUP)
	{
		if(pMsg->hwnd == GetDlgItem(IDC_BTN_UP)->m_hWnd)
		{
			request_data = bgCommand::GeneratePTZCommand("direction", "dh", "Up", "Stop", 7);
			http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)request_data.c_str(), request_data.size());
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_BTN_RIGHT_UP)->m_hWnd)
		{
			request_data = bgCommand::GeneratePTZCommand("direction", "dh", "UpRight", "Stop", 7);
			http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)request_data.c_str(), request_data.size());
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_BTN_RIGHT)->m_hWnd)
		{
			request_data = bgCommand::GeneratePTZCommand("direction", "dh", "Right", "Stop", 7);
			http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)request_data.c_str(), request_data.size());	
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_BTN_RIGHT_DOWN)->m_hWnd)
		{
			request_data = bgCommand::GeneratePTZCommand("direction", "dh", "DownRight", "Stop", 7);
			http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)request_data.c_str(), request_data.size());
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_BTN_DOWN)->m_hWnd)
		{
			request_data = bgCommand::GeneratePTZCommand("direction", "dh", "Down", "Stop", 7);
			http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)request_data.c_str(), request_data.size());
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_BTN_LEFT_DOWN)->m_hWnd)
		{
			request_data = bgCommand::GeneratePTZCommand("direction", "dh", "DownLeft", "Stop", 7);
			http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)request_data.c_str(), request_data.size());
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_BTN_LEFT)->m_hWnd)
		{
			request_data = bgCommand::GeneratePTZCommand("direction", "dh", "Left", "Stop", 7);
			http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)request_data.c_str(), request_data.size());
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_BTN_LEFT_UP)->m_hWnd)
		{
			request_data = bgCommand::GeneratePTZCommand("direction", "dh", "UpLeft", "Stop", 7);
			http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)request_data.c_str(), request_data.size());
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_BTN_SHOT_PLUS)->m_hWnd)
		{
			request_data = bgCommand::GeneratePTZCommand("camera", "dh", "Plus", "Stop", 7);
			http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)request_data.c_str(), request_data.size());
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_BTN_SHOT_MINUS)->m_hWnd)
		{
			request_data = bgCommand::GeneratePTZCommand("camera", "dh", "Minus", "Stop", 7);
			http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)request_data.c_str(), request_data.size());
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_BTN_FOCAL_DISTANS_PLUS)->m_hWnd)
		{
			request_data = bgCommand::GeneratePTZCommand("focus", "dh", "Plus", "Stop", 7);
			http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)request_data.c_str(), request_data.size());
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_BTN_FOCAL_DISTANCE_MINUS)->m_hWnd)
		{
			request_data = bgCommand::GeneratePTZCommand("focus", "dh", "Minus", "Stop", 7);
			http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)request_data.c_str(), request_data.size());
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_BTN_APERYURE_PLUS)->m_hWnd)
		{
			request_data = bgCommand::GeneratePTZCommand("aperture", "dh", "Plus", "Stop", 7);
			http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)request_data.c_str(), request_data.size());
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_BTN_APERYURE_MINUS)->m_hWnd)
		{
			request_data = bgCommand::GeneratePTZCommand("aperture", "dh", "Minus", "Stop", 7);
			http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)request_data.c_str(), request_data.size());
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_BTN_REALVIDEO)->m_hWnd)
		{

		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}



void CbgCarEndpointDlg::OnBnClickedBtnUp()
{

}


void CbgCarEndpointDlg::OnBnClickedBtnRightUp()
{

}


void CbgCarEndpointDlg::OnBnClickedBtnRight()
{

}


void CbgCarEndpointDlg::OnBnClickedBtnRightDown()
{

}


void CbgCarEndpointDlg::OnBnClickedBtnDown()
{

}


void CbgCarEndpointDlg::OnBnClickedBtnLeftDown()
{

}


void CbgCarEndpointDlg::OnBnClickedBtnLeft()
{

}

void CbgCarEndpointDlg::OnBnClickedBtnLeftUp()
{

}


void CbgCarEndpointDlg::OnBnClickedBtnShotPlus()
{

}


void CbgCarEndpointDlg::OnBnClickedBtnShotMinus()
{

}


void CbgCarEndpointDlg::OnBnClickedBtnFocalDistansPlus()
{

}


void CbgCarEndpointDlg::OnBnClickedBtnFocalDistanceMinus()
{

}


void CbgCarEndpointDlg::OnBnClickedBtnAperyurePlus()
{

}


void CbgCarEndpointDlg::OnBnClickedBtnAperyureMinus()
{

}

void CbgCarEndpointDlg::OnBnClickedBtnRealvideo()
{
	std::string request_data = bgCommand::GenerateStreamCommand("realvideo", "dh", "", "Start");

	// ����HTTP����
	http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)request_data.c_str(), request_data.size());
}



void CbgCarEndpointDlg::OnBnClickedBtnConnect()
{
	CString ip;
	CString port;
	bool use_https = false;

	m_cServerIp.GetWindowText(ip);
	m_cServerPort.GetWindowText(port);

	CWnd *pCwnd = GetDlgItem(IDC_RADIO1);
	CButton *b = (CButton *)pCwnd;
	if (b->GetCheck() == 1)
		use_https = false;
	else
		use_https = true;

	USES_CONVERSION;
	int errCode = http_client_->OnInit(T2A(ip.GetBuffer(0)), _ttoi(port.GetString()));
	if (errCode != 0)
	{
		AfxMessageBox(_T("��¼ʧ�ܣ�"));
		return ;
	}
}




void CbgCarEndpointDlg::OnBnClickedBtnRealvideoStop()
{
	std::string request_data = bgCommand::GenerateStreamCommand("realvideo", "dh", "", "Stop");

	// ����HTTP����
	http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)request_data.c_str(), request_data.size());
}
