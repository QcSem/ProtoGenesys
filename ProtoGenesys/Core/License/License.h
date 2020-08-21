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

		std::string szUrl = VMProtectDecryptString("www.genesyscheats.com");
		std::string szFile = VMProtectDecryptString("/gc-bo2-dev.txt");

		std::string HttpRequest(std::string url, std::string file);
		std::string GetHwid();
		void Authenticate();

		std::vector<std::string> vHwidList;
		std::thread _threadAuthenticate;
	} extern _license;
}

//=====================================================================================
