// ThreadQueue.cpp: implementation of the CThreadQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ThreadQueue.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CThreadQueue::CThreadQueue()
{

}

CThreadQueue::~CThreadQueue()
{

}

void CThreadQueue::Push(ILSThread* elem)
{
	m_lstQueue.push_back(elem);
}

bool CThreadQueue::Pop(ILSThread* &elem)
{
	if (m_lstQueue.empty())
		return false;
	elem=m_lstQueue.front();
	m_lstQueue.pop_front();
	return true;
}

int CThreadQueue::GetCount()
{
	return m_lstQueue.size();
}

void CThreadQueue::Remove(ILSThread* elem)
{
    m_lstQueue.remove(elem);
}

void CThreadQueue::TerminateThreads()
{
	THREAD_ITER iter=m_lstQueue.begin();
	for (;iter!=m_lstQueue.end();)
	{
		ILSThread *pThread=*iter;
		if (pThread->TerminateThread()==TT_FORCE)
		{
			iter=m_lstQueue.erase(iter);
			delete pThread;
			continue;
		}
		iter++;
	}
}

bool CThreadQueue::GetNext(ILSThread *&pThread,bool bNext)
{
	if (bNext)
		m_iterQueue++;
	else
		m_iterQueue=m_lstQueue.begin();
	if (m_iterQueue==m_lstQueue.end())
		return false;
	pThread=*m_iterQueue;
	return true;
}