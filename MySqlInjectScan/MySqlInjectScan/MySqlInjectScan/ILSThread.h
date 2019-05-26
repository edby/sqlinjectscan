#include "stdafx.h"
#include "InstanceCount.h"
enum
{
	TT_NORMAL,
	TT_FORCE
};

class CMySqlInjectScanDlg;
class ILSThread
{
	CString m_szWebUrl;
	int m_iFlag;//0:查百度搜索结果 1:查网站子链接 2：检查网站是否可注入
	int m_iPn;
	HANDLE m_hThread;
	
public:
	ILSThread(CString szWebUrl, int iFlag, int iPn, CMySqlInjectScanDlg* pDlg);
	~ILSThread();
	void StartThread();
	int TerminateThread();
	void Execute();
public:
	CMySqlInjectScanDlg* m_pMainDlg;
	static CInstanceCount sm_instanceCount;

};