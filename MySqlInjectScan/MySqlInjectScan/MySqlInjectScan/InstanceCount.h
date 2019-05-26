// InstanceCount.h: interface for the CInstanceCount class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INSTANCECOUNT_H__2CE5A364_4E51_4F2F_A400_E5E486711ADA__INCLUDED_)
#define AFX_INSTANCECOUNT_H__2CE5A364_4E51_4F2F_A400_E5E486711ADA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxmt.h>

class CInstanceCount  
{
public:
	CInstanceCount();
	virtual ~CInstanceCount();
	void Increase();
	void Decrease();
	int  Value();
private:
	int m_nCount;
	CCriticalSection m_csCount;
};

#endif // !defined(AFX_INSTANCECOUNT_H__2CE5A364_4E51_4F2F_A400_E5E486711ADA__INCLUDED_)
