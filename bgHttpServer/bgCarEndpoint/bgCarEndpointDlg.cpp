
// bgCarEndpointDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "bgCarEndpoint.h"
#include "bgCarEndpointDlg.h"
#include "afxdialogex.h"

#include "bgHttpClientImp.h"

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



void CbgCarEndpointDlg::OnBnClickedBtnUp()
{
	// ����һ��GUID
	char buffer[128] = {0};  
	GUID guid;  

	if (CoCreateGuid(&guid))  
	{  
		fprintf(stderr, "create guid error\n");  
		return ;  
	}  
	_snprintf(buffer, sizeof(buffer),  
		"%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X",  
		guid.Data1, guid.Data2, guid.Data3,  
		guid.Data4[0], guid.Data4[1], guid.Data4[2],  
		guid.Data4[3], guid.Data4[4], guid.Data4[5],  
		guid.Data4[6], guid.Data4[7]);  

	// ���ﹹ��json��������
	Json::Value arr;
	arr["id"] = Json::Value((char*)buffer);
	arr["commandtype"] = Json::Value("PTZControl");
	arr["subcmd"] = Json::Value("direction");
	arr["value"] = Json::Value("Up");
	arr["speed"] = Json::Value("5");

	Json::StyledWriter sw;
	std::string data = sw.write(arr);

	// ����HTTP����
	http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)data.c_str(), data.size());
}


void CbgCarEndpointDlg::OnBnClickedBtnRightUp()
{
	// ����һ��GUID
	char buffer[128] = {0};  
	GUID guid;  

	if (CoCreateGuid(&guid))  
	{  
		fprintf(stderr, "create guid error\n");  
		return ;  
	}  
	_snprintf(buffer, sizeof(buffer),  
		"%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X",  
		guid.Data1, guid.Data2, guid.Data3,  
		guid.Data4[0], guid.Data4[1], guid.Data4[2],  
		guid.Data4[3], guid.Data4[4], guid.Data4[5],  
		guid.Data4[6], guid.Data4[7]);  

	// ���ﹹ��json��������
	Json::Value arr;
	arr["id"] = Json::Value((char*)buffer);
	arr["commandtype"] = Json::Value("PTZControl");
	arr["subcmd"] = Json::Value("direction");
	arr["value"] = Json::Value("RightUp");
	arr["speed"] = Json::Value("5");

	Json::StyledWriter sw;
	std::string data = sw.write(arr);

	// ����HTTP����
	http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)data.c_str(), data.size());
}


void CbgCarEndpointDlg::OnBnClickedBtnRight()
{
	// ����һ��GUID
	char buffer[128] = {0};  
	GUID guid;  

	if (CoCreateGuid(&guid))  
	{  
		fprintf(stderr, "create guid error\n");  
		return ;  
	}  
	_snprintf(buffer, sizeof(buffer),  
		"%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X",  
		guid.Data1, guid.Data2, guid.Data3,  
		guid.Data4[0], guid.Data4[1], guid.Data4[2],  
		guid.Data4[3], guid.Data4[4], guid.Data4[5],  
		guid.Data4[6], guid.Data4[7]);  

	// ���ﹹ��json��������
	Json::Value arr;
	arr["id"] = Json::Value((char*)buffer);
	arr["commandtype"] = Json::Value("PTZControl");
	arr["subcmd"] = Json::Value("direction");
	arr["value"] = Json::Value("Right");
	arr["speed"] = Json::Value("5");

	Json::StyledWriter sw;
	std::string data = sw.write(arr);

	// ����HTTP����
	http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)data.c_str(), data.size());
}


void CbgCarEndpointDlg::OnBnClickedBtnRightDown()
{
	// ����һ��GUID
	char buffer[128] = {0};  
	GUID guid;  

	if (CoCreateGuid(&guid))  
	{  
		fprintf(stderr, "create guid error\n");  
		return ;  
	}  
	_snprintf(buffer, sizeof(buffer),  
		"%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X",  
		guid.Data1, guid.Data2, guid.Data3,  
		guid.Data4[0], guid.Data4[1], guid.Data4[2],  
		guid.Data4[3], guid.Data4[4], guid.Data4[5],  
		guid.Data4[6], guid.Data4[7]);  

	// ���ﹹ��json��������
	Json::Value arr;
	arr["id"] = Json::Value((char*)buffer);
	arr["commandtype"] = Json::Value("PTZControl");
	arr["subcmd"] = Json::Value("direction");
	arr["value"] = Json::Value("RightDown");
	arr["speed"] = Json::Value("5");

	Json::StyledWriter sw;
	std::string data = sw.write(arr);

	// ����HTTP����
	http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)data.c_str(), data.size());
}


void CbgCarEndpointDlg::OnBnClickedBtnDown()
{
	// ����һ��GUID
	char buffer[128] = {0};  
	GUID guid;  

	if (CoCreateGuid(&guid))  
	{  
		fprintf(stderr, "create guid error\n");  
		return ;  
	}  
	_snprintf(buffer, sizeof(buffer),  
		"%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X",  
		guid.Data1, guid.Data2, guid.Data3,  
		guid.Data4[0], guid.Data4[1], guid.Data4[2],  
		guid.Data4[3], guid.Data4[4], guid.Data4[5],  
		guid.Data4[6], guid.Data4[7]);  

	// ���ﹹ��json��������
	Json::Value arr;
	arr["id"] = Json::Value((char*)buffer);
	arr["commandtype"] = Json::Value("PTZControl");
	arr["subcmd"] = Json::Value("direction");
	arr["value"] = Json::Value("Down");
	arr["speed"] = Json::Value("5");

	Json::StyledWriter sw;
	std::string data = sw.write(arr);

	// ����HTTP����
	http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)data.c_str(), data.size());
}


void CbgCarEndpointDlg::OnBnClickedBtnLeftDown()
{
	// ����һ��GUID
	char buffer[128] = {0};  
	GUID guid;  

	if (CoCreateGuid(&guid))  
	{  
		fprintf(stderr, "create guid error\n");  
		return ;  
	}  
	_snprintf(buffer, sizeof(buffer),  
		"%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X",  
		guid.Data1, guid.Data2, guid.Data3,  
		guid.Data4[0], guid.Data4[1], guid.Data4[2],  
		guid.Data4[3], guid.Data4[4], guid.Data4[5],  
		guid.Data4[6], guid.Data4[7]);  

	// ���ﹹ��json��������
	Json::Value arr;
	arr["id"] = Json::Value((char*)buffer);
	arr["commandtype"] = Json::Value("PTZControl");
	arr["subcmd"] = Json::Value("direction");
	arr["value"] = Json::Value("LeftDown");
	arr["speed"] = Json::Value("5");

	Json::StyledWriter sw;
	std::string data = sw.write(arr);

	// ����HTTP����
	http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)data.c_str(), data.size());
}


void CbgCarEndpointDlg::OnBnClickedBtnLeft()
{
	// ����һ��GUID
	char buffer[128] = {0};  
	GUID guid;  

	if (CoCreateGuid(&guid))  
	{  
		fprintf(stderr, "create guid error\n");  
		return ;  
	}  
	_snprintf(buffer, sizeof(buffer),  
		"%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X",  
		guid.Data1, guid.Data2, guid.Data3,  
		guid.Data4[0], guid.Data4[1], guid.Data4[2],  
		guid.Data4[3], guid.Data4[4], guid.Data4[5],  
		guid.Data4[6], guid.Data4[7]);  

	// ���ﹹ��json��������
	Json::Value arr;
	arr["id"] = Json::Value((char*)buffer);
	arr["commandtype"] = Json::Value("PTZControl");
	arr["subcmd"] = Json::Value("direction");
	arr["value"] = Json::Value("Left");
	arr["speed"] = Json::Value("5");

	Json::StyledWriter sw;
	std::string data = sw.write(arr);

	// ����HTTP����
	http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)data.c_str(), data.size());
}


void CbgCarEndpointDlg::OnBnClickedBtnLeftUp()
{
	// ����һ��GUID
	char buffer[128] = {0};  
	GUID guid;  

	if (CoCreateGuid(&guid))  
	{  
		fprintf(stderr, "create guid error\n");  
		return ;  
	}  
	_snprintf(buffer, sizeof(buffer),  
		"%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X",  
		guid.Data1, guid.Data2, guid.Data3,  
		guid.Data4[0], guid.Data4[1], guid.Data4[2],  
		guid.Data4[3], guid.Data4[4], guid.Data4[5],  
		guid.Data4[6], guid.Data4[7]);  

	// ���ﹹ��json��������
	Json::Value arr;
	arr["id"] = Json::Value((char*)buffer);
	arr["commandtype"] = Json::Value("PTZControl");
	arr["subcmd"] = Json::Value("direction");
	arr["value"] = Json::Value("LeftUp");
	arr["speed"] = Json::Value("5");

	Json::StyledWriter sw;
	std::string data = sw.write(arr);

	// ����HTTP����
	http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)data.c_str(), data.size());
}


void CbgCarEndpointDlg::OnBnClickedBtnShotPlus()
{
	// ����һ��GUID
	char buffer[128] = {0};  
	GUID guid;  

	if (CoCreateGuid(&guid))  
	{  
		fprintf(stderr, "create guid error\n");  
		return ;  
	}  
	_snprintf(buffer, sizeof(buffer),  
		"%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X",  
		guid.Data1, guid.Data2, guid.Data3,  
		guid.Data4[0], guid.Data4[1], guid.Data4[2],  
		guid.Data4[3], guid.Data4[4], guid.Data4[5],  
		guid.Data4[6], guid.Data4[7]);  

	// ���ﹹ��json��������
	Json::Value arr;
	arr["id"] = Json::Value((char*)buffer);
	arr["commandtype"] = Json::Value("PTZControl");
	arr["subcmd"] = Json::Value("camera");
	arr["value"] = Json::Value("Plus");

	Json::StyledWriter sw;
	std::string data = sw.write(arr);

	// ����HTTP����
	http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)data.c_str(), data.size());
}


void CbgCarEndpointDlg::OnBnClickedBtnShotMinus()
{
	// ����һ��GUID
	char buffer[128] = {0};  
	GUID guid;  

	if (CoCreateGuid(&guid))  
	{  
		fprintf(stderr, "create guid error\n");  
		return ;  
	}  
	_snprintf(buffer, sizeof(buffer),  
		"%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X",  
		guid.Data1, guid.Data2, guid.Data3,  
		guid.Data4[0], guid.Data4[1], guid.Data4[2],  
		guid.Data4[3], guid.Data4[4], guid.Data4[5],  
		guid.Data4[6], guid.Data4[7]);  

	// ���ﹹ��json��������
	Json::Value arr;
	arr["id"] = Json::Value((char*)buffer);
	arr["commandtype"] = Json::Value("PTZControl");
	arr["subcmd"] = Json::Value("camera");
	arr["value"] = Json::Value("Minus");

	Json::StyledWriter sw;
	std::string data = sw.write(arr);

	// ����HTTP����
	http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)data.c_str(), data.size());
}


void CbgCarEndpointDlg::OnBnClickedBtnFocalDistansPlus()
{
	// ����һ��GUID
	char buffer[128] = {0};  
	GUID guid;  

	if (CoCreateGuid(&guid))  
	{  
		fprintf(stderr, "create guid error\n");  
		return ;  
	}  
	_snprintf(buffer, sizeof(buffer),  
		"%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X",  
		guid.Data1, guid.Data2, guid.Data3,  
		guid.Data4[0], guid.Data4[1], guid.Data4[2],  
		guid.Data4[3], guid.Data4[4], guid.Data4[5],  
		guid.Data4[6], guid.Data4[7]);  

	// ���ﹹ��json��������
	Json::Value arr;
	arr["id"] = Json::Value((char*)buffer);
	arr["commandtype"] = Json::Value("PTZControl");
	arr["subcmd"] = Json::Value("focus");
	arr["value"] = Json::Value("Plus");

	Json::StyledWriter sw;
	std::string data = sw.write(arr);

	// ����HTTP����
	http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)data.c_str(), data.size());
}


void CbgCarEndpointDlg::OnBnClickedBtnFocalDistanceMinus()
{
	// ����һ��GUID
	char buffer[128] = {0};  
	GUID guid;  

	if (CoCreateGuid(&guid))  
	{  
		fprintf(stderr, "create guid error\n");  
		return ;  
	}  
	_snprintf(buffer, sizeof(buffer),  
		"%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X",  
		guid.Data1, guid.Data2, guid.Data3,  
		guid.Data4[0], guid.Data4[1], guid.Data4[2],  
		guid.Data4[3], guid.Data4[4], guid.Data4[5],  
		guid.Data4[6], guid.Data4[7]);  

	// ���ﹹ��json��������
	Json::Value arr;
	arr["id"] = Json::Value((char*)buffer);
	arr["commandtype"] = Json::Value("PTZControl");
	arr["subcmd"] = Json::Value("focus");
	arr["value"] = Json::Value("Minus");

	Json::StyledWriter sw;
	std::string data = sw.write(arr);

	// ����HTTP����
	http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)data.c_str(), data.size());
}


void CbgCarEndpointDlg::OnBnClickedBtnAperyurePlus()
{
	// ����һ��GUID
	char buffer[128] = {0};  
	GUID guid;  

	if (CoCreateGuid(&guid))  
	{  
		fprintf(stderr, "create guid error\n");  
		return ;  
	}  
	_snprintf(buffer, sizeof(buffer),  
		"%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X",  
		guid.Data1, guid.Data2, guid.Data3,  
		guid.Data4[0], guid.Data4[1], guid.Data4[2],  
		guid.Data4[3], guid.Data4[4], guid.Data4[5],  
		guid.Data4[6], guid.Data4[7]);  

	// ���ﹹ��json��������
	Json::Value arr;
	arr["id"] = Json::Value((char*)buffer);
	arr["commandtype"] = Json::Value("PTZControl");
	arr["subcmd"] = Json::Value("aperture");
	arr["value"] = Json::Value("Plus");

	Json::StyledWriter sw;
	std::string data = sw.write(arr);

	// ����HTTP����
	http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)data.c_str(), data.size());
}


void CbgCarEndpointDlg::OnBnClickedBtnAperyureMinus()
{
	// ����һ��GUID
	char buffer[128] = {0};  
	GUID guid;  

	if (CoCreateGuid(&guid))  
	{  
		fprintf(stderr, "create guid error\n");  
		return ;  
	}  
	_snprintf(buffer, sizeof(buffer),  
		"%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X",  
		guid.Data1, guid.Data2, guid.Data3,  
		guid.Data4[0], guid.Data4[1], guid.Data4[2],  
		guid.Data4[3], guid.Data4[4], guid.Data4[5],  
		guid.Data4[6], guid.Data4[7]);  

	// ���ﹹ��json��������
	Json::Value arr;
	arr["id"] = Json::Value((char*)buffer);
	arr["commandtype"] = Json::Value("PTZControl");
	arr["subcmd"] = Json::Value("aperture");
	arr["value"] = Json::Value("Minus");

	Json::StyledWriter sw;
	std::string data = sw.write(arr);

	// ����HTTP����
	http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)data.c_str(), data.size());
}

void CbgCarEndpointDlg::OnBnClickedBtnRealvideo()
{
	// ����һ��GUID
	char buffer[128] = {0};  
	GUID guid;  

	if (CoCreateGuid(&guid))  
	{  
		fprintf(stderr, "create guid error\n");  
		return ;  
	}  
	_snprintf(buffer, sizeof(buffer),  
		"%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X",  
		guid.Data1, guid.Data2, guid.Data3,  
		guid.Data4[0], guid.Data4[1], guid.Data4[2],  
		guid.Data4[3], guid.Data4[4], guid.Data4[5],  
		guid.Data4[6], guid.Data4[7]);  

	// ���ﹹ��json��������
	Json::Value arr;
	arr["id"] = Json::Value((char*)buffer);
	arr["commandtype"] = Json::Value("stream");
	arr["subcmd"] = Json::Value("realvideo");

	Json::StyledWriter sw;
	std::string data = sw.write(arr);

	// ����HTTP����
	http_client_->OnSendHttpRequest("/goldmsg/car/DeviceControl", nullptr, 0, "POST", (const unsigned char *)data.c_str(), data.size());
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


