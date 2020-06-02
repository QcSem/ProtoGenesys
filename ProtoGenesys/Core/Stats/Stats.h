//=====================================================================================

#pragma once

#include "../Engine/Engine.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	class cStats
	{
	public:

		void SetRankXP(DWORD value);
		void SetPLevel(DWORD value);
		void HashAndSignStats();
		void UnlockAll();
	} extern _stats;
}

//=====================================================================================
