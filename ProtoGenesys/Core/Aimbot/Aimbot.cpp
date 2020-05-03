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
				float flOldYaw = ShortToAngle(usercmd->iViewAngles[1]);

				usercmd->iViewAngles[0] += AngleToShort(AimState.vAimAngles[0]);
				usercmd->iViewAngles[1] += AngleToShort(AimState.vAimAngles[1]);

				_mathematics.MovementFix(usercmd, ShortToAngle(usercmd->iViewAngles[1]), flOldYaw, usercmd->szMove[0], usercmd->szMove[1]);
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
		AimState.bLockonTarget = (_profiler.gAimBotMode->Current.iValue == cProfiler::AIMBOT_MODE_AUTO || (_profiler.gAimBotMode->Current.iValue == cProfiler::AIMBOT_MODE_MANUAL && CG->Entity[CG->iClientNum].NextEntityState.LerpEntityState.eFlags1 & EF1_ZOOM));
		AimState.bIsAutoAiming = (AimState.bTargetAcquired && AimState.bLockonTarget);
		AimState.bIsAutoFiring = (_profiler.gAutoFire->Current.bValue && AimState.bIsAutoAiming);

		if (AimState.bLockonTarget)
		{
			if (AimState.iCurrentAimDelay == _profiler.gAutoAimDelay->Current.iValue)
				AimState.iCurrentAimTime += clock() - AimState.iDeltaTMR;

			AimState.iCurrentAimDelay += clock() - AimState.iDeltaTMR;
			AimState.iCurrentZoomDelay += clock() - AimState.iDeltaTMR;
			AimState.iCurrentFireDelay += clock() - AimState.iDeltaTMR;
		}

		AimState.iDeltaTMR = clock();

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
			Vector3 vViewOrigin;
			GetPlayerViewOrigin(vViewOrigin);

			VectorCopy(_targetList.EntityList[AimState.iTargetNum].vHitLocation, AimState.vAimPosition);

			_mathematics.CalculateAimAngles(AimState.vAimPosition, vViewOrigin, AimState.vAimAngles);
			_mathematics.CalculateAntiAimAngles(CG->Entity[AimState.iTargetNum].vOrigin, CG->vOrigin, AimState.vAntiAimAngles);
		}

		if (AimState.bAntiAimTargetAcquired)
		{
			_mathematics.CalculateAntiAimAngles(CG->Entity[AimState.iAntiAimTargetNum].vOrigin, CG->vOrigin, AimState.vAntiAimAngles);
		}
	}
	/*
	//=====================================================================================
	*/
	void cAimbot::ReloadCancel()
	{
		if (!(CG->PlayerState.iWeaponState[0] == 11 || CG->PlayerState.iWeaponState[1] == 11))
			return;

		if (!CG->PlayerState.iWeaponDelay)
		{
			CycleWeapon(0);
			Sleep(1);
			CycleWeapon(0);
		}
	}
}

//=====================================================================================