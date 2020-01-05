//=====================================================================================

#include "../../StdAfx.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	cTargetList _targetList;

	void cTargetList::GetInformation()
	{
		sTargetInfo TargetInfo;
		std::vector<sTargetInfo> vTargetInfo;

		_aimBot.AimState.iTargetNum = -1;

		bool bW2SSuccess = false;
		static int iCounter = 0;
		int iBonescanNum = iCounter % MAX_CLIENTS;

		for (int i = 0; i < MAX_CLIENTS; i++)
		{
			EntityInfo[i].bIsValid = false;

			if (!EntityIsValid(i))
				continue;

			LPVOID pDObj = GetDObj(&CG->Entity[i]);

			if (!pDObj)
				continue;

			Vector3 vMinTemp = { FLT_MAX, FLT_MAX, FLT_MAX }, vMaxTemp = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

			for (int j = BONE_HEAD; j < BONE_MAX; j++)
			{
				GetTagPosition(&CG->Entity[i], RegisterTag(vBones[j].second), pDObj, EntityInfo[i].vBones3D[j]);

				if (EntityInfo[i].vBones3D[j][0] < vMinTemp[0])
					vMinTemp[0] = EntityInfo[i].vBones3D[j][0];

				if (EntityInfo[i].vBones3D[j][0] > vMaxTemp[0])
					vMaxTemp[0] = EntityInfo[i].vBones3D[j][0];

				if (EntityInfo[i].vBones3D[j][1] < vMinTemp[1])
					vMinTemp[1] = EntityInfo[i].vBones3D[j][1];

				if (EntityInfo[i].vBones3D[j][1] > vMaxTemp[1])
					vMaxTemp[1] = EntityInfo[i].vBones3D[j][1];

				if (EntityInfo[i].vBones3D[j][2] < vMinTemp[2])
					vMinTemp[2] = EntityInfo[i].vBones3D[j][2];

				if (EntityInfo[i].vBones3D[j][2] > vMaxTemp[2])
					vMaxTemp[2] = EntityInfo[i].vBones3D[j][2];
			}

			VectorAverage(vMinTemp, vMaxTemp, EntityInfo[i].vCenter3D);

			if (WeaponNames[(BYTE)CG->Entity[i].iWeaponID1].szDisplayName)
				EntityInfo[i].szWeapon = WeaponNames[(BYTE)CG->Entity[i].iWeaponID1].szDisplayName;

			EntityInfo[i].bIsValid = true;

			Vector3 vViewOrigin;
			VectorCopy(CG->Entity[i].vOrigin, vViewOrigin);
			vViewOrigin[2] += M_METERS;

			EntityInfo[i].bW2SSuccess = _drawing.Calculate2D(EntityInfo[i].vBones3D, EntityInfo[i].vBones2D, EntityInfo[i].vPosition, EntityInfo[i].vDimentions) &&
				_drawing.Calculate3D(&CG->Entity[i], EntityInfo[i].vCenter3D, EntityInfo[i].vCorners3D, EntityInfo[i].vCorners2D) &&
				_mathematics.WorldToScreen(EntityInfo[i].vCenter3D, EntityInfo[i].vCenter2D) &&
				_mathematics.WorldToScreen(CG->Entity[i].vOrigin, EntityInfo[i].vLower) &&
				_mathematics.WorldToScreen(vViewOrigin, EntityInfo[i].vUpper);

			_mathematics.WorldToCompass(CG->Entity[i].vOrigin, _drawing.Compass.vCompassPosition, _drawing.Compass.flCompassSize, _drawing.Compass.vArrowPosition[i]);
			_mathematics.WorldToRadar(CG->Entity[i].vOrigin, _drawing.Radar.vRadarPosition, _drawing.Radar.flScale, _drawing.Radar.flRadarSize, _drawing.Radar.flBlipSize, _drawing.Radar.vBlipPosition[i]);

			if (!EntityIsEnemy(i))
			{
				EntityInfo[i].cColor = _profiler.gColorAllies->Custom.cValue;
				continue;
			}

			EntityInfo[i].cColor = _profiler.gColorAxis->Custom.cValue;

			if (_profiler.gBoneScan->Custom.iValue == cProfiler::BONESCAN_ONTIMER)
			{
				if (iBonescanNum == i)
				{
					EntityInfo[i].bIsVisible = Bonescan(&CG->Entity[i], EntityInfo[i].vBones3D, _profiler.gAutoWall->Custom.bValue, &EntityInfo[i].iBoneIndex);
					VectorCopy(EntityInfo[i].vBones3D[EntityInfo[i].iBoneIndex], EntityInfo[i].vHitLocation);
				}

				else
				{
					EntityInfo[i].bIsVisible = IsVisible(&CG->Entity[i], EntityInfo[i].vBones3D[EntityInfo[i].iBoneIndex], _profiler.gAutoWall->Custom.bValue, NULL);
					VectorCopy(EntityInfo[i].vBones3D[EntityInfo[i].iBoneIndex], EntityInfo[i].vHitLocation);
				}
			}

			else if (_profiler.gBoneScan->Custom.iValue == cProfiler::BONESCAN_IMMEDIATE)
			{
				EntityInfo[i].bIsVisible = Bonescan(&CG->Entity[i], EntityInfo[i].vBones3D, _profiler.gAutoWall->Custom.bValue, &EntityInfo[i].iBoneIndex);
				VectorCopy(EntityInfo[i].vBones3D[EntityInfo[i].iBoneIndex], EntityInfo[i].vHitLocation);
			}

			else
			{
				EntityInfo[i].iBoneIndex = _profiler.gAimBone->Custom.iValue;
				EntityInfo[i].bIsVisible = IsVisible(&CG->Entity[i], EntityInfo[i].vBones3D[EntityInfo[i].iBoneIndex], _profiler.gAutoWall->Custom.bValue, NULL);
				VectorCopy(EntityInfo[i].vBones3D[EntityInfo[i].iBoneIndex], EntityInfo[i].vHitLocation);
			}

			if (_mathematics.CalculateFOV(CG->RefDef.vViewOrg, EntityInfo[i].vHitLocation) > _profiler.gAimAngle->Custom.flValue)
				EntityInfo[i].bIsVisible = false;

			if (std::find(vIsTarget.begin(), vIsTarget.end(), TRUE) != vIsTarget.end())
				if (!vIsTarget[i])
					continue;

			if (EntityInfo[i].bIsVisible)
			{
				ImVec2 vTarget, vCenter = vCenter = ImGui::GetIO().DisplaySize / 2.0f;

				if (_mathematics.WorldToScreen(EntityInfo[i].vHitLocation, vTarget))
				{
					bW2SSuccess = true;

					TargetInfo.iIndex = i;
					TargetInfo.flDistance2D = GetDistance2D(vTarget, vCenter);

					vTargetInfo.push_back(TargetInfo);
				}

				if (!bW2SSuccess)
				{
					TargetInfo.iIndex = i;
					TargetInfo.flDistance3D = GetDistance3D(CG->Entity[i].vOrigin, CG->vOrigin);

					vTargetInfo.push_back(TargetInfo);
				}
			}
		}

		if (!vTargetInfo.empty())
		{
			if (bW2SSuccess)
			{
				std::sort(vTargetInfo.begin(), vTargetInfo.end(), [&](const sTargetInfo& a, const sTargetInfo& b) { return a.flDistance2D < b.flDistance2D; });
				_aimBot.AimState.iTargetNum = vTargetInfo.front().iIndex;
			}

			else
			{
				std::sort(vTargetInfo.begin(), vTargetInfo.end(), [&](const sTargetInfo& a, const sTargetInfo& b) { return a.flDistance3D < b.flDistance3D; });
				_aimBot.AimState.iTargetNum = vTargetInfo.front().iIndex;
			}

			vTargetInfo.clear();
		}

		_aimBot.AimState.bTargetAcquired = (_aimBot.AimState.iTargetNum > -1);
		_aimBot.AimState.bLockonTarget = (_profiler.gAimBotMode->Custom.iValue == cProfiler::AIMBOT_MODE_AUTO || (_profiler.gAimBotMode->Custom.iValue == cProfiler::AIMBOT_MODE_MANUAL && CG->Entity[CG->iClientNum].iFlags & EF_ZOOM));
		_aimBot.AimState.bIsAutoAiming = (_aimBot.AimState.bTargetAcquired && _aimBot.AimState.bLockonTarget);
		_aimBot.AimState.bIsAutoFiring = (_profiler.gAutoFire->Custom.bValue && _aimBot.AimState.bIsAutoAiming);

		if (_aimBot.AimState.bLockonTarget)
		{
			if (_aimBot.AimState.iCurrentAimDelay == _profiler.gAutoAimDelay->Custom.iValue)
				_aimBot.AimState.iCurrentAimTime += clock() - _aimBot.AimState.iDeltaTMR;

			_aimBot.AimState.iCurrentAimDelay += clock() - _aimBot.AimState.iDeltaTMR;
			_aimBot.AimState.iCurrentZoomDelay += clock() - _aimBot.AimState.iDeltaTMR;
			_aimBot.AimState.iCurrentFireDelay += clock() - _aimBot.AimState.iDeltaTMR;
		}

		_aimBot.AimState.iDeltaTMR = clock();

		if (_aimBot.AimState.iLastTarget != _aimBot.AimState.iTargetNum)
		{
			_aimBot.AimState.iLastTarget = _aimBot.AimState.iTargetNum;
			_aimBot.AimState.iCurrentAimTime = 0;
		}

		if (EntityInfo[_aimBot.AimState.iTargetNum].iLastBone != EntityInfo[_aimBot.AimState.iTargetNum].iBoneIndex)
		{
			EntityInfo[_aimBot.AimState.iTargetNum].iLastBone = EntityInfo[_aimBot.AimState.iTargetNum].iBoneIndex;
			_aimBot.AimState.iCurrentAimTime = 0;
		}

		if (!_aimBot.AimState.bTargetAcquired)
			_aimBot.AimState.iCurrentAimDelay = _aimBot.AimState.iCurrentZoomDelay = _aimBot.AimState.iCurrentFireDelay = 0;

		if (_aimBot.AimState.iCurrentAimTime > _profiler.gAutoAimTime->Custom.iValue)
			_aimBot.AimState.iCurrentAimTime = _profiler.gAutoAimTime->Custom.iValue;

		if (_aimBot.AimState.iCurrentAimDelay > _profiler.gAutoAimDelay->Custom.iValue)
			_aimBot.AimState.iCurrentAimDelay = _profiler.gAutoAimDelay->Custom.iValue;

		if (_aimBot.AimState.iCurrentZoomDelay > _profiler.gAutoZoomDelay->Custom.iValue)
			_aimBot.AimState.iCurrentZoomDelay = _profiler.gAutoZoomDelay->Custom.iValue;

		if (_aimBot.AimState.iCurrentFireDelay > _profiler.gAutoFireDelay->Custom.iValue)
			_aimBot.AimState.iCurrentFireDelay = _profiler.gAutoFireDelay->Custom.iValue;

		if (_aimBot.AimState.bTargetAcquired)
		{
			VectorCopy(EntityInfo[_aimBot.AimState.iTargetNum].vHitLocation, _aimBot.AimState.vAimbotPosition);
			_mathematics.CalculateAngles(CG->RefDef.vViewOrg, _aimBot.AimState.vAimbotPosition, _aimBot.AimState.vAimbotAngles);
		}

		iCounter++;
	}
	/*
	//=====================================================================================
	*/
	bool cTargetList::EntityIsValid(int index)
	{
		return (CG->Client[index].iInfoValid && (CG->Entity[index].iAlive & 2) && (&CG->Entity[index] != &CG->Entity[CG->iClientNum]));
	}
	/*
	//=====================================================================================
	*/
	bool cTargetList::EntityIsEnemy(int index)
	{
		return (((CG->Client[index].iTeam != 1) && (CG->Client[index].iTeam != 2)) || (CG->Client[index].iTeam != CG->Client[CG->iClientNum].iTeam));
	}
	/*
	//=====================================================================================
	*/
	bool cTargetList::IsVisible(sEntity* entity, Vector3 position, bool autowall, float* damage)
	{
		ApplyPrediction(entity, position);

		if (autowall && !WeaponIsVehicle())
		{
			float flDamage = _autoWall.Autowall(CG->RefDef.vViewOrg, position);

			if (damage)
				*damage = flDamage;

			if (flDamage >= 1.0f)
				return true;
		}

		else
		{
			bool bTraceHit = _autoWall.TraceBullet(CG->RefDef.vViewOrg, position, entity->iClientNum);

			if (bTraceHit)
				return true;
		}

		return false;
	}
	/*
	//=====================================================================================
	*/
	bool cTargetList::Bonescan(sEntity* entity, Vector3 bones3d[BONE_MAX], bool autowall, int* index)
	{
		bool bReturn = false;

		sDamageInfo DamageInfo;
		std::vector<sDamageInfo> vDamageInfo;

		for (int i = BONE_HEAD; i < BONE_MAX; i++)
		{
			if (autowall) {
				if (IsVisible(entity, bones3d[i], true, &DamageInfo.flDamage))
				{
					DamageInfo.iBoneIndex = i;
					vDamageInfo.push_back(DamageInfo);

					bReturn = true;
				}
			}

			else
			{
				if (IsVisible(entity, bones3d[i], false, NULL))
				{
					*index = i;
					return true;
				}
			}
		}

		if (!vDamageInfo.empty())
		{
			std::sort(vDamageInfo.begin(), vDamageInfo.end(), [&](const sDamageInfo& a, const sDamageInfo& b) { return a.flDamage > b.flDamage; });
			*index = vDamageInfo.front().iBoneIndex;
			vDamageInfo.clear();
		}

		return bReturn;
	}
	/*
	//=====================================================================================
	*/
	void cTargetList::ApplyPrediction(sEntity* entity, Vector3 position)
	{
		Vector3 vOldPosition, vNewPosition, vVelocity;

		EvaluateTrajectory(&entity->OldTrajectory, CG->OldSnapShot->iServerTime, vOldPosition);
		EvaluateTrajectory(&entity->NewTrajectory, CG->NewSnapShot->iServerTime, vNewPosition);

		VectorSubtract(vNewPosition, vOldPosition, vVelocity);

		VectorMA(position, *(int*)(dwCG + 0x48088) / 1000.0f, vVelocity, position);
		VectorMA(position, *(int*)(*(DWORD_PTR*)dwClientActive + 0x68) / 1000.0f, vVelocity, position);
	}
}

//=====================================================================================