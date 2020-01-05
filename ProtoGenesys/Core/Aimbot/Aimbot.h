//=====================================================================================

#pragma once

#include "../Engine/Engine.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	class cAimbot
	{
	public:

		struct sAimState
		{
			bool bTargetAcquired, bLockonTarget, bIsAutoAiming, bIsAutoFiring;
			int iCurrentAimTime, iCurrentAimDelay, iCurrentZoomDelay, iCurrentFireDelay, iDeltaTMR, iTargetNum, iLastTarget;
			Vector3 vAimbotPosition, vAimbotAngles;
		} AimState;

		void StandardAim();
		void SilentAim(sUserCmd* usercmd);
		void AutoFire(sUserCmd* usercmd);
	} extern _aimBot;
}

//=====================================================================================
