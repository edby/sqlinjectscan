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
	int m_iFlag;//0:��ٶ�������� 1:����վ������ 2�������վ�Ƿ��ע��
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