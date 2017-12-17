
// bgCarEndpointDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "bgCarEndpoint.h"
#include "bgCarEndpointDlg.h"
#include "afxdialogex.h"

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
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CbgCarEndpointDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_INFO, m_cControlInfo);
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CbgCarEndpointDlg::OnBnClickedBtnRightUp()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CbgCarEndpointDlg::OnBnClickedBtnRight()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CbgCarEndpointDlg::OnBnClickedBtnRightDown()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CbgCarEndpointDlg::OnBnClickedBtnDown()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CbgCarEndpointDlg::OnBnClickedBtnLeftDown()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CbgCarEndpointDlg::OnBnClickedBtnLeft()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CbgCarEndpointDlg::OnBnClickedBtnLeftUp()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CbgCarEndpointDlg::OnBnClickedBtnShotPlus()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CbgCarEndpointDlg::OnBnClickedBtnShotMinus()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CbgCarEndpointDlg::OnBnClickedBtnFocalDistansPlus()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CbgCarEndpointDlg::OnBnClickedBtnFocalDistanceMinus()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CbgCarEndpointDlg::OnBnClickedBtnAperyurePlus()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CbgCarEndpointDlg::OnBnClickedBtnAperyureMinus()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}
