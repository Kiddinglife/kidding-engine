#ifndef __PATH_UTIL_H__
#define __PATH_UTIL_H__

//#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <string>
#include <tchar.h>
#include <iostream>
//struct PathUtil
//{
char* ConvertLPWSTRToLPSTR(LPWSTR lpwszStrIn)
{
	LPSTR pszOut = NULL;
	if( lpwszStrIn != NULL )
	{
		int nInputStrLen = wcslen(lpwszStrIn);

		// Double NULL Termination
		int nOutputStrLen = WideCharToMultiByte(CP_ACP, 0, lpwszStrIn, nInputStrLen, NULL, 0, 0, 0) + 2;
		pszOut = new char[nOutputStrLen];

		if( pszOut )
		{
			memset(pszOut, 0x00, nOutputStrLen);
			WideCharToMultiByte(CP_ACP, 0, lpwszStrIn, nInputStrLen, pszOut, nOutputStrLen, 0, 0);
		}
	}
	return pszOut;
}

std::string GetBinDirectory()
{
	TCHAR path[2048];
	GetModuleFileName(NULL, path, 2048);
	std::string sWorkingDirectory(ConvertLPWSTRToLPSTR(path));
	return sWorkingDirectory.substr(0, sWorkingDirectory.find_last_of("\\")).append("\\");
}


std::string GetMonoDirectory()
{
	return GetBinDirectory().append("Mono\\");
}

std::string GetLibDirectory()
{
	return GetMonoDirectory().append("lib\\");
}

std::string GetConfigDirectory()
{
	return GetMonoDirectory().append("etc\\");
}
//};

#endif //__PATH_UTIL_H__