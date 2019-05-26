// ThreadPool.cpp: implementation of the CThreadPool class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "stdafx.h"
#include "ThreadPool.h"
#include "MySqlInjectScanDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UINT ThreadsManagerThread(LPVOID pParam)
{
	CThreadPool *tp=(CThreadPool*)pParam;
	tp->ThreadsManager();
	return 0;
}

CThreadPool::CThreadPool(CMySqlInjectScanDlg *pDlg)
{
    pMainDlg=pDlg;
	m_pWinThread=NULL;
	m_nMaxExeThreadNum=32;
	m_bStopThread=false;
}

CThreadPool::~CThreadPool()
{
    m_bStopThread=true;
	WaitForSingleObject(m_pWinThread->m_hThread,INFINITE);
}

void CThreadPool::AddThread(ILSThread *pThread)
{
	m_csThreadQueue.Lock();
	m_queueWaitThread.Push(pThread);
	m_csThreadQueue.Unlock();
}

void CThreadPool::RemoveThread(ILSThread *pThread)
{
	m_csThreadQueue.Lock();
	m_queueExecuteThread.Remove(pThread);
	m_csThreadQueue.Unlock();
}

//int  CThreadPool::GetThreadCount(CString strName)
//{
//	TTI_VEC tv;
//	GetThreadTypeInfo(tv);
//	for (int i=0;i<tv.size();i++)
//	{
//		if (tv[i].strName==strName)
//			return tv[i].nWaitNum+tv[i].nExeNum;
//	}
//	return 0;
//}

void CThreadPool::ThreadsManager()
{
	while (true)
	{
		if (m_bStopThread)
			break;
		m_csThreadQueue.Lock();
		while (m_queueExecuteThread.GetCount()<m_nMaxExeThreadNum)
		{
			ILSThread *pThread;
			if (!m_queueWaitThread.Pop(pThread))
				break;
			pThread->StartThread();
			m_queueExecuteThread.Push(pThread);
		}
		int nExecuteCount=m_queueExecuteThread.GetCount();
        int nWaitCount=m_queueWaitThread.GetCount();
		m_csThreadQueue.Unlock();

		CString str;
		str.Format("执行线程数=%d,等待任务数=%d",nExecuteCount,nWaitCount);
        //m_pOutputMsg->ShowStatusBarMsg(1,str);
		pMainDlg->SendMessage(MSG_SET_STATUSBAR_TEXT,1,(long)str.GetBuffer(0));
		/*if(nExecuteCount==0&&nWaitCount==0)
			pMainDlg->SendMessage(MSG_SET_BUTTON_TEXT)*/;
		Sleep(100);
	}
}

void CThreadPool::StartThread()
{
	m_pWinThread=AfxBeginThread(ThreadsManagerThread,this);
}

int CThreadPool::TerminalThreads()
{
	m_csThreadQueue.Lock();
	int nCount=m_queueExecuteThread.GetCount()+m_queueWaitThread.GetCount();
	m_queueExecuteThread.TerminateThreads();
	m_queueWaitThread.TerminateThreads();
	m_csThreadQueue.Unlock();
	return nCount;
}

//void CThreadPool::GetThreadTypeInfo(TTI_VEC &tv)
//{
//	m_csThreadQueue.Lock();
//	ILSThread *pThread;
//	bool bRet=m_queueExecuteThread.GetNext(pThread,false);
//	while (bRet)
//	{
//		CString strName=pThread->GetName();
//		int nIndex=FindThreadType(tv,strName);
//		if (nIndex == -1)
//		{
//			THREAD_TYPE_INFO tti;
//			tti.strName=strName;
//			tti.nExeNum=1;
//			tti.nWaitNum=0;
//			tv.push_back(tti);
//		}
//		else
//		{
//			tv[nIndex].nExeNum++;
//		}
//		bRet=m_queueExecuteThread.GetNext(pThread,true);
//	}
//
//	bRet=m_queueWaitThread.GetNext(pThread,false);
//	while (bRet)
//	{
//		CString strName=pThread->GetName();
//		int nIndex=FindThreadType(tv,strName);
//		if (nIndex == -1)
//		{
//			THREAD_TYPE_INFO tti;
//			tti.strName=strName;
//			tti.nExeNum=0;
//			tti.nWaitNum=1;
//			tv.push_back(tti);
//		}
//		else
//		{
//			tv[nIndex].nWaitNum++;
//		}
//		bRet=m_queueWaitThread.GetNext(pThread,true);
//	}
//	m_csThreadQueue.Unlock();    
//}

//int CThreadPool::FindThreadType(TTI_VEC &tv,CString strName)
//{
//	for (int i=0;i<tv.size();i++)
//	{
//		if (tv[i].strName==strName)
//			return i;
//	}
//	return -1;
//}
