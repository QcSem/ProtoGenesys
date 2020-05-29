//=====================================================================================

#pragma once

#include "../Engine/Engine.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	class cAntiAim
	{
	public:

		ImVec3 vAntiAimAngles;

		void AntiAimPitch(sUserCmd* usercmd);
		void AntiAimYaw(sUserCmd* usercmd);
		bool ReadyForAntiAim();
	} extern _antiAim;
}

//=====================================================================================
