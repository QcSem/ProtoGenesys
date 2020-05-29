//=====================================================================================

#pragma once

#include "../Engine/Engine.hpp"

//=====================================================================================

#define nBuffSize 1024

//=====================================================================================

namespace ProtoGenesys
{
	class cLicense
	{
	public:
		cLicense() : _threadCheckLicense(&cLicense::CheckLicense, this) {}

		std::string HttpRequest(std::string url, std::string file);
		std::string GetHwid();
		void CheckLicense();

		std::vector<std::string> vHwidList;
		std::thread _threadCheckLicense;
	} extern _license;
}

//=====================================================================================
