//=====================================================================================

#pragma once

#include "../Engine/Engine.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	class cAimbot
	{
	public:

		typedef enum
		{
			RELOAD_DEFAULT,
			RELOAD,
			RELOADING,
			RELOADED
		} eReloadState;

		struct sAimState
		{
			bool bTargetAcquired, bAntiAimTargetAcquired, bLockonTarget, bIsAutoAiming, bIsAutoFiring;
			int iCurrentAimTime, iCurrentAimDelay, iCurrentZoomDelay, iCurrentFireDelay, iDeltaTMR, iTargetNum, iLastTargetNum, iAntiAimTargetNum;
			ImVec3 vAimPosition, vAimAngles, vAntiAimAngles;
		} AimState;

		struct sReloadState
		{
			eReloadState iReloadState = RELOAD_DEFAULT;
			int iAmmo, iIncrement;
		} ReloadState;

		void StandardAim();
		void SilentAim(sUserCmd* usercmd);
		void AutoFire(sUserCmd* usercmd);
		void SetAimState();
		void SetReloadState();

	} extern _aimBot;
}

//=====================================================================================
