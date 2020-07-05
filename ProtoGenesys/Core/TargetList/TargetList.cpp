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

			if (i < MAX_CLIENTS)
			{
				if (_targetList.Priorities[i].bIsPrioritized && !_targetList.Priorities[i].bIsIgnored)
				{
					AntiAimTargetInfo.iIndex = i;

					AntiAimTargetInfo.flDistance = _mathematics.CalculateDistance(CG->CEntity[i].vOrigin, CG->vOrigin);
					AntiAimTargetInfo.flDamage = EntityList[i].flDamage;
					AntiAimTargetInfo.flFOV = _mathematics.CalculateFOV(EntityList[i].vHitLocation);

					vAntiAimTargetInfo.push_back(AntiAimTargetInfo);
				}
			}

			if (CG->CEntity[i].NextEntityState.wEntityType == ET_PLAYER || CG->CEntity[i].NextEntityState.wEntityType == ET_ACTOR)
			{
				ImVec3 vMinTemp = { FLT_MAX, FLT_MAX, FLT_MAX }, vMaxTemp = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

				for (auto& Bone : vBones)
				{
					GetTagPosition(&CG->CEntity[i], RegisterTag(vBones[Bone.first].second.second), GetDObj(&CG->CEntity[i]), &EntityList[i].vBones3D[Bone.first]);

					if (EntityList[i].vBones3D[Bone.first].x < vMinTemp.x)
						vMinTemp.x = EntityList[i].vBones3D[Bone.first].x;

					if (EntityList[i].vBones3D[Bone.first].x > vMaxTemp.x)
						vMaxTemp.x = EntityList[i].vBones3D[Bone.first].x;

					if (EntityList[i].vBones3D[Bone.first].y < vMinTemp.y)
						vMinTemp.y = EntityList[i].vBones3D[Bone.first].y;

					if (EntityList[i].vBones3D[Bone.first].y > vMaxTemp.y)
						vMaxTemp.y = EntityList[i].vBones3D[Bone.first].y;

					if (EntityList[i].vBones3D[Bone.first].z < vMinTemp.z)
						vMinTemp.z = EntityList[i].vBones3D[Bone.first].z;

					if (EntityList[i].vBones3D[Bone.first].z > vMaxTemp.z)
						vMaxTemp.z = EntityList[i].vBones3D[Bone.first].z;
				}

				EntityList[i].vCenter3D = (vMinTemp + vMaxTemp) / 2.0f;
			}

			if (WeaponNames[(BYTE)CG->CEntity[i].NextEntityState.iWeaponID].szDisplayName)
				EntityList[i].szWeapon = WeaponNames[(BYTE)CG->CEntity[i].NextEntityState.iWeaponID].szDisplayName;

			EntityList[i].bIsValid = true;

			if (CG->CEntity[i].NextEntityState.wEntityType == ET_PLAYER)
			{
				ImVec3 vViewOrigin;
				vViewOrigin = CG->CEntity[i].vOrigin;
				vViewOrigin.z += M_METERS;

				EntityList[i].bW2SSuccess = _drawing.Calculate2D(EntityList[i].vBones3D, EntityList[i].vBones2D, EntityList[i].vPosition, EntityList[i].vDimentions) &&
					_drawing.Calculate3D(&CG->CEntity[i], EntityList[i].vCenter3D, EntityList[i].vCorners3D, EntityList[i].vCorners2D) &&
					_mathematics.WorldToScreen(EntityList[i].vCenter3D, EntityList[i].vCenter2D) &&
					_mathematics.WorldToScreen(CG->CEntity[i].vOrigin, EntityList[i].vLower) &&
					_mathematics.WorldToScreen(vViewOrigin, EntityList[i].vUpper);

				_mathematics.WorldToCompass(CG->CEntity[i].vOrigin, _drawing.Compass.vCompassPosition, _drawing.Compass.flCompassSize, _drawing.Compass.vArrowPosition[i]);
				_mathematics.WorldToRadar(CG->CEntity[i].vOrigin, _drawing.Radar.vRadarPosition, _drawing.Radar.flScale, _drawing.Radar.flRadarSize, _drawing.Radar.flBlipSize, _drawing.Radar.vBlipPosition[i]);

				if (EntityIsTeammate(&CG->CEntity[i]))
				{
					EntityList[i].cColor = IsVisible(&CG->CEntity[i], EntityList[i].vBones3D, false, false, vBones[BONE_HEAD].first, NULL) ? _profiler.gColorAlliesVisible->Current.cValue : _profiler.gColorAlliesInvisible->Current.cValue;
					continue;
				}

				EntityList[i].cColor = IsVisible(&CG->CEntity[i], EntityList[i].vBones3D, false, false, vBones[BONE_HEAD].first, NULL) ? _profiler.gColorAxisVisible->Current.cValue : _profiler.gColorAxisInvisible->Current.cValue;
			}

			else if (CG->CEntity[i].NextEntityState.wEntityType == ET_ACTOR)
			{
				EntityList[i].bW2SSuccess = _mathematics.WorldToScreen(EntityList[i].vBones3D[vBones[BONE_HEAD].first], EntityList[i].vCenter2D);

				if (EntityIsTeammate(&CG->CEntity[i]))
					continue;
			}

			else
			{
				EntityList[i].bW2SSuccess = _mathematics.WorldToScreen(CG->CEntity[i].vOrigin, EntityList[i].vCenter2D);
				continue;
			}

			if (CG->CEntity[i].NextEntityState.wEntityType == ET_PLAYER)
			{
				if (_profiler.gBoneScan->Current.iValue == cProfiler::BONESCAN_ONTIMER)
				{
					if ((_profiler.gBoneScanPriorities->Current.iValue && _targetList.Priorities[i].bIsPrioritized) ||
						(_profiler.gBoneScanRiotShielders->Current.iValue && EntityHasRiotShield(&CG->CEntity[i])))
					{
						EntityList[i].bIsVisible = IsVisible(&CG->CEntity[i], EntityList[i].vBones3D, iBonescanNum == i, _profiler.gAutoWall->Current.iValue, EntityList[i].iBoneIndex, &EntityList[i].flDamage);
					}

					else if (!_profiler.gBoneScanPriorities->Current.iValue && !_profiler.gBoneScanRiotShielders->Current.iValue)
					{
						EntityList[i].bIsVisible = IsVisible(&CG->CEntity[i], EntityList[i].vBones3D, iBonescanNum == i, _profiler.gAutoWall->Current.iValue, EntityList[i].iBoneIndex, &EntityList[i].flDamage);
					}

					else
					{
						EntityList[i].iBoneIndex = (eBone)_profiler.gAimBone->Current.iValue;
						EntityList[i].bIsVisible = IsVisible(&CG->CEntity[i], EntityList[i].vBones3D, false, _profiler.gAutoWall->Current.iValue, EntityList[i].iBoneIndex, &EntityList[i].flDamage);
					}
				}

				else if (_profiler.gBoneScan->Current.iValue == cProfiler::BONESCAN_IMMEDIATE)
				{
					if ((_profiler.gBoneScanPriorities->Current.iValue && _targetList.Priorities[i].bIsPrioritized) ||
						(_profiler.gBoneScanRiotShielders->Current.iValue && EntityHasRiotShield(&CG->CEntity[i])))
					{
						EntityList[i].bIsVisible = IsVisible(&CG->CEntity[i], EntityList[i].vBones3D, true, _profiler.gAutoWall->Current.iValue, EntityList[i].iBoneIndex, &EntityList[i].flDamage);
					}

					else if (!_profiler.gBoneScanPriorities->Current.iValue && !_profiler.gBoneScanRiotShielders->Current.iValue)
					{
						EntityList[i].bIsVisible = IsVisible(&CG->CEntity[i], EntityList[i].vBones3D, true, _profiler.gAutoWall->Current.iValue, EntityList[i].iBoneIndex, &EntityList[i].flDamage);
					}

					else
					{
						EntityList[i].iBoneIndex = (eBone)_profiler.gAimBone->Current.iValue;
						EntityList[i].bIsVisible = IsVisible(&CG->CEntity[i], EntityList[i].vBones3D, false, _profiler.gAutoWall->Current.iValue, EntityList[i].iBoneIndex, &EntityList[i].flDamage);
					}
				}

				else
				{
					EntityList[i].iBoneIndex = (eBone)_profiler.gAimBone->Current.iValue;
					EntityList[i].bIsVisible = IsVisible(&CG->CEntity[i], EntityList[i].vBones3D, false, _profiler.gAutoWall->Current.iValue, EntityList[i].iBoneIndex, &EntityList[i].flDamage);
				}
			}

			else if (CG->CEntity[i].NextEntityState.wEntityType == ET_ACTOR)
			{
				EntityList[i].iBoneIndex = vBones[BONE_HEAD].first;
				EntityList[i].bIsVisible = IsVisible(&CG->CEntity[i], EntityList[i].vBones3D, false, _profiler.gAutoWall->Current.iValue, EntityList[i].iBoneIndex, &EntityList[i].flDamage);
			}

			EntityList[i].vHitLocation = EntityList[i].vBones3D[EntityList[i].iBoneIndex];

			if (i < MAX_CLIENTS)
				if (Priorities[i].bIsIgnored)
					continue;

			if (EntityList[i].bIsVisible && _mathematics.CalculateFOV(EntityList[i].vHitLocation) <= _profiler.gAimAngle->Current.iValue)
			{
				TargetInfo.bIsPriority = _targetList.Priorities[i].bIsPrioritized;
				TargetInfo.iIndex = i;

				TargetInfo.flDistance = _mathematics.CalculateDistance(CG->CEntity[i].vOrigin, CG->vOrigin);
				TargetInfo.flDamage = EntityList[i].flDamage;
				TargetInfo.flFOV = _mathematics.CalculateFOV(EntityList[i].vHitLocation);

				vTargetInfo.push_back(TargetInfo);
			}
		}

		if (!vTargetInfo.empty())
		{
			if (_profiler.gSortMethod->Current.iValue == cProfiler::SORT_METHOD_DISTANCE)
			{
				std::sort(vTargetInfo.begin(), vTargetInfo.end(), [&](const sTargetInfo& a, const sTargetInfo& b) { return a.flDistance < b.flDistance; });

				auto itTargetInfo = std::find_if(vTargetInfo.begin(), vTargetInfo.end(), [&](const sTargetInfo& targetinfo) { return targetinfo.bIsPriority; });

				if (itTargetInfo != vTargetInfo.end())
					_aimBot.AimState.iTargetNum = itTargetInfo->iIndex;

				else
					_aimBot.AimState.iTargetNum = vTargetInfo.front().iIndex;
			}

			else if (_profiler.gSortMethod->Current.iValue == cProfiler::SORT_METHOD_DAMAGE)
			{
				std::sort(vTargetInfo.begin(), vTargetInfo.end(), [&](const sTargetInfo& a, const sTargetInfo& b) { return a.flDamage > b.flDamage; });

				auto itTargetInfo = std::find_if(vTargetInfo.begin(), vTargetInfo.end(), [&](const sTargetInfo& targetinfo) { return targetinfo.bIsPriority; });

				if (itTargetInfo != vTargetInfo.end())
					_aimBot.AimState.iTargetNum = itTargetInfo->iIndex;

				else
					_aimBot.AimState.iTargetNum = vTargetInfo.front().iIndex;
			}
			
			else if (_profiler.gSortMethod->Current.iValue == cProfiler::SORT_METHOD_FOV)
			{
				std::sort(vTargetInfo.begin(), vTargetInfo.end(), [&](const sTargetInfo& a, const sTargetInfo& b) { return a.flFOV < b.flFOV; });

				auto itTargetInfo = std::find_if(vTargetInfo.begin(), vTargetInfo.end(), [&](const sTargetInfo& targetinfo) { return targetinfo.bIsPriority; });

				if (itTargetInfo != vTargetInfo.end())
					_aimBot.AimState.iTargetNum = itTargetInfo->iIndex;

				else
					_aimBot.AimState.iTargetNum = vTargetInfo.front().iIndex;
			}

			vTargetInfo.clear();
		}

		if (!vAntiAimTargetInfo.empty())
		{
			if (_profiler.gSortMethod->Current.iValue == cProfiler::SORT_METHOD_DISTANCE)
			{
				std::sort(vAntiAimTargetInfo.begin(), vAntiAimTargetInfo.end(), [&](const sAntiAimTargetInfo& a, const sAntiAimTargetInfo& b) { return a.flDistance < b.flDistance; });

				_aimBot.AimState.iAntiAimTargetNum = vAntiAimTargetInfo.front().iIndex;
			}

			else if (_profiler.gSortMethod->Current.iValue == cProfiler::SORT_METHOD_DAMAGE)
			{
				std::sort(vAntiAimTargetInfo.begin(), vAntiAimTargetInfo.end(), [&](const sAntiAimTargetInfo& a, const sAntiAimTargetInfo& b) { return a.flDamage > b.flDamage; });

				_aimBot.AimState.iAntiAimTargetNum = vAntiAimTargetInfo.front().iIndex;
			}

			else if (_profiler.gSortMethod->Current.iValue == cProfiler::SORT_METHOD_FOV)
			{
				std::sort(vAntiAimTargetInfo.begin(), vAntiAimTargetInfo.end(), [&](const sAntiAimTargetInfo& a, const sAntiAimTargetInfo& b) { return a.flFOV < b.flFOV; });

				_aimBot.AimState.iAntiAimTargetNum = vAntiAimTargetInfo.front().iIndex;
			}

			vAntiAimTargetInfo.clear();
		}

		iCounter++;
	}
	/*
	//=====================================================================================
	*/
	bool cTargetList::IsVisibleInternal(sCEntity* entity, ImVec3 position, bool autowall, float* damage)
	{
		ImVec3 vViewOrigin;
		GetPlayerViewOrigin(&vViewOrigin);

		if (WeaponIsVehicle())
		{
			float flDamage = _autoWall.TraceBullet(entity, vViewOrigin, position);

			if (damage)
				*damage = flDamage;

			if (flDamage >= 1.0f)
				return true;
		}

		else if (autowall)
		{
			float flDamage = _autoWall.Autowall(entity, vViewOrigin, position);

			if (damage)
				*damage = flDamage;

			if (flDamage >= 1.0f)
				return true;
		}

		else
		{
			float flDamage = _autoWall.TraceBullet(entity, vViewOrigin, position);

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
	bool cTargetList::IsVisible(sCEntity* entity, ImVec3 bones3d[BONE_MAX], bool bonescan, bool autowall, eBone& index, float* damage)
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
					vIsVisible[Bone.first] = std::async(&cTargetList::IsVisibleInternal, this, entity, bones3d[Bone.first], autowall, &DamageInfo.flDamage);
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
				return std::async(&cTargetList::IsVisibleInternal, this, entity, bones3d[index], autowall, damage).get();
			}
		}

		else
		{
			if (bonescan)
			{
				for (auto& Bone : vBones)
				{
					if (IsVisibleInternal(entity, bones3d[Bone.first], autowall, &DamageInfo.flDamage))
					{
						DamageInfo.iBoneIndex = Bone.first;
						vDamageInfo.push_back(DamageInfo);

						bReturn = true;
					}
				}
			}

			else
			{
				return IsVisibleInternal(entity, bones3d[index], autowall, damage);
			}
		}

		if (!vDamageInfo.empty())
		{
			std::stable_sort(vDamageInfo.begin(), vDamageInfo.end(), [&](const sDamageInfo& a, const sDamageInfo& b) { return a.flDamage > b.flDamage; });

			index = vDamageInfo.front().iBoneIndex;

			if (damage)
				*damage = vDamageInfo.front().flDamage;

			vDamageInfo.clear();
		}

		return bReturn;
	}
}

//=====================================================================================