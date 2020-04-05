//=====================================================================================

#include "../../StdAfx.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	cAimbot _aimBot;

	void cAimbot::StandardAim()
	{
		if ((!_profiler.gSilentAim->Current.bValue || WeaponIsVehicle()) && AimState.bTargetAcquired)
		{
			AimState.vAimAngles[0] *= _profiler.gAimPower->Current.iValue / 100.0f;
			AimState.vAimAngles[1] *= _profiler.gAimPower->Current.iValue / 100.0f;

			if (_profiler.gAutoAimTime->Current.iValue)
			{
				AimState.vAimAngles[0] *= (float)AimState.iCurrentAimTime / (float)_profiler.gAutoAimTime->Current.iValue;
				AimState.vAimAngles[1] *= (float)AimState.iCurrentAimTime / (float)_profiler.gAutoAimTime->Current.iValue;
			}

			if (AimState.iCurrentAimDelay == _profiler.gAutoAimDelay->Current.iValue)
			{
				if (AimState.bLockonTarget)
				{
					ViewAngles->vAngles[0] += AimState.vAimAngles[0];
					ViewAngles->vAngles[1] += AimState.vAimAngles[1];
				}
			}

			if (AimState.iCurrentZoomDelay == _profiler.gAutoZoomDelay->Current.iValue)
				if (_profiler.gAutoZoom->Current.bValue && _profiler.gAimBotMode->Current.iValue == cProfiler::AIMBOT_MODE_AUTO)
					SetZoomState(true);
		}
	}
	/*
	//=====================================================================================
	*/
	void cAimbot::SilentAim(sUserCmd* usercmd)
	{
		if (_profiler.gSilentAim->Current.bValue && !WeaponIsVehicle() && AimState.bTargetAcquired)
		{
			if (AimState.bLockonTarget)
			{
				usercmd->iViewAngles[0] += AngleToShort(AimState.vAimAngles[0]);
				usercmd->iViewAngles[1] += AngleToShort(AimState.vAimAngles[1]);
			}

			if (AimState.iCurrentZoomDelay == _profiler.gAutoZoomDelay->Current.iValue)
				if (_profiler.gAutoZoom->Current.bValue && _profiler.gAimBotMode->Current.iValue == cProfiler::AIMBOT_MODE_AUTO)
					SetZoomState(true);
		}
	}
	/*
	//=====================================================================================
	*/
	void cAimbot::AutoFire(sUserCmd* usercmd)
	{
		if (_profiler.gAutoFire->Current.bValue && AimState.bTargetAcquired)
		{
			if (AimState.iCurrentFireDelay == _profiler.gAutoFireDelay->Current.iValue)
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