// MyListBox.cpp : 实现文件
//

#include "stdafx.h"
#include "MySqlInjectScan.h"
#include "MyListBox.h"


// CMyListBox

IMPLEMENT_DYNAMIC(CMyListBox, CListBox)

CMyListBox::CMyListBox()
{

}

CMyListBox::~CMyListBox()
{
}


BEGIN_MESSAGE_MAP(CMyListBox, CListBox)
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()



// CMyListBox 消息处理程序




void CMyListBox::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	POINT   pt;
	GetCursorPos(&pt);
	int nCount = GetCount();
	ScreenToClient(&pt);
	//实现右键点击选中目标
	for (int i =0; i<nCount; i++)
	{
		bool a;
		CRect rc;

		GetItemRect(i, &rc);
		if(rc.PtInRect(pt))
		{
			SetCurSel(i);
			CMenu temp,*ptr;
			temp.LoadMenu(IDR_MENU2);
			ptr = temp.GetSubMenu(0);
			ClientToScreen(&pt);
			ptr->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,pt.x,pt.y,GetParent());


			break;
		}
	}
	CListBox::OnRButtonDown(nFlags, point);
}
