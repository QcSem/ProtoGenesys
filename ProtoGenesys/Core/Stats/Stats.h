//=====================================================================================

#pragma once

#include "../Engine/Engine.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	class cStats
	{
	public:

		void SetRankXP(int value);
		void SetPLevel(int value);
		void HashAndSignStats();
		void UnlockAll();
	} extern _stats;
}

//=====================================================================================
