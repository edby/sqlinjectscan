
// MySqlInjectScan.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMySqlInjectScanApp:
// �йش����ʵ�֣������ MySqlInjectScan.cpp
//

class CMySqlInjectScanApp : public CWinApp
{
public:
	CMySqlInjectScanApp();

// ��д
public:
	 BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMySqlInjectScanApp theApp;