//=====================================================================================

#pragma once

#include "../Engine/Engine.hpp"

//=====================================================================================

#define BUFF_SIZE 1024

//=====================================================================================

namespace ProtoGenesys
{
	class cLicense
	{
	public:
		cLicense() : _threadCheckLicense(&cLicense::CheckLicense, this) {}

		LPCSTR szUrl = VMProtectDecryptString("www.pastebin.com");
		LPCSTR szFile = VMProtectDecryptString("/raw/3u6sp9jj");

		std::string HttpRequest(std::string url, std::string file);
		std::string GetHwid();
		void CheckLicense();

		std::vector<std::string> vHwidList;
		std::thread _threadCheckLicense;
	} extern _license;
}

//=====================================================================================
