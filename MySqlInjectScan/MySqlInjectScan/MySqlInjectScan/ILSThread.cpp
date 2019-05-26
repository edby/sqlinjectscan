#include "MySqlInjectScanDlg.h"

extern CString GetUrlCode(CString szUrl);
extern void ExtractSinglePageLink(CString szUrlCode, vector<CString>& linkVec);
extern void ExtractWebAllDynamicLink(CString szUrlCode, CString szDomain, vector<CString>& vec);
extern  CString GetBaiduLinkByKey(CString szSearchKey, int iEachPageNum);
extern void GetAllBaiduLinkByKey(CString szSearchKey, int iPageNum/*ҳ��*/, int iEachPageNum/*ÿҳ����*/, vector<CString>& vec);
extern void FilterLinkVec(vector<CString>& vec, char cFlag='��');
extern void GetCurPageAllSubLink(CString szUrlCode, vector<CString>& vec);
extern bool CanUrlInject(CString szUrl);

CInstanceCount ILSThread::sm_instanceCount;
ILSThread::ILSThread( CString szWebUrl, int iFlag, int iPn, CMySqlInjectScanDlg* pDlg):m_szWebUrl(szWebUrl),m_iFlag(iFlag),m_iPn(iPn),m_pMainDlg(pDlg)
{
	m_hThread = NULL;
	sm_instanceCount.Increase();
}

ILSThread::~ILSThread()
{
	sm_instanceCount.Decrease();
	if (sm_instanceCount.Value()==0)
	{
		m_pMainDlg->SendMessage(MSG_SET_BUTTON_TEXT);
	}
}

DWORD WINAPI PerScanThread(LPVOID pParam);
void ILSThread::StartThread()
{
	if (m_hThread)
		return;
	DWORD dwThreadID;
	m_hThread =::CreateThread(NULL,0,PerScanThread,this,0, &dwThreadID);
}

int ILSThread::TerminateThread()
{
	if (m_hThread)
	{
		::TerminateThread(m_hThread,0);
		::WaitForSingleObject(m_hThread,INFINITE);
	}
	return TT_FORCE;
}

void ILSThread::Execute()
{
	CString szUrlTmp = m_szWebUrl;
	if(m_szWebUrl.Find("http://") == -1)
		szUrlTmp = "http://" + m_szWebUrl;
	char* p = szUrlTmp.GetBuffer();

	CString szUrlCode = GetUrlCode(szUrlTmp);
	if(szUrlCode == "")
		goto ag;

	//��ȡ�ٶȽ����ҳ
	if( 0 == m_iFlag)
	{
		
		//���������ȡ��ǰҳ����ҳ����(�ٶȿ���ǰ����ʵ��������)
		tgVecPn* pVecPn = new tgVecPn;
		GetCurPageAllSubLink(szUrlCode, pVecPn->vecTmp);
		pVecPn->iPn = m_iPn;
		m_pMainDlg->SendMessage(MESSAGE_SHOW_SCAN, 0, (LPARAM)pVecPn);
		
	}
	//��ȡ��ǰ��ҳ��������
	else if( 1 == m_iFlag)
	{
		tgVecPn* pVecPn = new tgVecPn;
		ExtractWebAllDynamicLink(szUrlCode, m_szWebUrl, pVecPn->vecTmp);
		pVecPn->szUrl = m_szWebUrl;
		m_pMainDlg->SendMessage(MESSAGE_SHOW_SCAN, 1, (LPARAM)pVecPn);
	}
	//�����ҳ�Ƿ��ע��
	else
	{	
		tgVecPn* pVecPn = new tgVecPn;
			pVecPn->szUrl = m_szWebUrl;
		if(CanUrlInject(m_szWebUrl))
			m_pMainDlg->SendMessage(MESSAGE_SHOW_SCAN, 2, (LPARAM)pVecPn);
	}
	ag:m_pMainDlg->m_pThreadPool->RemoveThread(this);
}


//����ɨ���߳�
DWORD WINAPI PerScanThread(LPVOID pParam)
{
	ILSThread* pThis = (ILSThread*)pParam;
	pThis->Execute();
	delete pThis;
	return 0;
}


