//=====================================================================================

#pragma once

#include "../Engine/Engine.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	class cAntiAim
	{
	public:

		Vector3 vAntiAimAngles;

		void AntiAimPitch(sUserCmd* usercmd);
		void AntiAimYaw(sUserCmd* usercmd);
		bool ReadyForAntiAim();
	} extern _antiAim;
}

//=====================================================================================
