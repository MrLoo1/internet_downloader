#include"utility.h"
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include <string>

void util_err_exit(char const* ErrMsg)
{
	fprintf(stderr, "%s", ErrMsg);
	exit(-1);
}
int util_geterror()
{
	return GetLastError();
}
void util_errno_exit(char const* ErrMsg)
{
	char buf[256];
	int n = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, util_geterror(), 0, buf, 256, NULL);
	fprintf(stderr, "%s%s", ErrMsg, buf);
	exit(-1);
}

bool util_utf8_to_acp(std::string utf8_str, std::string &outstr)
{
	wchar_t * lpUnicodeStr = NULL;
	int nRetLen = 0;

	if (utf8_str.empty())  //���UTF8�ַ���ΪNULL������˳�
		return false;

	nRetLen = MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, NULL, NULL);  //��ȡת����Unicode���������Ҫ���ַ��ռ䳤��
	lpUnicodeStr = new WCHAR[nRetLen];  //ΪUnicode�ַ����ռ�
	if (lpUnicodeStr == nullptr)
		return false;
	nRetLen = MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, lpUnicodeStr, nRetLen);  //ת����Unicode����
	if (!nRetLen)  //ת��ʧ��������˳�
		return false;

	nRetLen = WideCharToMultiByte(CP_ACP, 0, lpUnicodeStr, -1, NULL, NULL, NULL, NULL);  //��ȡת����GBK���������Ҫ���ַ��ռ䳤��
	char *lpGBKStr = new char[nRetLen];
	if (lpGBKStr == nullptr)
		return false;
	nRetLen = ::WideCharToMultiByte(CP_ACP, 0, lpUnicodeStr, -1, lpGBKStr, nRetLen, NULL, NULL);  //ת����GBK����
	if (!nRetLen)  //ת��ʧ��������˳�
		return false;

	outstr = lpGBKStr;

	delete[] lpGBKStr;
	delete[] lpUnicodeStr;

	return true;
}

struct addrinfo * util_getaddrinfo(char const* host, char const* port, int family, int socktype, int protocol)
{
	char hostname[256];
	int returnval;
	struct addrinfo *result = NULL;
	struct addrinfo *ptr = NULL;
	struct addrinfo hints;

	if (!host)
	{
		int ret = gethostname(hostname, sizeof(hostname));
		if (ret == SOCKET_ERROR)
			util_errno_exit("gethostname failed:");
		host = hostname;
	}

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = family;
	hints.ai_socktype = socktype;
	hints.ai_protocol = protocol;

	returnval = getaddrinfo(host, port, &hints, &result);

	return returnval == 0 ? result : NULL;
}
