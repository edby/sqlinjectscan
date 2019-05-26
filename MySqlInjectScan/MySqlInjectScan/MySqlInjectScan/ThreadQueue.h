// ThreadQueue.h: interface for the CThreadQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THREADQUEUE_H__9AFFD31E_A362_4921_A7A6_299A603B23E6__INCLUDED_)
#define AFX_THREADQUEUE_H__9AFFD31E_A362_4921_A7A6_299A603B23E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include "ILSThread.h"
using namespace std;


typedef list<ILSThread*> THREAD_LIST;
typedef list<ILSThread*>::iterator THREAD_ITER;

class CThreadQueue  
{
public:
	CThreadQueue();
	 ~CThreadQueue();
	void Push(ILSThread* elem);
	bool Pop(ILSThread* &elem);
	void Remove(ILSThread* elem);
	int  GetCount();
	void TerminateThreads();
	bool GetNext(ILSThread *&pThread,bool bNext=true);
private:
	THREAD_LIST m_lstQueue;
	THREAD_ITER m_iterQueue;
};

#endif // !defined(AFX_THREADQUEUE_H__9AFFD31E_A362_4921_A7A6_299A603B23E6__INCLUDED_)
