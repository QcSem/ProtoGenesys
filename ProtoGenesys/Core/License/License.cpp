//=====================================================================================

#include "../../StdAfx.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	cLicense _license;

	std::string cLicense::HttpRequest(std::string url, std::string file)
	{
		HINTERNET hInternet = InternetOpen(
			VMProtectDecryptString(PROGRAM_NAME),
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

		LPCSTR szAcceptTypes[] = { VMProtectDecryptString("text/*"), NULL };

		HINTERNET hRequest = HttpOpenRequest(hConnect, VMProtectDecryptString("GET"), file.c_str(), VMProtectDecryptString("HTTP/1.0"), NULL, szAcceptTypes,
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

		char szBuff[BUFF_SIZE];

		BOOL bKeepReading = true;
		DWORD dwBytesRead = -1;

		std::string szData;

		while (bKeepReading && dwBytesRead != 0)
		{
			bKeepReading = InternetReadFile(hRequest, szBuff, BUFF_SIZE, &dwBytesRead);
			szData.append(szBuff, dwBytesRead);
		}

		InternetCloseHandle(hRequest);
		InternetCloseHandle(hConnect);
		InternetCloseHandle(hInternet);

		return szData;
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
	void cLicense::Authenticate()
	{
		while (true)
		{
			if (LicenseTimer.Ready())
			{
				vHwidList.clear();

				std::string szLine;
				std::stringstream szStream;

				szStream << HttpRequest(szUrl, szFile);

				while (std::getline(szStream, szLine))
				{
					vHwidList.push_back(acut::FindAndEraseString(szLine, "\r"));
				}

				if (std::find_if(vHwidList.begin(), vHwidList.end(), [&](const std::string& hwid) { return !GetHwid().compare(hwid); }) == vHwidList.end())
				{
					exit(EXIT_FAILURE);
				}

				LicenseTimer.Wait(1000);
			}
		}
	}
}

//=====================================================================================