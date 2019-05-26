
// MySqlInjectScanDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "ThreadPool.h"
#include "resource.h"
#include <vector>
#include <map>
#include "afxcmn.h"
#include "MyListBox.h"
using namespace std;

// CMySqlInjectScanDlg �Ի���
class CMySqlInjectScanDlg : public CDialogEx
{
// ����
public:
	CMySqlInjectScanDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MYSQLINJECTSCAN_DIALOG };

	protected:
	 void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	 BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_szKey;
	int m_iPageNum;
	CComboBox m_comboBox2;
	CComboBox m_coboBox1;
	CThreadPool* m_pThreadPool;
	CString m_szLinkHeadByKey;
	vector<CString> m_vecAllBaiduLink; //���ݹؼ��������������аٶ�����
	vector<HTREEITEM> m_vecLinkItem;
	vector<CString> m_vecAllLinkFlag;
	vector<CString> m_vecInjectDomainFlag;
	map<CString, vector<CString>> m_mapAllLink;
	CStatusBar  m_wndStatusBar;

	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton1();
	CTreeCtrl m_tree;
	LRESULT OnShowScan(WPARAM wParam,LPARAM lParam);
	int m_iEachPageNum;
	CMyListBox m_listBox;
	afx_msg void OnNMRClickTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCopyname();
	afx_msg void OnClose();
	LRESULT OnSetStatusBarText(WPARAM wParam,LPARAM lParam);
	LRESULT OnSetButtonStatus(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCopyinjecturl();
	afx_msg void OnBrowseurl();
	afx_msg void OnAllsave();
};
