//=====================================================================================

#include "../../StdAfx.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	cAimbot _aimBot;

	void cAimbot::StandardAim()
	{
		if ((!_profiler.gSilentAim->Custom.bValue || WeaponIsVehicle()) && AimState.bTargetAcquired)
		{
			AimState.vAimAngles[0] *= _profiler.gAimPower->Custom.iValue / 100.0f;
			AimState.vAimAngles[1] *= _profiler.gAimPower->Custom.iValue / 100.0f;

			if (_profiler.gAutoAimTime->Custom.iValue)
			{
				AimState.vAimAngles[0] *= (float)AimState.iCurrentAimTime / (float)_profiler.gAutoAimTime->Custom.iValue;
				AimState.vAimAngles[1] *= (float)AimState.iCurrentAimTime / (float)_profiler.gAutoAimTime->Custom.iValue;
			}

			if (AimState.iCurrentAimDelay == _profiler.gAutoAimDelay->Custom.iValue)
			{
				if (AimState.bLockonTarget)
				{
					ViewAngles->vAngles[0] += AimState.vAimAngles[0];
					ViewAngles->vAngles[1] += AimState.vAimAngles[1];
				}
			}

			if (AimState.iCurrentZoomDelay == _profiler.gAutoZoomDelay->Custom.iValue)
				if (_profiler.gAutoZoom->Custom.bValue && _profiler.gAimBotMode->Custom.iValue == cProfiler::AIMBOT_MODE_AUTO)
					SetZoomState(true);
		}
	}
	/*
	//=====================================================================================
	*/
	void cAimbot::SilentAim(sUserCmd* usercmd)
	{
		if (_profiler.gSilentAim->Custom.bValue && !WeaponIsVehicle() && AimState.bTargetAcquired)
		{
			if (AimState.bLockonTarget)
			{
				usercmd->iViewAngles[0] += AngleToShort(AimState.vAimAngles[0]);
				usercmd->iViewAngles[1] += AngleToShort(AimState.vAimAngles[1]);
			}

			if (AimState.iCurrentZoomDelay == _profiler.gAutoZoomDelay->Custom.iValue)
				if (_profiler.gAutoZoom->Custom.bValue && _profiler.gAimBotMode->Custom.iValue == cProfiler::AIMBOT_MODE_AUTO)
					SetZoomState(true);
		}
	}
	/*
	//=====================================================================================
	*/
	void cAimbot::AutoFire(sUserCmd* usercmd)
	{
		if (_profiler.gAutoFire->Custom.bValue && AimState.bTargetAcquired)
		{
			if (AimState.iCurrentFireDelay == _profiler.gAutoFireDelay->Custom.iValue)
			{
				if (_aimBot.AimState.bLockonTarget)
				{
					usercmd->iButtons[0] |= 0x80000000;
					usercmd->iButtons[1] |= 0x20000000;

					if (WeaponIsAkimbo())
						usercmd->iButtons[0] |= 0x100080;
				}
			}
		}
	}
}

//=====================================================================================