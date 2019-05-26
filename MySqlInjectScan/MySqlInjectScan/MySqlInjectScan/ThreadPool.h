// ThreadPool.h: interface for the CThreadPool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THREADPOOL_H__0F696442_2571_4265_84F8_AC23B3F285CB__INCLUDED_)
#define AFX_THREADPOOL_H__0F696442_2571_4265_84F8_AC23B3F285CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ThreadQueue.h"
//#include <afxmt.h>
#include <vector>

using namespace std;

//typedef struct _THREAD_TYPE_INFO
//{
//	CString strName;
//	int nExeNum;
//	int nWaitNum;
//}THREAD_TYPE_INFO;
//
//typedef vector<THREAD_TYPE_INFO>   TTI_VEC;
//
//class IOutputMsg;

class CMySqlInjectScanDlg;
class CThreadPool
{
public:
	CThreadPool(CMySqlInjectScanDlg* pDlg);
	~CThreadPool();
	void AddThread(ILSThread *pThread);
	 void RemoveThread(ILSThread *pThread);
	 //int  GetThreadCount(CString strName);
	void ThreadsManager();
	void StartThread();
	int TerminalThreads();
	void SetMaxExeThreadNum(int nNum){m_nMaxExeThreadNum=nNum;}
	//void GetThreadTypeInfo(TTI_VEC &tv);
private:
	//IOutputMsg *m_pOutputMsg;
    CThreadQueue m_queueExecuteThread;
	CThreadQueue m_queueWaitThread;
	CCriticalSection m_csThreadQueue;
	CWinThread *m_pWinThread;//主工作线程
	bool m_bStopThread;
	int  m_nMaxExeThreadNum;
	CMySqlInjectScanDlg* pMainDlg;
	//int FindThreadType(TTI_VEC &tv,CString strName);
};

#endif // !defined(AFX_THREADPOOL_H__0F696442_2571_4265_84F8_AC23B3F285CB__INCLUDED_)
