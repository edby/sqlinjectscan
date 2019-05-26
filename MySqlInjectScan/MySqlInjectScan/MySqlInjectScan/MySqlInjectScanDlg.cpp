
// MySqlInjectScanDlg.cpp : 实现文件
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


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
extern CString GetUrlCode(CString szUrl);
extern void ExtractSinglePageLink(CString szUrlCode, vector<CString>& linkVec);
extern void ExtractWebAllDynamicLink(CString szWebUrl, vector<CString>& vec);
extern  CString GetBaiduLinkByKey(CString szSearchKey, int iEachPageNum);
extern void GetAllBaiduLinkByKey(CString szSearchKey, int iPageNum/*页数*/, int iEachPageNum/*每页条数*/, vector<CString>& vec);
extern void FilterLinkVec(vector<CString>& vec, char cFlag='￥');
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

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	 void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CMySqlInjectScanDlg 对话框




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


// CMySqlInjectScanDlg 消息处理程序

BOOL CMySqlInjectScanDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
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
	m_szKey = "有限公司";
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
	m_wndStatusBar.SetPaneText(0,"就绪");
	m_wndStatusBar.SetPaneInfo(1,0,0,200);
	m_wndStatusBar.SetPaneText(1,"大写");
	m_wndStatusBar.SetPaneInfo(2,0,0,800);
	m_wndStatusBar.SetPaneText(2,"更多精彩尽在c++hack编程:572399768");
	
	//----------------让这个状态栏最终显示在对话框中-------------
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);

	m_pThreadPool->StartThread();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMySqlInjectScanDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMySqlInjectScanDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//获取链接并扫描注入
void CMySqlInjectScanDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
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
		//获取下一页链接
		szUrl.Format("&pn=%d",iPn);
		szUrl = m_szLinkHeadByKey + szUrl;
		ILSThread* pThread = new ILSThread(szUrl, 0, iPn, this);
		m_pThreadPool->AddThread(pThread);
	}
}


//设置线程数
void CMySqlInjectScanDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
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
			//搜索当前网页所有动态链接
			ILSThread* pThread = new ILSThread(vecTmp[i], 1, 0, this);
			m_pThreadPool->AddThread(pThread);
		}
		
		break;
	case 1:
		szCurUrl = pTg->szUrl;
		//根据url找出在vec中的索引
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

			//检查是否可注入
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
	// TODO: 在此添加控件通知处理程序代码
	CPoint ScreenPt;
	GetCursorPos(&ScreenPt);
	//获取到当前鼠标选择的树节点
	m_tree.ScreenToClient(&ScreenPt);
	HTREEITEM tree_Item = m_tree.HitTest(ScreenPt);
	if (tree_Item != NULL)
	{
		m_tree.SelectItem(tree_Item); //使右键单击的树节点被选中

		CMenu menu;
		menu.LoadMenu(IDR_MENU1);
		CMenu* pPopup = menu.GetSubMenu(0);//装载第一个子菜单，即我们菜单的第一列
		m_tree.ClientToScreen(&ScreenPt);
		 pPopup->TrackPopupMenu(TPM_LEFTALIGN, ScreenPt.x, ScreenPt.y, this);
		menu.Detach();
	}
	*pResult = 0;
}


void CMySqlInjectScanDlg::OnCopyname()
{
	// TODO: 在此添加命令处理程序代码
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
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_pThreadPool->TerminalThreads();
	CDialogEx::OnClose();
}

LRESULT CMySqlInjectScanDlg::OnSetStatusBarText( WPARAM wParam,LPARAM lParam )
{
	LPCTSTR szMsg=(LPCTSTR)lParam;
	m_wndStatusBar.SetPaneText(wParam,szMsg);
	return 0;
}

//复制注入点
void CMySqlInjectScanDlg::OnCopyinjecturl()
{
	// TODO: 在此添加命令处理程序代码
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


//浏览器打开注入url
void CMySqlInjectScanDlg::OnBrowseurl()
{
	// TODO: 在此添加命令处理程序代码
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
	// TODO: 在此添加命令处理程序代码
	//获取listbox中所有内容
	CString szAllContent, szTmp;
	int iCount = m_listBox.GetCount();
	for(int i=0; i<iCount; i++)
	{
		m_listBox.GetText(i, szTmp);
		szTmp += "\r\n";
		szAllContent+=szTmp;
	}

	CFileDialog fileDlg(FALSE);
	fileDlg.m_ofn.lpstrTitle = "文件保存";
	fileDlg.m_ofn.lpstrFilter="Text Files(*.txt)\0*.txt\0All Files(*.*)\0*.*\0\0";
	fileDlg.m_ofn.lpstrDefExt = "txt";
	if(IDOK == fileDlg.DoModal())
	{
		CFile file(fileDlg.GetPathName(), CFile::modeCreate|CFile::modeWrite);
		file.Write(szAllContent, szAllContent.GetLength());
		file.Close();
	}
}
