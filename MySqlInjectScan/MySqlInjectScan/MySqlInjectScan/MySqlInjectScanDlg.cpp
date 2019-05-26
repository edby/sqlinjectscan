
// MySqlInjectScanDlg.cpp : ʵ���ļ�
//
#include "stdafx.h"
#include "MySqlInjectScan.h"
#include "MySqlInjectScanDlg.h"
#include "afxdialogex.h"
#include <vector>
#include <algorithm>
#include <afx.h>
using namespace std;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���
extern CString GetUrlCode(CString szUrl);
extern void ExtractSinglePageLink(CString szUrlCode, vector<CString>& linkVec);
extern void ExtractWebAllDynamicLink(CString szWebUrl, vector<CString>& vec);
extern  CString GetBaiduLinkByKey(CString szSearchKey, int iEachPageNum);
extern void GetAllBaiduLinkByKey(CString szSearchKey, int iPageNum/*ҳ��*/, int iEachPageNum/*ÿҳ����*/, vector<CString>& vec);
extern void FilterLinkVec(vector<CString>& vec, char cFlag='��');
extern bool IsStandardDynamicLink(CString szUrl);
 
static UINT indicators[3] =
{
	ID_FIRST,                               // status line indicator
	ID_SECOND,           //CAP lock indicator.
	ID_THIRD            //NUM lock indicator.
};


class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	 void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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


// CMySqlInjectScanDlg �Ի���




CMySqlInjectScanDlg::CMySqlInjectScanDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMySqlInjectScanDlg::IDD, pParent)
	, m_szKey(_T(""))
	, m_iPageNum(0)
	, m_iEachPageNum(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pThreadPool = new CThreadPool(this);
	
}

void CMySqlInjectScanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_KEY, m_szKey);
	DDX_Text(pDX, IDC_PAGENUM, m_iPageNum);
	DDX_Control(pDX, IDC_COMBO2, m_comboBox2);
	DDX_Control(pDX, IDC_COMBO1, m_coboBox1);
	DDX_Control(pDX, IDC_TREE1, m_tree);
	DDX_Text(pDX, IDC_COMBO2, m_iEachPageNum);
	DDX_Control(pDX, IDC_LIST1, m_listBox);
}

BEGIN_MESSAGE_MAP(CMySqlInjectScanDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON3, &CMySqlInjectScanDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON1, &CMySqlInjectScanDlg::OnBnClickedButton1)
	ON_MESSAGE(MESSAGE_SHOW_SCAN,OnShowScan)
	ON_MESSAGE(MSG_SET_STATUSBAR_TEXT,OnSetStatusBarText)
	ON_MESSAGE(MSG_SET_BUTTON_TEXT, OnSetButtonStatus)
	ON_NOTIFY(NM_RCLICK, IDC_TREE1, &CMySqlInjectScanDlg::OnNMRClickTree1)
	ON_COMMAND(ID_COPYNAME, &CMySqlInjectScanDlg::OnCopyname)
	ON_WM_CLOSE()
	ON_COMMAND(ID_COPYINJECTURL, &CMySqlInjectScanDlg::OnCopyinjecturl)
	ON_COMMAND(ID_BROWSEURL, &CMySqlInjectScanDlg::OnBrowseurl)
	ON_COMMAND(ID_ALLSAVE, &CMySqlInjectScanDlg::OnAllsave)
END_MESSAGE_MAP()


// CMySqlInjectScanDlg ��Ϣ�������

BOOL CMySqlInjectScanDlg::OnInitDialog()
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
	m_comboBox2.AddString("10");
	m_comboBox2.AddString("20");
	m_comboBox2.AddString("50");
	m_comboBox2.SetCurSel(2);

	m_coboBox1.AddString("1");
	m_coboBox1.AddString("8");
	m_coboBox1.AddString("32");
	m_coboBox1.AddString("64");
	m_coboBox1.AddString("256");
	m_coboBox1.SetCurSel(2);
	vector<CString> vec;
	m_szKey = "���޹�˾";
	m_iPageNum = 0;
	UpdateData(FALSE);
	
	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1; 
	}
	bool flag;
	flag = m_wndStatusBar.SetIndicators(indicators, 3);
	/*if(!flag)
	{
		TRACE0("Failed to create status bar\n");
		return -1;
	}*/
	m_wndStatusBar.SetPaneInfo(0,0,0,100);
	m_wndStatusBar.SetPaneText(0,"����");
	m_wndStatusBar.SetPaneInfo(1,0,0,200);
	m_wndStatusBar.SetPaneText(1,"��д");
	m_wndStatusBar.SetPaneInfo(2,0,0,800);
	m_wndStatusBar.SetPaneText(2,"���ྫ�ʾ���c++hack���:572399768");
	
	//----------------�����״̬��������ʾ�ڶԻ�����-------------
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);

	m_pThreadPool->StartThread();
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMySqlInjectScanDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMySqlInjectScanDlg::OnPaint()
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
HCURSOR CMySqlInjectScanDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//��ȡ���Ӳ�ɨ��ע��
void CMySqlInjectScanDlg::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
	UpdateData(TRUE);
	m_tree.DeleteAllItems();
	m_listBox.ResetContent();
	m_vecAllBaiduLink.clear();
	m_vecLinkItem.clear();
	m_vecAllLinkFlag.clear();
	m_vecInjectDomainFlag.clear();
	
	if(m_iEachPageNum == 10)
	{
		if(m_iPageNum==0||m_iPageNum>76)
			m_iPageNum = 76;
	}
	else if(m_iEachPageNum == 20)
	{
		if(m_iPageNum==0||m_iPageNum>38)
			m_iPageNum = 38;
	}
	else /*if(m_iPageNum == 50)*/
	{
		if(m_iPageNum==0||m_iPageNum>16)
		m_iPageNum = 16;
	}
	UpdateData(FALSE);
	m_szLinkHeadByKey = GetBaiduLinkByKey(m_szKey, m_iEachPageNum);

	CString szUrl;
	int iPn;
	for(int iPos=0; iPos<m_iPageNum; iPos++)
	{
		iPn = iPos*m_iEachPageNum;
		//��ȡ��һҳ����
		szUrl.Format("&pn=%d",iPn);
		szUrl = m_szLinkHeadByKey + szUrl;
		ILSThread* pThread = new ILSThread(szUrl, 0, iPn, this);
		m_pThreadPool->AddThread(pThread);
	}
}


//�����߳���
void CMySqlInjectScanDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString szThreadNum;
	m_coboBox1.GetWindowText(szThreadNum);
	int iThreadNum = atoi(szThreadNum);
	m_pThreadPool->SetMaxExeThreadNum(iThreadNum);
}



int GetUrlPosInVec(CString szUrl, vector<CString> vec)
{
	for(int i=0; i<vec.size(); i++)
		if(vec[i] == szUrl)
			return i;
	return -1;
}

CString GetDomainByUrl(CString szUrl)
{
	szUrl.Replace("http://","");
	int iPos = szUrl.Find("/");
	return szUrl.Left(iPos);
}

LRESULT CMySqlInjectScanDlg::OnShowScan( WPARAM wParam,LPARAM lParam )
{
	tgVecPn* pTg = (tgVecPn*)lParam;
	vector<CString> vecTmp(pTg->vecTmp);
	CString szCurUrl,szDomain;
	HTREEITEM hItemTmp=NULL, hItem0=NULL, hItem=NULL;
	int iPos;
	vector<CString>::iterator iter;

	switch(wParam)
	{
	case 0:
		FilterLinkVec(vecTmp);
		for(int i=0; i<vecTmp.size(); i++)
		{
			iter = std::find(m_vecAllLinkFlag.begin(), m_vecAllLinkFlag.end(), vecTmp[i]);
			if(iter != m_vecAllLinkFlag.end())
				continue;
			if(0==i)
			{
				hItem0 = m_tree.InsertItem(vecTmp[i], TVI_ROOT);
			}
			else
			{
				hItem0 = m_tree.InsertItem(vecTmp[i], TVI_ROOT, hItem0);
			}
			m_vecAllBaiduLink.push_back(vecTmp[i]);
			m_vecAllLinkFlag.push_back(vecTmp[i]);
			m_vecLinkItem.push_back(hItem0);
			//������ǰ��ҳ���ж�̬����
			ILSThread* pThread = new ILSThread(vecTmp[i], 1, 0, this);
			m_pThreadPool->AddThread(pThread);
		}
		
		break;
	case 1:
		szCurUrl = pTg->szUrl;
		//����url�ҳ���vec�е�����
		iPos = GetUrlPosInVec(szCurUrl, m_vecAllBaiduLink);
		hItem = m_vecLinkItem[iPos];
		for(int i=0; i<vecTmp.size(); i++)
		{
			if(!IsStandardDynamicLink(vecTmp[i]))
				continue;
			iter = std::find(m_vecAllLinkFlag.begin(),m_vecAllLinkFlag.end(), vecTmp[i]);
			if(iter != m_vecAllLinkFlag.end())
				continue;
			if(0 == i)
				hItem0 = m_tree.InsertItem(vecTmp[i], hItem);
			else
				hItem0 = m_tree.InsertItem(vecTmp[i], hItem, hItem0);
			m_vecAllLinkFlag.push_back(vecTmp[i]);

			//����Ƿ��ע��
			ILSThread* pThread = new ILSThread(vecTmp[i], 2, 0, this);
			m_pThreadPool->AddThread(pThread);
		}
		break;
	case 2:
		szCurUrl = pTg->szUrl;
		szDomain = GetDomainByUrl(szCurUrl);
		iter = std::find(m_vecInjectDomainFlag.begin(),m_vecInjectDomainFlag.end(), szDomain);
		if(iter != m_vecInjectDomainFlag.end())
			break;
		m_listBox.AddString(szCurUrl);
		m_vecInjectDomainFlag.push_back(szDomain);
		break;
	}
	return 0;
}


void CMySqlInjectScanDlg::OnNMRClickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CPoint ScreenPt;
	GetCursorPos(&ScreenPt);
	//��ȡ����ǰ���ѡ������ڵ�
	m_tree.ScreenToClient(&ScreenPt);
	HTREEITEM tree_Item = m_tree.HitTest(ScreenPt);
	if (tree_Item != NULL)
	{
		m_tree.SelectItem(tree_Item); //ʹ�Ҽ����������ڵ㱻ѡ��

		CMenu menu;
		menu.LoadMenu(IDR_MENU1);
		CMenu* pPopup = menu.GetSubMenu(0);//װ�ص�һ���Ӳ˵��������ǲ˵��ĵ�һ��
		m_tree.ClientToScreen(&ScreenPt);
		 pPopup->TrackPopupMenu(TPM_LEFTALIGN, ScreenPt.x, ScreenPt.y, this);
		menu.Detach();
	}
	*pResult = 0;
}


void CMySqlInjectScanDlg::OnCopyname()
{
	// TODO: �ڴ���������������
	HTREEITEM hItem =m_tree.GetSelectedItem();
	CString szUrl = m_tree.GetItemText(hItem);
	//MessageBox(szUrl);

	if(OpenClipboard())
	{
		HANDLE hClip;
		char* pBuf;
		EmptyClipboard();
		hClip = GlobalAlloc(GMEM_MOVEABLE, szUrl.GetLength()+1);
		pBuf = (char*)GlobalLock(hClip);
		strcpy(pBuf, szUrl);
		GlobalUnlock(hClip);
		SetClipboardData(CF_TEXT, hClip);
		CloseClipboard();
	}
}


void CMySqlInjectScanDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_pThreadPool->TerminalThreads();
	CDialogEx::OnClose();
}

LRESULT CMySqlInjectScanDlg::OnSetStatusBarText( WPARAM wParam,LPARAM lParam )
{
	LPCTSTR szMsg=(LPCTSTR)lParam;
	m_wndStatusBar.SetPaneText(wParam,szMsg);
	return 0;
}

//����ע���
void CMySqlInjectScanDlg::OnCopyinjecturl()
{
	// TODO: �ڴ���������������
	int iPos =m_listBox.GetCurSel();
	CString szUrl;
	m_listBox.GetText(iPos, szUrl);
	//szUrl.ReleaseBuffer();

	if(OpenClipboard())
	{
		HANDLE hClip;
		char* pBuf;
		EmptyClipboard();
		hClip = GlobalAlloc(GMEM_MOVEABLE, szUrl.GetLength()+1);
		pBuf = (char*)GlobalLock(hClip);
		strcpy(pBuf, szUrl);
		GlobalUnlock(hClip);
		SetClipboardData(CF_TEXT, hClip);
		CloseClipboard();
	}
}


//�������ע��url
void CMySqlInjectScanDlg::OnBrowseurl()
{
	// TODO: �ڴ���������������
	int iPos =m_listBox.GetCurSel();
	CString szUrl;
	m_listBox.GetText(iPos,szUrl);
	ShellExecute(0, 0, szUrl, 0, 0, SW_SHOWMAXIMIZED);
	szUrl.ReleaseBuffer();
}

LRESULT CMySqlInjectScanDlg::OnSetButtonStatus( WPARAM wParam, LPARAM lParam )
{
	GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);
	return 0;
}

void CMySqlInjectScanDlg::OnAllsave()
{
	// TODO: �ڴ���������������
	//��ȡlistbox����������
	CString szAllContent, szTmp;
	int iCount = m_listBox.GetCount();
	for(int i=0; i<iCount; i++)
	{
		m_listBox.GetText(i, szTmp);
		szTmp += "\r\n";
		szAllContent+=szTmp;
	}

	CFileDialog fileDlg(FALSE);
	fileDlg.m_ofn.lpstrTitle = "�ļ�����";
	fileDlg.m_ofn.lpstrFilter="Text Files(*.txt)\0*.txt\0All Files(*.*)\0*.*\0\0";
	fileDlg.m_ofn.lpstrDefExt = "txt";
	if(IDOK == fileDlg.DoModal())
	{
		CFile file(fileDlg.GetPathName(), CFile::modeCreate|CFile::modeWrite);
		file.Write(szAllContent, szAllContent.GetLength());
		file.Close();
	}
}
