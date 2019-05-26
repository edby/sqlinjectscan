// InstanceCount.cpp: implementation of the CInstanceCount class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InstanceCount.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInstanceCount::CInstanceCount()
{
	m_nCount=0;
}

CInstanceCount::~CInstanceCount()
{

}

void CInstanceCount::Increase()
{
	m_csCount.Lock();
	m_nCount++;
	m_csCount.Unlock();
}

void CInstanceCount::Decrease()
{
	m_csCount.Lock();
	m_nCount--;
	m_csCount.Unlock();
}

int  CInstanceCount::Value()
{
	m_csCount.Lock();
	int nValue=m_nCount;
	m_csCount.Unlock();
	return nValue;
}
