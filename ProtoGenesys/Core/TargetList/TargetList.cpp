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

		sAntiAimTargetInfo AntiAimTargetInfo;
		std::vector<sAntiAimTargetInfo> vAntiAimTargetInfo;

		_aimBot.AimState.iTargetNum = -1;
		_aimBot.AimState.iAntiAimTargetNum = -1;

		static int iCounter = 0;
		int iBonescanNum = iCounter % MAX_CLIENTS;

		for (int i = 0; i < MAX_ENTITIES; i++)
		{
			EntityList[i].bIsValid = false;

			if (!EntityIsValid(&CG->CEntity[i]))
				continue;

			if (CG->CEntity[i].NextEntityState.wEntityType == ET_PLAYER)
			{
				if (bIsPriority[i] && _mathematics.CalculateFOV(EntityList[i].vHitLocation) <= _profiler.gAimAngle->Current.iValue)
				{
					AntiAimTargetInfo.iIndex = i;

					AntiAimTargetInfo.flFOV = _mathematics.CalculateFOV(EntityList[i].vHitLocation);
					AntiAimTargetInfo.flDistance = _mathematics.CalculateDistance(CG->CEntity[i].vOrigin, CG->vOrigin);

					vAntiAimTargetInfo.push_back(AntiAimTargetInfo);
				}

				Vector3 vMinTemp = { FLT_MAX, FLT_MAX, FLT_MAX }, vMaxTemp = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

				for (auto& Bone : vBones)
				{
					GetTagPosition(&CG->CEntity[i], RegisterTag(szBones[Bone.first].second), GetDObj(&CG->CEntity[i]), EntityList[i].vBones3D[Bone.first]);

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

			if (WeaponNames[(BYTE)CG->CEntity[i].NextEntityState.iWeaponID].szDisplayName)
				EntityList[i].szWeapon = WeaponNames[(BYTE)CG->CEntity[i].NextEntityState.iWeaponID].szDisplayName;

			EntityList[i].bIsValid = true;

			if (CG->CEntity[i].NextEntityState.wEntityType == ET_PLAYER)
			{
				Vector3 vViewOrigin;
				VectorCopy(CG->CEntity[i].vOrigin, vViewOrigin);
				vViewOrigin[2] += M_METERS;

				EntityList[i].bW2SSuccess = _drawing.Calculate2D(EntityList[i].vBones3D, EntityList[i].vBones2D, EntityList[i].vPosition, EntityList[i].vDimentions) &&
					_drawing.Calculate3D(&CG->CEntity[i], EntityList[i].vCenter3D, EntityList[i].vCorners3D, EntityList[i].vCorners2D) &&
					WorldToScreen(EntityList[i].vCenter3D, EntityList[i].vCenter2D) &&
					WorldToScreen(CG->CEntity[i].vOrigin, EntityList[i].vLower) &&
					WorldToScreen(vViewOrigin, EntityList[i].vUpper);

				_mathematics.WorldToCompass(CG->CEntity[i].vOrigin, _drawing.Compass.vCompassPosition, _drawing.Compass.flCompassSize, _drawing.Compass.vArrowPosition[i]);
				_mathematics.WorldToRadar(CG->CEntity[i].vOrigin, _drawing.Radar.vRadarPosition, _drawing.Radar.flScale, _drawing.Radar.flRadarSize, _drawing.Radar.flBlipSize, _drawing.Radar.vBlipPosition[i]);

				if (EntityIsTeammate(&CG->CEntity[i]))
				{
					EntityList[i].cColor = IsVisible(&CG->CEntity[i], EntityList[i].vBones3D, false, false, vBones[BONE_HEAD].first) ? _profiler.gColorAlliesVisible->Current.cValue : _profiler.gColorAlliesInvisible->Current.cValue;
					continue;
				}

				EntityList[i].cColor = IsVisible(&CG->CEntity[i], EntityList[i].vBones3D, false, false, vBones[BONE_HEAD].first) ? _profiler.gColorAxisVisible->Current.cValue : _profiler.gColorAxisInvisible->Current.cValue;
			}

			else
			{
				EntityList[i].bW2SSuccess = WorldToScreen(CG->CEntity[i].vOrigin, EntityList[i].vCenter2D);
				continue;
			}

			if (_profiler.gBoneScan->Current.iValue == cProfiler::BONESCAN_ONTIMER)
			{
				if (_profiler.gBoneScanPriorities->Current.bValue && !bIsPriority[i])
				{
					EntityList[i].iBoneIndex = (eBone)_profiler.gAimBone->Current.iValue;
					EntityList[i].bIsVisible = IsVisible(&CG->CEntity[i], EntityList[i].vBones3D, false, _profiler.gAutoWall->Current.bValue, EntityList[i].iBoneIndex);
				}

				else
				{
					EntityList[i].bIsVisible = IsVisible(&CG->CEntity[i], EntityList[i].vBones3D, iBonescanNum == i, _profiler.gAutoWall->Current.bValue, EntityList[i].iBoneIndex);
				}
			}

			else if (_profiler.gBoneScan->Current.iValue == cProfiler::BONESCAN_IMMEDIATE)
			{
				if (_profiler.gBoneScanPriorities->Current.bValue && !bIsPriority[i])
				{
					EntityList[i].iBoneIndex = (eBone)_profiler.gAimBone->Current.iValue;
					EntityList[i].bIsVisible = IsVisible(&CG->CEntity[i], EntityList[i].vBones3D, false, _profiler.gAutoWall->Current.bValue, EntityList[i].iBoneIndex);
				}

				else
				{
					EntityList[i].bIsVisible = IsVisible(&CG->CEntity[i], EntityList[i].vBones3D, true, _profiler.gAutoWall->Current.bValue, EntityList[i].iBoneIndex);
				}
			}

			else
			{
				EntityList[i].iBoneIndex = (eBone)_profiler.gAimBone->Current.iValue;
				EntityList[i].bIsVisible = IsVisible(&CG->CEntity[i], EntityList[i].vBones3D, false, _profiler.gAutoWall->Current.bValue, EntityList[i].iBoneIndex);
			}

			VectorCopy(EntityList[i].vBones3D[EntityList[i].iBoneIndex], EntityList[i].vHitLocation);

			if (EntityList[i].bIsVisible && _mathematics.CalculateFOV(EntityList[i].vHitLocation) <= _profiler.gAimAngle->Current.iValue)
			{
				TargetInfo.bIsPriority = bIsPriority[i];
				TargetInfo.iIndex = i;

				TargetInfo.flFOV = _mathematics.CalculateFOV(EntityList[i].vHitLocation);
				TargetInfo.flDistance = _mathematics.CalculateDistance(CG->CEntity[i].vOrigin, CG->vOrigin);

				vTargetInfo.push_back(TargetInfo);
			}
		}

		if (!vTargetInfo.empty())
		{
			if (_profiler.gSortMethod->Current.iValue == cProfiler::SORT_METHOD_FOV)
			{
				std::sort(vTargetInfo.begin(), vTargetInfo.end(), [&](const sTargetInfo& a, const sTargetInfo& b) { return a.flFOV < b.flFOV; });

				auto ItTargetInfo = std::find_if(vTargetInfo.begin(), vTargetInfo.end(), [&](const sTargetInfo& targetinfo) { return targetinfo.bIsPriority; });

				if (ItTargetInfo != vTargetInfo.end())
					_aimBot.AimState.iTargetNum = ItTargetInfo->iIndex;

				else
					_aimBot.AimState.iTargetNum = vTargetInfo.front().iIndex;
			}

			else if (_profiler.gSortMethod->Current.iValue == cProfiler::SORT_METHOD_DISTANCE)
			{
				std::sort(vTargetInfo.begin(), vTargetInfo.end(), [&](const sTargetInfo& a, const sTargetInfo& b) { return a.flDistance < b.flDistance; });

				auto ItTargetInfo = std::find_if(vTargetInfo.begin(), vTargetInfo.end(), [&](const sTargetInfo& targetinfo) { return targetinfo.bIsPriority; });

				if (ItTargetInfo != vTargetInfo.end())
					_aimBot.AimState.iTargetNum = ItTargetInfo->iIndex;

				else
					_aimBot.AimState.iTargetNum = vTargetInfo.front().iIndex;
			}

			vTargetInfo.clear();
		}

		if (!vAntiAimTargetInfo.empty())
		{
			if (_profiler.gSortMethod->Current.iValue == cProfiler::SORT_METHOD_FOV)
			{
				std::sort(vAntiAimTargetInfo.begin(), vAntiAimTargetInfo.end(), [&](const sAntiAimTargetInfo& a, const sAntiAimTargetInfo& b) { return a.flFOV < b.flFOV; });

				_aimBot.AimState.iAntiAimTargetNum = vAntiAimTargetInfo.front().iIndex;
			}

			else if (_profiler.gSortMethod->Current.iValue == cProfiler::SORT_METHOD_DISTANCE)
			{
				std::sort(vAntiAimTargetInfo.begin(), vAntiAimTargetInfo.end(), [&](const sAntiAimTargetInfo& a, const sAntiAimTargetInfo& b) { return a.flDistance < b.flDistance; });

				_aimBot.AimState.iAntiAimTargetNum = vAntiAimTargetInfo.front().iIndex;
			}

			vAntiAimTargetInfo.clear();
		}

		iCounter++;
	}
	/*
	//=====================================================================================
	*/
	bool cTargetList::IsVisibleInternal(sCEntity* entity, Vector3 position, short hitloc, bool autowall, float* damage)
	{
		Vector3 vViewOrigin;
		GetPlayerViewOrigin(vViewOrigin);

		if (WeaponIsVehicle())
		{
			float flDamage = _autoWall.TraceBullet(entity, vViewOrigin, position, hitloc);

			if (damage)
				*damage = flDamage;

			if (flDamage >= 1.0f)
				return true;
		}

		else if (autowall)
		{
			float flDamage = _autoWall.Autowall(entity, vViewOrigin, position, hitloc);

			if (damage)
				*damage = flDamage;

			if (flDamage >= 1.0f)
				return true;
		}

		else
		{
			float flDamage = _autoWall.TraceBullet(entity, vViewOrigin, position, hitloc);

			if (damage)
				*damage = flDamage;

			if (flDamage >= 1.0f)
				return true;
		}

		return false;
	}
	/*
	//=====================================================================================
	*/
	bool cTargetList::IsVisible(sCEntity* entity, Vector3 bones3d[BONE_MAX], bool bonescan, bool autowall, eBone& index)
	{
		bool bReturn = false;

		sDamageInfo DamageInfo;
		std::vector<sDamageInfo> vDamageInfo;
		std::vector<std::future<bool>> vIsVisible(BONE_MAX);

		if (bIsSteamVersion)
		{
			if (bonescan)
			{
				for (auto& Bone : vBones)
				{
					vIsVisible[Bone.first] = std::async(&cTargetList::IsVisibleInternal, this, entity, bones3d[Bone.first], Bone.second, autowall, &DamageInfo.flDamage);
				}

				for (auto& Bone : vBones)
				{
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
				return std::async(&cTargetList::IsVisibleInternal, this, entity, bones3d[index], vBones[index].second, autowall, nullptr).get();
			}
		}

		else
		{
			if (bonescan)
			{
				for (auto& Bone : vBones)
				{
					if (IsVisibleInternal(entity, bones3d[Bone.first], Bone.second, autowall, &DamageInfo.flDamage))
					{
						DamageInfo.iBoneIndex = Bone.first;
						vDamageInfo.push_back(DamageInfo);

						bReturn = true;
					}
				}
			}

			else
			{
				return IsVisibleInternal(entity, bones3d[index], vBones[index].second, autowall, NULL);
			}
		}

		if (!vDamageInfo.empty())
		{
			std::stable_sort(vDamageInfo.begin(), vDamageInfo.end(), [&](const sDamageInfo& a, const sDamageInfo& b) { return a.flDamage > b.flDamage; });
			index = vDamageInfo.front().iBoneIndex;
			vDamageInfo.clear();
		}

		return bReturn;
	}
}

//=====================================================================================