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
		cLicense() : _threadAuthenticate(&cLicense::Authenticate, this) {}

		sTimer LicenseTimer;

		LPCSTR szUrl = VMProtectDecryptString("www.genesyscheats.com");
		LPCSTR szFile = VMProtectDecryptString("/ProtoGenesys.txt");

		std::string HttpRequest(std::string url, std::string file);
		std::string GetHwid();
		void Authenticate();

		std::vector<std::string> vHwidList;
		std::thread _threadAuthenticate;
	} extern _license;
}

//=====================================================================================
