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
			bool bTargetAcquired, bAntiAimTargetAcquired, bLockonTarget, bIsAutoAiming, bIsAutoFiring;
			int iCurrentAimTime, iCurrentAimDelay, iCurrentZoomDelay, iCurrentFireDelay, iDeltaTMR, iTargetNum, iLastTargetNum, iAntiAimTargetNum;
			ImVec3 vAimPosition, vAimAngles, vAntiAimAngles;
		} AimState;

		void StandardAim();
		void SilentAim(sUserCmd* usercmd);
		void AutoFire(sUserCmd* usercmd);
		void SetAimState();
		void CancelReloadAnimation();

	} extern _aimBot;
}

//=====================================================================================
