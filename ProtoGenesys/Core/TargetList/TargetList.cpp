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

		static int iCounter = 0;
		int iBonescanNum = iCounter % MAX_CLIENTS;

		for (int i = 0; i < MAX_ENTITIES; i++)
		{
			EntityList[i].bIsValid = false;

			if (!EntityIsValid(i))
				continue;

			if (CG->Entity[i].NextEntityState.wEntityType == ET_PLAYER)
			{
				LPVOID pDObj = GetDObj(&CG->Entity[i]);

				if (!pDObj)
					continue;

				Vector3 vMinTemp = { FLT_MAX, FLT_MAX, FLT_MAX }, vMaxTemp = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

				for (int j = BONE_HEAD; j < BONE_MAX; j++)
				{
					GetTagPosition(&CG->Entity[i], RegisterTag(vBones[j].second), pDObj, EntityList[i].vBones3D[j]);

					for (int k = 0; k < 3; k++)
					{
						if (EntityList[i].vBones3D[j][k] < vMinTemp[k])
							vMinTemp[k] = EntityList[i].vBones3D[j][k];

						if (EntityList[i].vBones3D[j][k] > vMaxTemp[k])
							vMaxTemp[k] = EntityList[i].vBones3D[j][k];
					}
				}

				VectorAverage(vMinTemp, vMaxTemp, EntityList[i].vCenter3D);
			}

			if (WeaponNames[(BYTE)CG->Entity[i].NextEntityState.iWeaponID].szDisplayName)
				EntityList[i].szWeapon = WeaponNames[(BYTE)CG->Entity[i].NextEntityState.iWeaponID].szDisplayName;

			EntityList[i].bIsValid = true;

			if (CG->Entity[i].NextEntityState.wEntityType == ET_PLAYER)
			{
				Vector3 vViewOrigin;
				VectorCopy(CG->Entity[i].vOrigin, vViewOrigin);
				vViewOrigin[2] += M_METERS;

				EntityList[i].bW2SSuccess = _drawing.Calculate2D(EntityList[i].vBones3D, EntityList[i].vBones2D, EntityList[i].vPosition, EntityList[i].vDimentions) &&
					_drawing.Calculate3D(&CG->Entity[i], EntityList[i].vCenter3D, EntityList[i].vCorners3D, EntityList[i].vCorners2D) &&
					_mathematics.WorldToScreen(EntityList[i].vCenter3D, EntityList[i].vCenter2D) &&
					_mathematics.WorldToScreen(CG->Entity[i].vOrigin, EntityList[i].vLower) &&
					_mathematics.WorldToScreen(vViewOrigin, EntityList[i].vUpper);

				_mathematics.WorldToCompass(CG->Entity[i].vOrigin, _drawing.Compass.vCompassPosition, _drawing.Compass.flCompassSize, _drawing.Compass.vArrowPosition[i]);
				_mathematics.WorldToRadar(CG->Entity[i].vOrigin, _drawing.Radar.vRadarPosition, _drawing.Radar.flScale, _drawing.Radar.flRadarSize, _drawing.Radar.flBlipSize, _drawing.Radar.vBlipPosition[i]);

				if (!EntityIsEnemy(i))
				{
					EntityList[i].cColor = _profiler.gColorAllies->Custom.cValue;
					continue;
				}

				EntityList[i].cColor = _profiler.gColorAxis->Custom.cValue;
			}

			else
			{
				EntityList[i].bW2SSuccess = _mathematics.WorldToScreen(CG->Entity[i].vOrigin, EntityList[i].vCenter2D);
				continue;
			}

			if (_profiler.gBoneScan->Custom.iValue == cProfiler::BONESCAN_ONTIMER)
			{
				if (iBonescanNum == i)
				{
					EntityList[i].bIsVisible = Bonescan(&CG->Entity[i], EntityList[i].vBones3D, _profiler.gAutoWall->Custom.bValue, &EntityList[i].iBoneIndex);
					VectorCopy(EntityList[i].vBones3D[EntityList[i].iBoneIndex], EntityList[i].vHitLocation);
				}

				else
				{
					EntityList[i].bIsVisible = IsVisible(&CG->Entity[i], EntityList[i].vBones3D[EntityList[i].iBoneIndex], _profiler.gAutoWall->Custom.bValue, NULL);
					VectorCopy(EntityList[i].vBones3D[EntityList[i].iBoneIndex], EntityList[i].vHitLocation);
				}
			}

			else if (_profiler.gBoneScan->Custom.iValue == cProfiler::BONESCAN_IMMEDIATE)
			{
				EntityList[i].bIsVisible = Bonescan(&CG->Entity[i], EntityList[i].vBones3D, _profiler.gAutoWall->Custom.bValue, &EntityList[i].iBoneIndex);
				VectorCopy(EntityList[i].vBones3D[EntityList[i].iBoneIndex], EntityList[i].vHitLocation);
			}

			else
			{
				EntityList[i].iBoneIndex = _profiler.gAimBone->Custom.iValue;
				EntityList[i].bIsVisible = IsVisible(&CG->Entity[i], EntityList[i].vBones3D[EntityList[i].iBoneIndex], _profiler.gAutoWall->Custom.bValue, NULL);
				VectorCopy(EntityList[i].vBones3D[EntityList[i].iBoneIndex], EntityList[i].vHitLocation);
			}

			if (_mathematics.CalculateFOV(EntityList[i].vHitLocation) > _profiler.gAimAngle->Custom.flValue)
				EntityList[i].bIsVisible = false;

			if (std::find(vIsTarget.begin(), vIsTarget.end(), TRUE) != vIsTarget.end())
				if (!vIsTarget[i])
					continue;

			if (EntityList[i].bIsVisible)
			{
				TargetInfo.iIndex = i;

				TargetInfo.flFOV = _mathematics.CalculateFOV(EntityList[i].vHitLocation);
				TargetInfo.flDistance = _mathematics.CalculateDistance(CG->Entity[i].vOrigin, CG->vOrigin);

				vTargetInfo.push_back(TargetInfo);
			}
		}

		if (!vTargetInfo.empty())
		{
			if (_profiler.gSortMethod->Custom.iValue == cProfiler::SORT_METHOD_FOV)
			{
				std::sort(vTargetInfo.begin(), vTargetInfo.end(), [&](const sTargetInfo& a, const sTargetInfo& b) { return a.flFOV < b.flFOV; });
				_aimBot.AimState.iTargetNum = vTargetInfo.front().iIndex;
			}

			else if (_profiler.gSortMethod->Custom.iValue == cProfiler::SORT_METHOD_DISTANCE)
			{
				std::sort(vTargetInfo.begin(), vTargetInfo.end(), [&](const sTargetInfo& a, const sTargetInfo& b) { return a.flDistance < b.flDistance; });
				_aimBot.AimState.iTargetNum = vTargetInfo.front().iIndex;
			}
		}

		_aimBot.AimState.bTargetAcquired = (_aimBot.AimState.iTargetNum > -1);
		_aimBot.AimState.bLockonTarget = (_profiler.gAimBotMode->Custom.iValue == cProfiler::AIMBOT_MODE_AUTO || (_profiler.gAimBotMode->Custom.iValue == cProfiler::AIMBOT_MODE_MANUAL && CG->Entity[CG->iClientNum].NextEntityState.LerpEntityState.eFlags1 & EF_ZOOM));
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

		if (EntityList[_aimBot.AimState.iTargetNum].iLastBone != EntityList[_aimBot.AimState.iTargetNum].iBoneIndex)
		{
			EntityList[_aimBot.AimState.iTargetNum].iLastBone = EntityList[_aimBot.AimState.iTargetNum].iBoneIndex;
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
			Vector3 vViewOrigin;

			GetPlayerViewOrigin(vViewOrigin);

			VectorCopy(EntityList[_aimBot.AimState.iTargetNum].vHitLocation, _aimBot.AimState.vAimbotPosition);
			_mathematics.CalculateAngles(WeaponIsVehicle() ? CG->RefDef.vViewOrg : vViewOrigin, _aimBot.AimState.vAimbotPosition, _aimBot.AimState.vAimbotAngles);
		}

		iCounter++;
	}
	/*
	//=====================================================================================
	*/
	bool cTargetList::EntityIsValid(int index)
	{
		if (CG->Entity[index].NextEntityState.wEntityType == ET_PLAYER)
		{
			if (CG->Client[index].iInfoValid && (CG->Entity[index].iAlive & 2) && (&CG->Entity[index] != &CG->Entity[CG->iClientNum]))
				return true;
		}

		else
		{
			if (CG->Entity[index].wValid && (CG->Entity[index].iAlive & 2) && (&CG->Entity[index] != &CG->Entity[CG->iClientNum]))
				return true;
		}

		return false;
	}
	/*
	//=====================================================================================
	*/
	bool cTargetList::EntityIsEnemy(int index)
	{
		if (CG->Entity[index].NextEntityState.wEntityType == ET_PLAYER)
		{
			if (((CG->Client[index].iTeam1 != TEAM_ALLIES) && (CG->Client[index].iTeam1 != TEAM_AXIS)) || (CG->Client[index].iTeam1 != CG->Client[CG->iClientNum].iTeam1))
				return true;
		}

		else
		{
			if (((CG->Client[CG->Entity[index].NextEntityState.wOtherEntityNum].iTeam1 != TEAM_ALLIES) && (CG->Client[CG->Entity[index].NextEntityState.wOtherEntityNum].iTeam1 != TEAM_AXIS)) || (CG->Client[CG->Entity[index].NextEntityState.wOtherEntityNum].iTeam1 != CG->Client[CG->iClientNum].iTeam1))
				return true;
		}

		return false;
	}
	/*
	//=====================================================================================
	*/
	bool cTargetList::IsVisible(sEntity* entity, Vector3 position, bool autowall, float* damage)
	{
		Vector3 vViewOrigin;

		GetPlayerViewOrigin(vViewOrigin);

		if (_profiler.gApplyPrediction->Custom.bValue)
			ApplyPrediction(entity, position);

		if (WeaponIsVehicle())
		{
			bool bTraceHit = _autoWall.TraceBullet(CG->RefDef.vViewOrg, position, entity->NextEntityState.iEntityNum);

			if (bTraceHit)
				return true;
		}

		else if (autowall)
		{
			float flDamage = _autoWall.Autowall(vViewOrigin, position);

			if (damage)
				*damage = flDamage;

			if (flDamage >= 1.0f)
				return true;
		}

		else
		{
			bool bTraceHit = _autoWall.TraceBullet(vViewOrigin, position, entity->NextEntityState.iEntityNum);

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

		if (entity->CurrentEntityState.PositionTrajectory.iType && entity->CurrentEntityState.PositionTrajectory.iType != 1 && entity->CurrentEntityState.PositionTrajectory.iType != 14 && entity->CurrentEntityState.PositionTrajectory.iType != 10)
			EvaluateTrajectory(&entity->CurrentEntityState.PositionTrajectory, CG->OldSnapShot->iServerTime, vOldPosition);
		else
			VectorCopy(entity->CurrentEntityState.PositionTrajectory.vBase, vOldPosition);

		if (entity->NextEntityState.LerpEntityState.PositionTrajectory.iType && entity->NextEntityState.LerpEntityState.PositionTrajectory.iType != 1 && entity->NextEntityState.LerpEntityState.PositionTrajectory.iType != 14 && entity->NextEntityState.LerpEntityState.PositionTrajectory.iType != 10)
			EvaluateTrajectory(&entity->NextEntityState.LerpEntityState.PositionTrajectory, CG->NewSnapShot->iServerTime, vNewPosition);
		else
			VectorCopy(entity->NextEntityState.LerpEntityState.PositionTrajectory.vBase, vNewPosition);

		VectorSubtract(vNewPosition, vOldPosition, vVelocity);

		VectorMA(position, *(int*)(dwCG + 0x48088) / 1000.0f, vVelocity, position);
		VectorMA(position, *(int*)(*(DWORD_PTR*)dwClientActive + 0x68) / 1000.0f, vVelocity, position);
	}
}

//=====================================================================================