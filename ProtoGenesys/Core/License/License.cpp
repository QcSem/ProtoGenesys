//=====================================================================================

#include "../../StdAfx.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	cLicense _license;

	std::string cLicense::HttpRequest(std::string url, std::string file)
	{
		HINTERNET hInternet = InternetOpen(
			VMProtectDecryptString("ProtoGenesys"),
			INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
		if (hInternet == NULL)
		{
			return "";
		}

		HINTERNET hConnect = InternetConnect(hInternet, url.c_str(), INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, NULL);
		if (hConnect == NULL)
		{
			InternetCloseHandle(hInternet);
			return "";
		}

		LPCSTR parrAcceptTypes[] = { VMProtectDecryptString("text/*"), NULL };

		HINTERNET hRequest = HttpOpenRequest(hConnect, VMProtectDecryptString("GET"), file.c_str(), VMProtectDecryptString("HTTP/1.0"), NULL, parrAcceptTypes,
			INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_RELOAD, 0);
		if (hRequest == NULL)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hInternet);
			return "";
		}

		BOOL bRequestSent = HttpSendRequest(hRequest, NULL, 0, NULL, 0);
		if (!bRequestSent)
		{
			InternetCloseHandle(hRequest);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hInternet);
			return "";
		}

		char buff[nBuffSize];

		BOOL bKeepReading = true;
		DWORD dwBytesRead = -1;

		std::string data;

		while (bKeepReading && dwBytesRead != 0)
		{
			bKeepReading = InternetReadFile(hRequest, buff, nBuffSize, &dwBytesRead);
			data.append(buff, dwBytesRead);
		}

		InternetCloseHandle(hRequest);
		InternetCloseHandle(hConnect);
		InternetCloseHandle(hInternet);

		return data;
	}
	/*
	//=====================================================================================
	*/
	std::string cLicense::GetHwid()
	{
		int nSize = VMProtectGetCurrentHWID(NULL, 0);
		char* pBuf = new char[nSize];
		VMProtectGetCurrentHWID(pBuf, nSize);
		std::string hwid(pBuf);
		delete[] pBuf;
		return hwid;
	}
	/*
	//=====================================================================================
	*/
	void cLicense::CheckLicense()
	{
		while (true)
		{
			vHwidList.clear();

			std::string szLine;
			std::stringstream szStream;

			szStream << HttpRequest(VMProtectDecryptString("www.pastebin.com"), VMProtectDecryptString("/raw/3u6sp9jj"));

			while (std::getline(szStream, szLine))
			{
				vHwidList.push_back(szLine);
			}

			if (std::find_if(vHwidList.begin(), vHwidList.end(), [&](const std::string& hwid) { return !strncmp(hwid.c_str(), GetHwid().c_str(), GetHwid().length()); }) == vHwidList.end())
			{
				exit(EXIT_FAILURE);
			}

			Sleep(60000);
		}
	}
}

//=====================================================================================