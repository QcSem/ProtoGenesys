//=====================================================================================

#include "../../StdAfx.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	cAimbot _aimBot;

	void cAimbot::StandardAim()
	{
		if (!_profiler.gSilentAim->Current.iValue && AimState.bTargetAcquired)
		{
			AimState.vAimAngles *= _profiler.gAimPower->Current.iValue / 100.0f;

			if (_profiler.gAutoAimTime->Current.iValue)
			{
				AimState.vAimAngles *= (float)AimState.iCurrentAimTime / (float)_profiler.gAutoAimTime->Current.iValue;
			}

			if (AimState.iCurrentAimDelay == _profiler.gAutoAimDelay->Current.iValue)
			{
				if (AimState.bLockonTarget)
				{
					ViewAngles->vAngles += AimState.vAimAngles;
				}
			}

			if (AimState.iCurrentZoomDelay == _profiler.gAutoZoomDelay->Current.iValue)
				if (_profiler.gAutoZoom->Current.iValue && _profiler.gAimBotMode->Current.iValue == cProfiler::AIMBOT_MODE_AUTO)
					SetZoomState(true);
		}
	}
	/*
	//=====================================================================================
	*/
	void cAimbot::SilentAim(sUserCmd* usercmd)
	{
		if (_profiler.gSilentAim->Current.iValue && AimState.bTargetAcquired)
		{
			if (AimState.bLockonTarget)
			{
				float flOldYaw = ShortToAngle(usercmd->iViewAngles[1]);

				usercmd->iViewAngles[0] += AngleToShort(AimState.vAimAngles.x);
				usercmd->iViewAngles[1] += AngleToShort(AimState.vAimAngles.y);

				_mathematics.MovementFix(usercmd, ShortToAngle(usercmd->iViewAngles[1]), flOldYaw, usercmd->szMove[0], usercmd->szMove[1]);
			}

			if (AimState.iCurrentZoomDelay == _profiler.gAutoZoomDelay->Current.iValue)
				if (_profiler.gAutoZoom->Current.iValue && _profiler.gAimBotMode->Current.iValue == cProfiler::AIMBOT_MODE_AUTO)
					SetZoomState(true);
		}
	}
	/*
	//=====================================================================================
	*/
	void cAimbot::AutoFire(sUserCmd* usercmd)
	{
		if (_profiler.gAutoFire->Current.iValue && AimState.bTargetAcquired)
		{
			if (AimState.iCurrentFireDelay == _profiler.gAutoFireDelay->Current.iValue)
			{
				if (AimState.bLockonTarget)
				{
					usercmd->iButtons[0] |= 0x80000000;
					usercmd->iButtons[1] |= 0x20000000;

					if (WeaponIsAkimbo())
						usercmd->iButtons[0] |= 0x100080;
				}
			}
		}
	}
	/*
	//=====================================================================================
	*/
	void cAimbot::SetAimState()
	{
		AimState.bTargetAcquired = (AimState.iTargetNum > -1);
		AimState.bAntiAimTargetAcquired = (AimState.iAntiAimTargetNum > -1);
		AimState.bLockonTarget = (_profiler.gAimBotMode->Current.iValue == cProfiler::AIMBOT_MODE_AUTO || (_profiler.gAimBotMode->Current.iValue == cProfiler::AIMBOT_MODE_MANUAL && CG->CEntity[CG->iClientNum].NextEntityState.LerpEntityState.eFlags1 & EF1_ZOOM));
		AimState.bIsAutoAiming = (AimState.bTargetAcquired && AimState.bLockonTarget);
		AimState.bIsAutoFiring = (_profiler.gAutoFire->Current.iValue && AimState.bIsAutoAiming);

		if (AimState.bLockonTarget)
		{
			if (AimState.iCurrentAimDelay == _profiler.gAutoAimDelay->Current.iValue)
				AimState.iCurrentAimTime += Sys_Milliseconds() - AimState.iDeltaTMR;

			AimState.iCurrentAimDelay += Sys_Milliseconds() - AimState.iDeltaTMR;
			AimState.iCurrentZoomDelay += Sys_Milliseconds() - AimState.iDeltaTMR;
			AimState.iCurrentFireDelay += Sys_Milliseconds() - AimState.iDeltaTMR;
		}

		AimState.iDeltaTMR = Sys_Milliseconds();

		if (AimState.iLastTargetNum != AimState.iTargetNum)
		{
			AimState.iLastTargetNum = AimState.iTargetNum;
			AimState.iCurrentAimTime = 0;
		}

		if (_targetList.EntityList[AimState.iTargetNum].iLastBone != _targetList.EntityList[AimState.iTargetNum].iBoneIndex)
		{
			_targetList.EntityList[AimState.iTargetNum].iLastBone = _targetList.EntityList[AimState.iTargetNum].iBoneIndex;
			AimState.iCurrentAimTime = 0;
		}

		if (!AimState.bTargetAcquired || _mainGui.bIsAirStuck)
			AimState.iCurrentAimDelay = AimState.iCurrentZoomDelay = AimState.iCurrentFireDelay = 0;

		if (AimState.iCurrentAimTime > _profiler.gAutoAimTime->Current.iValue)
			AimState.iCurrentAimTime = _profiler.gAutoAimTime->Current.iValue;

		if (AimState.iCurrentAimDelay > _profiler.gAutoAimDelay->Current.iValue)
			AimState.iCurrentAimDelay = _profiler.gAutoAimDelay->Current.iValue;

		if (AimState.iCurrentZoomDelay > _profiler.gAutoZoomDelay->Current.iValue)
			AimState.iCurrentZoomDelay = _profiler.gAutoZoomDelay->Current.iValue;

		if (AimState.iCurrentFireDelay > _profiler.gAutoFireDelay->Current.iValue)
			AimState.iCurrentFireDelay = _profiler.gAutoFireDelay->Current.iValue;

		if (AimState.bTargetAcquired)
		{
			ImVec3 vViewOrigin;
			GetPlayerViewOrigin(&vViewOrigin);

			AimState.vAimPosition = _targetList.EntityList[AimState.iTargetNum].vHitLocation;

			_mathematics.CalculateAimAngles(AimState.vAimPosition, vViewOrigin, AimState.vAimAngles);
			_mathematics.CalculateAntiAimAngles(CG->CEntity[AimState.iTargetNum].vOrigin, CG->vOrigin, AimState.vAntiAimAngles);
		}

		if (AimState.bAntiAimTargetAcquired)
		{
			_mathematics.CalculateAntiAimAngles(CG->CEntity[AimState.iAntiAimTargetNum].vOrigin, CG->vOrigin, AimState.vAntiAimAngles);
		}
	}
	/*
	//=====================================================================================
	*/
	void cAimbot::SetReloadState()
	{
		static DWORD iTick, iWait;

		auto Ready = [&]()
		{
			return ((Sys_Milliseconds() - iTick) > iWait);
		};

		auto Wait = [&](DWORD timeout)
		{
			iTick = Sys_Milliseconds();
			iWait = timeout;
		};

		if (_profiler.gReloadCancel->Current.iValue)
		{
			switch (ReloadState.iReloadState)
			{
			case RELOAD_DEFAULT:
				if (CG->PlayerState.iWeaponState[0] == 11 || CG->PlayerState.iWeaponState[1] == 11)
					ReloadState.iReloadState = RELOAD;

				break;

			case RELOAD:
				ReloadState.iAmmo = WeaponAmmoAvailable();
				ReloadState.iReloadState = RELOADING;

				break;

			case RELOADING:
				if (WeaponAmmoAvailable() > ReloadState.iAmmo)
					ReloadState.iReloadState = RELOADED;

				break;

			case RELOADED:
				if (Ready())
				{
					CycleWeapon(0);
					ReloadState.iIncrement++;
					Wait(35);
				}

				if (ReloadState.iIncrement == 2)
				{
					ReloadState.iAmmo = 0;
					ReloadState.iIncrement = 0;
					ReloadState.iReloadState = RELOAD_DEFAULT;
				}

				break;
			}
		}
	}
}

//=====================================================================================