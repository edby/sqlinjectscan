// ExtractLink.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "afxinet.h"
#include <iostream>
#include <vector>
#include "boost/regex.hpp"
using namespace std;

//编码格式转换
inline void UTF_8ToGB2312(CString &pOut, LPCSTR pText)
{
	int len=MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)pText, -1, NULL,0); 
	unsigned short* wszGBK = new unsigned short[len+1];       
	memset(wszGBK, 0, len * 2 + 2); 
	MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)pText, -1, (LPWSTR)wszGBK, len); 
	len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wszGBK, -1, NULL, 0, NULL, NULL); 
	char *szGBK=new char[len + 1]; 
	memset(szGBK, 0, len + 1); 
	WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)wszGBK, -1, (LPSTR)szGBK, len, NULL,NULL);
	pOut=szGBK;

	delete wszGBK;
	delete szGBK;
}

 CString GetUrlCode(CString szUrl)
{
	CInternetSession* m_pSession=NULL;
	DWORD dwStatusCode;
	CString data, content;
	CHttpFile* m_pFile = NULL;

		/////////
	try
	{
		/*HINTERNET hOpen = InternetOpen(NULL,PRE_CONFIG_INTERNET_ACCESS,NULL,  NULL, 0);  
		HINTERNET hRequest = InternetOpenUrl(hOpen, szUrl,NULL, 0, INTERNET_FLAG_TRANSFER_BINARY,0);  
		if( hOpen )  
		{  
			InternetCloseHandle(hOpen);  
		}  

		if(!hRequest)
			return "";  

		InternetCloseHandle(hRequest);  */
		//////////
		m_pSession = new CInternetSession("HttpClient");
	
		m_pFile = (CHttpFile *)m_pSession->OpenURL(szUrl, 1, INTERNET_FLAG_TRANSFER_ASCII|INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_RELOAD);
	
	
		if(m_pFile == NULL)
		{
			//m_pFile->Close();
			delete m_pFile;
			if(m_pSession)
				m_pSession->Close();
			delete m_pSession;
			return "";
		}
		if(m_pFile!=NULL)
			m_pFile->QueryInfoStatusCode(dwStatusCode);
		if(dwStatusCode != HTTP_STATUS_OK)
		{
			//::MessageBox(NULL,"can't open url", "error", NULL);
			if(m_pFile!=NULL)
				m_pFile->Close();
			delete m_pFile;
			if(m_pSession)
				m_pSession->Close();
			delete m_pSession;
			return "";
		}
		while (m_pFile -> ReadString(data))
		{
			content += data;
			data.Empty();
		}
	
		if(content.Find("charset=utf-8")!=-1)
		{
			CString szTmp;
			UTF_8ToGB2312(szTmp, content);
			content = szTmp;
		}
		return content;
	}
	catch(CInternetException* e)  //捕获异常
	{
		e->Delete();
		delete m_pFile;
		if(m_pSession)
			m_pSession->Close();
		delete m_pSession;
		return "";
	}
	/*catch(...)
	{
		delete m_pFile;
		if(m_pSession)
			m_pSession->Close();
		delete m_pSession;
		return "";
	}*/
}
 

/*字符串转urlencode*/
unsigned char ToHex(unsigned char x)   
{   
	return  x > 9 ? x + 55 : x + 48;   
}  

unsigned char FromHex(unsigned char x)   
{   
	unsigned char y;  
	if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;  
	else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;  
	else if (x >= '0' && x <= '9') y = x - '0';  
	else assert(0);  
	return y;  
}  

std::string UrlEncode(const std::string& str)  
{  
	std::string strTemp = "";  
	size_t length = str.length();  
	for (size_t i = 0; i < length; i++)  
	{  
		if (isalnum((unsigned char)str[i]) ||   
			(str[i] == '-') ||  
			(str[i] == '_') ||   
			(str[i] == '.') ||   
			(str[i] == '~'))  
			strTemp += str[i];  
		else if (str[i] == ' ')  
			strTemp += "+";  
		else  
		{  
			strTemp += '%';  
			strTemp += ToHex((unsigned char)str[i] >> 4);  
			strTemp += ToHex((unsigned char)str[i] % 16);  
		}  
	}  
	return strTemp;  
}  

std::string UrlDecode(const std::string& str)  
{  
	std::string strTemp = "";  
	size_t length = str.length();  
	for (size_t i = 0; i < length; i++)  
	{  
		if (str[i] == '+') strTemp += ' ';  
		else if (str[i] == '%')  
		{  
			assert(i + 2 < length);  
			unsigned char high = FromHex((unsigned char)str[++i]);  
			unsigned char low = FromHex((unsigned char)str[++i]);  
			strTemp += high*16 + low;  
		}  
		else strTemp += str[i];  
	}  
	return strTemp;  
}  
////////////////////////////////

 CString GetBaiduLinkByKey(CString szSearchKey, int iEachPageNum)
{
	string strTmp(szSearchKey.GetBuffer());
	strTmp = UrlEncode(strTmp);
	CString szTmp, szTmp2;
	szTmp = strTmp.c_str();
	szTmp2.Format("&rn=%d", iEachPageNum);
	szTmp = "http://www.baidu.com/s?q1="+szTmp+szTmp2;
	return szTmp;
}

string MakeFullPath(string szUrlPath, CString szDomain)
{
	if(szUrlPath[0] == 'h' && szUrlPath[1] == 't' && szUrlPath[2] == 't' && szUrlPath[3] == 'p')
	{
		if(szUrlPath.find(szDomain) == string::npos) //不是本网站
			return "";
		return szUrlPath;
	}
	if(szUrlPath[0] == '/')
		return "http://" + string(szDomain.GetBuffer()) + szUrlPath;
	if(szUrlPath[0] == '.') //暂时忽略./ 或../开头的路径
		return ""; 
	return "http://" + string(szDomain.GetBuffer()) + "/" + szUrlPath;
}

// CString GetNextPageUrl(CString szUrlCode)
//{
//	CFile file("code.txt", CFile::modeCreate|CFile::modeWrite);
//	file.Write(szUrlCode.GetBuffer(), szUrlCode.GetLength());
//	file.Close();
//	//boost::regex reg("[a-zA-Z0-9_\\./:]+?(\\.asp|\\.php|\\.jsp|\\.aspx)\\?[^\\'\\"]+");
//	
//	boost::regex reg("<a[^<>]+?href=\"([^\"]+)\"[^<>]+>下一页");
//	std::string testString(szUrlCode.GetBuffer());
//	boost::smatch what;
//	std::string::const_iterator start = testString.begin();
//	std::string::const_iterator end = testString.end();
//	if( boost::regex_search(start, end, what, reg) )
//	{
//		std::string name(what[1].first, what[1].second);
//		if(name[0] == '/')
//			name = "http://www.baidu.com" + name;
//		return name.c_str();
//	}
//	else
//	{
//		MessageBox(NULL, "没有下一页", "Error!", NULL);
//		return "";
//	}
//}

 void GetCurPageAllSubLink(CString szUrlCode, vector<CString>& vec)
{
	boost::regex reg("style=\"text-decoration:none;\">([^<]+)<");
	std::string testString(szUrlCode.GetBuffer());
	boost::smatch what;
	std::string::const_iterator start = testString.begin();
	std::string::const_iterator end = testString.end();
	CString szSubUrl;
	int iPos;
	while( boost::regex_search(start, end, what, reg) )
	{
		std::string strSubLink(what[1].first, what[1].second);
		szSubUrl = strSubLink.c_str();
		iPos = szSubUrl.Find('/');
		if(iPos!=-1)
			szSubUrl = szSubUrl.Left(iPos);
		else
			szSubUrl.Replace("...&nbsp;", "");
		const char* p = szSubUrl.GetBuffer();
		vec.push_back(szSubUrl);
		start = what[0].second;
	}
}

//批量获取百度搜索结果页面中的域名列表 形如 http://www.xxx.com
 void GetAllBaiduLinkByKey(CString szSearchKey, int iPageNum/*页数*/, int iEachPageNum/*每页条数*/, vector<CString>& vec)
{
	CString szLoopUrl, szLoopUrlCode, szPn;
	szLoopUrl = GetBaiduLinkByKey(szSearchKey, iEachPageNum);
	CString szNextPageUrl = szLoopUrl;
	int iPos = 0, iPn;
	if(iPageNum == 0)
		iPageNum = 1000;
	do
	{
		++iPos;
		iPn = iPos*iEachPageNum;
		szLoopUrlCode = GetUrlCode(szNextPageUrl);
		if(szLoopUrlCode == "")
			break;
		//获取下一页链接
		szPn.Format("&pn=%d",iPn);
		szNextPageUrl = szLoopUrl + szPn;
		//正则解析获取当前页的子页链接(百度快照前的真实域名链接)
		GetCurPageAllSubLink(szLoopUrlCode, vec);
	}while(szNextPageUrl!=""&&--iPageNum);

}

//获取单页所有形如xxx.asp?id=3的链接
 void ExtractSinglePageDynamicLink(CString szUrlCode, CString szDomain, vector<CString>& linkVec)
{
	char* pTmp = szDomain.GetBuffer();

	boost::cmatch mat;
	//boost::regex reg("(http://([A-Za-z0-9_.:]+/))?([(\\w*/)|(\\./)|(\\.\\./)])*((\\w+((\\.aspx)|(\\.php)|(\\.jsp)|(\\.asp))((\\?)(\\w+=([A-Za-z0-9\\u0391-\\uFFE5_.]+)&?)+)?)|(\\w+\\?((\\w+=([A-Za-z0-9\\u0391-\\uFFE5_.]+)&?)+)))"); 
	//boost::regex reg("[a-zA-Z0-9_\\./:]+?\\.asp(\\?.+?=[a-zA-Z0-9]+|)"); 
	//boost::regex reg("[a-zA-Z0-9_\\./:]+?(\\.asp|\\.php|\\.jsp|\\.aspx)\\?([^\'\"\\|\\s%]|[\\u4e00-\\u9fa5])+"); 
	//boost::regex reg("[a-zA-Z0-9_\\./:]+?(\\.asp|\\.php|\\.jsp|\\.aspx)\\?[a-zA-Z0-9_&=;%\\u4e00-\\u9fa5]+"); 
	boost::regex reg("[a-zA-Z0-9_\\./:]+?(\\.asp|\\.php|\\.jsp|\\.aspx)\\?[^\'\"\\|\\s<>]+"); 

	string str(szUrlCode.GetBuffer());  
	boost::smatch what;
	std::string::const_iterator start = str.begin();  
	std::string::const_iterator end = str.end();  
	
	//CFile file("link.txt", CFile::modeCreate|CFile::modeWrite);
	try
	{
		while (boost::regex_search(start, end, what, reg)) 
	{  
		start = what[0].second;
		string szTmp = MakeFullPath(what[0], szDomain);
		if(szTmp == "")
			continue;
		//szTmp += "\r\n";
		CString str;
		str = szTmp.c_str();
		linkVec.push_back(str);

		//const char* pTmp = szTmp.c_str();
		//file.Write(pTmp, strlen(pTmp));
		
	} 
	}
	catch(std::exception& e)
	{
		//MessageBox(NULL,e.what(),"error",NULL);
	}

	//file.Close();
}

bool IsRepeatSqlLink(CString szUrl1, CString szUrl2, char cFlag)
{
	if(cFlag!='?') //判断域名是否相同
		return szUrl1 == szUrl2;
	int iPos1, iPos2;
	iPos1 = szUrl1.Find("?");
	iPos2 = szUrl2.Find("?");
	return szUrl1.Left(iPos1) == szUrl2.Left(iPos2);
}

 void FilterLinkVec(vector<CString>& vec, char cFlag='￥')
{
	int iSize = vec.size();
	int* iFlagStr = new int[iSize];
	//memset(iFlagStr, 1, iSize*4);
	for(int i = 0; i < iSize; i++)
		iFlagStr[i] = 1;
	for(int i = 0; i != vec.size(); ++i)
	{
		char* strTmp = vec[i].GetBuffer();
		if( 0 == iFlagStr[i])
			continue;
		for(int j = i+1; j != vec.size(); ++j)
		{
			char* strTmp2 = vec[j].GetBuffer();
			if( 0 == iFlagStr[j])
				continue;
			if(IsRepeatSqlLink(vec[i], vec[j], cFlag))
				iFlagStr[j] = 0;
		}
	}
	vector<CString> vecTmp;
	/*for(int i = 0; i < iSize; i++)
		iTmp = iFlagStr[i];*/
	for(int i = 0; i < vec.size(); ++i)
	{
		if( 1 == iFlagStr[i])
		{
			vecTmp.push_back(vec[i]);
		}
	}
	vec.clear();
	vec.assign(vecTmp.begin(), vecTmp.end());
}

 void ExtractWebAllDynamicLink(CString szUrlCode, CString szDomain, vector<CString>& vec)
{
	//获取此网站首页的所有动态链接
	ExtractSinglePageDynamicLink(szUrlCode, szDomain, vec);
	//过滤不是本网站和不是动态页的链接
	FilterLinkVec(vec, '?');

	//下一步可考虑进一步获取子页中的所有链接
}

 bool IsSimilarCode(CString szUrlCode1, CString szUrlCode2)
 {
	 int iLen1, iLen2;
	 iLen1 = szUrlCode1.GetLength();
	 iLen2 = szUrlCode2.GetLength();
	 if((abs(2.0*iLen1/(iLen1+iLen2))-1)<0.0008)
		 return true;
	 return false;
 }

 bool CanUrlInject(CString szUrl)
 {
	 CString szUrlCode, szNumTypeUrlCode1, szNumTypeUrlCode2, szCharTypeUrlCode1, szCharTypeUrlCode2;
	 szUrlCode = GetUrlCode(szUrl);
	 if(szUrlCode == "")
		 return false;
	 //数字型注入
	 szNumTypeUrlCode1 = GetUrlCode(szUrl+" and 1=1");
	 szNumTypeUrlCode2 = GetUrlCode(szUrl+" and 1=2");
	 CString szTmp = szNumTypeUrlCode2;
	 szTmp.TrimLeft();
	 szTmp.TrimRight();
	 if(IsSimilarCode(szUrlCode,szNumTypeUrlCode1)&&(!IsSimilarCode(szUrlCode, szNumTypeUrlCode2)||szTmp==""))
		 return true;
	 
	 //字符型注入
	 /*szCharTypeUrlCode1 = GetUrlCode(szUrl+"' and '1'='1");
	 szCharTypeUrlCode2 = GetUrlCode(szUrl+ "' and '1'='2");
	 if(IsSimilarCode(szUrlCode, szCharTypeUrlCode1)&&!IsSimilarCode(szUrlCode, szCharTypeUrlCode2))
		 return true;*/

	//多参数的情况*/

	 return false;
 }

 bool IsStandardDynamicLink(CString szUrl)
 {
	 boost::regex reg("http://([^/]+/)+[^\\.]+(\\.asp|\\.aspx|\\.jsp|\\.php)\\?[^=]+=\\d+");
	 string str(szUrl.GetBuffer());
	 bool r =  boost::regex_match(str, reg);
	 return r;
 }