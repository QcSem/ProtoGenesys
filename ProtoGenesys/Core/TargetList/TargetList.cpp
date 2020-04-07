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

			if (!IsValid(i))
				continue;

			if (CG->Entity[i].NextEntityState.wEntityType == ET_PLAYER)
			{
				LPVOID pDObj = GetDObj(&CG->Entity[i]);

				if (!pDObj)
					continue;

				Vector3 vMinTemp = { FLT_MAX, FLT_MAX, FLT_MAX }, vMaxTemp = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

				for (auto& Bone : vBones)
				{
					GetTagPosition(&CG->Entity[i], RegisterTag(szBones[Bone.first].second), pDObj, EntityList[i].vBones3D[Bone.first]);

					for (int j = 0; j < 3; j++)
					{
						if (EntityList[i].vBones3D[Bone.first][j] < vMinTemp[j])
							vMinTemp[j] = EntityList[i].vBones3D[Bone.first][j];

						if (EntityList[i].vBones3D[Bone.first][j] > vMaxTemp[j])
							vMaxTemp[j] = EntityList[i].vBones3D[Bone.first][j];
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
					WorldToScreen(EntityList[i].vCenter3D, EntityList[i].vCenter2D) &&
					WorldToScreen(CG->Entity[i].vOrigin, EntityList[i].vLower) &&
					WorldToScreen(vViewOrigin, EntityList[i].vUpper);

				_mathematics.WorldToCompass(CG->Entity[i].vOrigin, _drawing.Compass.vCompassPosition, _drawing.Compass.flCompassSize, _drawing.Compass.vArrowPosition[i]);
				_mathematics.WorldToRadar(CG->Entity[i].vOrigin, _drawing.Radar.vRadarPosition, _drawing.Radar.flScale, _drawing.Radar.flRadarSize, _drawing.Radar.flBlipSize, _drawing.Radar.vBlipPosition[i]);

				if (!IsEnemy(i))
				{
					EntityList[i].cColor = _profiler.gColorAllies->Current.cValue;
					continue;
				}

				EntityList[i].cColor = _profiler.gColorAxis->Current.cValue;
			}

			else
			{
				EntityList[i].bW2SSuccess = WorldToScreen(CG->Entity[i].vOrigin, EntityList[i].vCenter2D);
				continue;
			}

			if (_profiler.gBoneScan->Current.iValue == cProfiler::BONESCAN_ONTIMER)
			{
				EntityList[i].bIsVisible = IsVisible(&CG->Entity[i], EntityList[i].vBones3D, iBonescanNum == i, _profiler.gAutoWall->Current.bValue, &EntityList[i].iBoneIndex);
				VectorCopy(EntityList[i].vBones3D[EntityList[i].iBoneIndex], EntityList[i].vHitLocation);
			}

			else if (_profiler.gBoneScan->Current.iValue == cProfiler::BONESCAN_IMMEDIATE)
			{
				EntityList[i].bIsVisible = IsVisible(&CG->Entity[i], EntityList[i].vBones3D, true, _profiler.gAutoWall->Current.bValue, &EntityList[i].iBoneIndex);
				VectorCopy(EntityList[i].vBones3D[EntityList[i].iBoneIndex], EntityList[i].vHitLocation);
			}

			else
			{
				EntityList[i].iBoneIndex = (eBone)_profiler.gAimBone->Current.iValue;
				EntityList[i].bIsVisible = IsVisible(&CG->Entity[i], EntityList[i].vBones3D, false, _profiler.gAutoWall->Current.bValue, &EntityList[i].iBoneIndex);
				VectorCopy(EntityList[i].vBones3D[EntityList[i].iBoneIndex], EntityList[i].vHitLocation);
			}

			if (std::find(vIsTarget.begin(), vIsTarget.end(), TRUE) != vIsTarget.end())
				if (!vIsTarget[i])
					continue;

			if (EntityList[i].bIsVisible && _mathematics.CalculateFOV(EntityList[i].vHitLocation) <= _profiler.gAimAngle->Current.flValue)
			{
				TargetInfo.iIndex = i;

				TargetInfo.flFOV = _mathematics.CalculateFOV(EntityList[i].vHitLocation);
				TargetInfo.flDistance = _mathematics.CalculateDistance(CG->Entity[i].vOrigin, CG->vOrigin);

				vTargetInfo.push_back(TargetInfo);
			}
		}

		if (!vTargetInfo.empty())
		{
			if (_profiler.gSortMethod->Current.iValue == cProfiler::SORT_METHOD_FOV)
			{
				std::sort(vTargetInfo.begin(), vTargetInfo.end(), [&](const sTargetInfo& a, const sTargetInfo& b) { return a.flFOV < b.flFOV; });
				_aimBot.AimState.iTargetNum = vTargetInfo.front().iIndex;
			}

			else if (_profiler.gSortMethod->Current.iValue == cProfiler::SORT_METHOD_DISTANCE)
			{
				std::sort(vTargetInfo.begin(), vTargetInfo.end(), [&](const sTargetInfo& a, const sTargetInfo& b) { return a.flDistance < b.flDistance; });
				_aimBot.AimState.iTargetNum = vTargetInfo.front().iIndex;
			}
		}

		_aimBot.AimState.bTargetAcquired = (_aimBot.AimState.iTargetNum > -1);
		_aimBot.AimState.bLockonTarget = (_profiler.gAimBotMode->Current.iValue == cProfiler::AIMBOT_MODE_AUTO || (_profiler.gAimBotMode->Current.iValue == cProfiler::AIMBOT_MODE_MANUAL && CG->Entity[CG->iClientNum].NextEntityState.LerpEntityState.eFlags1 & EF1_ZOOM));
		_aimBot.AimState.bIsAutoAiming = (_aimBot.AimState.bTargetAcquired && _aimBot.AimState.bLockonTarget);
		_aimBot.AimState.bIsAutoFiring = (_profiler.gAutoFire->Current.bValue && _aimBot.AimState.bIsAutoAiming);

		if (_aimBot.AimState.bLockonTarget)
		{
			if (_aimBot.AimState.iCurrentAimDelay == _profiler.gAutoAimDelay->Current.iValue)
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

		if (_aimBot.AimState.iCurrentAimTime > _profiler.gAutoAimTime->Current.iValue)
			_aimBot.AimState.iCurrentAimTime = _profiler.gAutoAimTime->Current.iValue;

		if (_aimBot.AimState.iCurrentAimDelay > _profiler.gAutoAimDelay->Current.iValue)
			_aimBot.AimState.iCurrentAimDelay = _profiler.gAutoAimDelay->Current.iValue;

		if (_aimBot.AimState.iCurrentZoomDelay > _profiler.gAutoZoomDelay->Current.iValue)
			_aimBot.AimState.iCurrentZoomDelay = _profiler.gAutoZoomDelay->Current.iValue;

		if (_aimBot.AimState.iCurrentFireDelay > _profiler.gAutoFireDelay->Current.iValue)
			_aimBot.AimState.iCurrentFireDelay = _profiler.gAutoFireDelay->Current.iValue;

		if (_aimBot.AimState.bTargetAcquired)
		{
			Vector3 vViewOrigin;
			GetPlayerViewOrigin(vViewOrigin);

			VectorCopy(EntityList[_aimBot.AimState.iTargetNum].vHitLocation, _aimBot.AimState.vAimPosition);

			_mathematics.CalculateAimAngles(_aimBot.AimState.vAimPosition, vViewOrigin, _aimBot.AimState.vAimAngles);
			_mathematics.CalculateAntiAimAngles(_aimBot.AimState.vAimPosition, vViewOrigin, _aimBot.AimState.vAntiAimAngles);
		}

		iCounter++;
	}
	/*
	//=====================================================================================
	*/
	bool cTargetList::IsLocalPlayerValid()
	{
		return ((CG->Entity[CG->iClientNum].iAlive & 2) && !(CG->Entity[CG->iClientNum].NextEntityState.LerpEntityState.eFlags1 & EF1_DEAD));
	}
	/*
	//=====================================================================================
	*/
	bool cTargetList::IsValid(int index)
	{
		if (CG->Entity[index].NextEntityState.wEntityType == ET_PLAYER)
		{
			if (CG->Client[index].iInfoValid && (CG->Entity[index].iAlive & 2) && (&CG->Entity[index] != &CG->Entity[CG->iClientNum]) && !(CG->Entity[index].NextEntityState.LerpEntityState.eFlags1 & EF1_DEAD))
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
	bool cTargetList::IsEnemy(int index)
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
	bool cTargetList::IsVisibleInternal(sEntity* entity, Vector3 position, short hitloc, bool autowall, float* damage)
	{
		Vector3 vViewOrigin;

		GetPlayerViewOrigin(vViewOrigin);

		if (_profiler.gApplyPrediction->Current.bValue)
			ApplyPrediction(entity, position);

		if (!WeaponIsVehicle() && autowall)
		{
			*damage = _autoWall.Autowall(vViewOrigin, position, hitloc);

			if (*damage >= 1.0f)
				return true;
		}

		else
		{
			*damage = _autoWall.TraceBullet(vViewOrigin, position, hitloc, entity->NextEntityState.iEntityNum);

			if (*damage >= 1.0f)
				return true;
		}

		return false;
	}
	/*
	//=====================================================================================
	*/
	bool cTargetList::IsVisible(sEntity* entity, Vector3 bones3d[BONE_MAX], bool bonescan, bool autowall, eBone* index)
	{
		bool bReturn = false;

		sDamageInfo DamageInfo;
		std::vector<sDamageInfo> vDamageInfo;
		std::vector<std::future<bool>> vIsVisible(BONE_MAX);

		if (bonescan)
		{
			for (auto& Bone : vBones)
			{
				vIsVisible[Bone.first] = std::async(&cTargetList::IsVisibleInternal, this, entity, bones3d[Bone.first], vBones[Bone.first].second, autowall, &DamageInfo.flDamage);
			}

			for (auto& Bone : vBones)
			{
				vIsVisible[Bone.first].wait();

				if (vIsVisible[Bone.first].get())
				{
					DamageInfo.iBoneIndex = Bone.first;
					vDamageInfo.push_back(DamageInfo);

					bReturn = true;
				}
			}
		}

		else
		{
			return std::async(&cTargetList::IsVisibleInternal, this, entity, bones3d[*index], vBones[*index].second, autowall, &DamageInfo.flDamage).get();
		}

		if (!vDamageInfo.empty())
		{
			std::stable_sort(vDamageInfo.begin(), vDamageInfo.end(), [&](const sDamageInfo& a, const sDamageInfo& b) { return a.flDamage > b.flDamage; });
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

		ClientActive = *(sClientActive**)dwClientActive;

		VectorMA(position, CG->iFrameTime / 1000.0f, vVelocity, position);
		VectorMA(position, ClientActive->iPing / 1000.0f, vVelocity, position);
	}
}

//=====================================================================================