//=====================================================================================

#pragma once

#include "../Engine/Engine.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	class cAimbot
	{
	public:

		cAimbot() : _threadReloadCancel(&cAimbot::ReloadCancel, this) {}

		struct sAimState
		{
			bool bTargetAcquired, bAntiAimTargetAcquired, bLockonTarget, bIsAutoAiming, bIsAutoFiring;
			int iCurrentAimTime, iCurrentAimDelay, iCurrentZoomDelay, iCurrentFireDelay, iDeltaTMR, iTargetNum, iLastTargetNum, iAntiAimTargetNum;
			Vector3 vAimPosition, vAimAngles, vAntiAimAngles;
		} AimState;

		void StandardAim();
		void SilentAim(sUserCmd* usercmd);
		void AutoFire(sUserCmd* usercmd);
		void SetAimState();
		void ReloadCancel();

		std::thread _threadReloadCancel;
	} extern _aimBot;
}

//=====================================================================================
