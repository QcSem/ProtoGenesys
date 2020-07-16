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

			EntityList[i].flDistance = FLT_MAX;
			EntityList[i].flDamage = -FLT_MAX;
			EntityList[i].flFOV = FLT_MAX;

			if (!EntityIsValid(&CG->CEntity[i]))
				continue;

			if (CG->CEntity[i].NextEntityState.wEntityType == ET_PLAYER || CG->CEntity[i].NextEntityState.wEntityType == ET_ACTOR)
			{
				LPVOID lpDObj = GetDObj(&CG->CEntity[i]);

				if (!lpDObj)
					continue;

				ImVec3 vMinTemp = { FLT_MAX, FLT_MAX, FLT_MAX }, vMaxTemp = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

				for (auto& Bone : vBones)
				{
					GetTagPosition(&CG->CEntity[i], RegisterTag(vBones[Bone.first].second.second), lpDObj, &EntityList[i].vBones3D[Bone.first]);

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

				if (CG->CEntity[i].NextEntityState.wEntityType == ET_PLAYER)
				{
					EntityList[i].vMultiPoints2D.clear();
					EntityList[i].vMultiPoints3D.clear();

					EntityList[i].vMultiPoints3D = CalculateMultiPoints(&CG->CEntity[i], EntityList[i].vBones3D, _profiler.gPointScale->Current.iValue, 2.0f);

					for (auto& vMultiPoint3D : EntityList[i].vMultiPoints3D)
					{
						ImVec2 vMultiPoint2D;

						_mathematics.WorldToScreen(vMultiPoint3D, vMultiPoint2D);
						EntityList[i].vMultiPoints2D.push_back(vMultiPoint2D);
					}

					EntityList[i].vCenter3D = (vMinTemp + vMaxTemp) / 2.0f;
				}
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
					EntityList[i].cColor = ScanPosition(&CG->CEntity[i], EntityList[i].vBones3D[vBones[BONE_HEAD].first], false, NULL) ? _profiler.gColorAlliesVisible->Current.cValue : _profiler.gColorAlliesInvisible->Current.cValue;
					continue;
				}

				EntityList[i].cColor = ScanPosition(&CG->CEntity[i], EntityList[i].vBones3D[vBones[BONE_HEAD].first], false, NULL) ? _profiler.gColorAxisVisible->Current.cValue : _profiler.gColorAxisInvisible->Current.cValue;
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
					if (iBonescanNum == i)
					{
						if ((_profiler.gBoneScanPriorities->Current.iValue && Priorities[i].bIsPrioritized) ||
							(_profiler.gBoneScanRiotShielders->Current.iValue && EntityHasRiotShield(&CG->CEntity[i])))
						{
							EntityList[i].flDistance = _mathematics.CalculateDistance(CG->CEntity[i].vOrigin, CG->vOrigin);
							EntityList[i].bIsVisible = ScanBones(&CG->CEntity[i], EntityList[i].vBones3D, EntityList[i].vHitLocation = EntityList[i].vBones3D[EntityList[i].iBoneIndex], EntityList[i].iBoneIndex, _profiler.gAutoWall->Current.iValue, &EntityList[i].flDamage);
							EntityList[i].flFOV = _mathematics.CalculateFOV(EntityList[i].vHitLocation);
						}

						else if (!_profiler.gBoneScanPriorities->Current.iValue && !_profiler.gBoneScanRiotShielders->Current.iValue)
						{
							EntityList[i].flDistance = _mathematics.CalculateDistance(CG->CEntity[i].vOrigin, CG->vOrigin);
							EntityList[i].bIsVisible = ScanBones(&CG->CEntity[i], EntityList[i].vBones3D, EntityList[i].vHitLocation = EntityList[i].vBones3D[EntityList[i].iBoneIndex], EntityList[i].iBoneIndex, _profiler.gAutoWall->Current.iValue, &EntityList[i].flDamage);
							EntityList[i].flFOV = _mathematics.CalculateFOV(EntityList[i].vHitLocation);
						}

						else
						{
							EntityList[i].flDistance = _mathematics.CalculateDistance(CG->CEntity[i].vOrigin, CG->vOrigin);
							EntityList[i].bIsVisible = ScanPosition(&CG->CEntity[i], EntityList[i].vHitLocation = EntityList[i].vBones3D[EntityList[i].iBoneIndex = (eBone)_profiler.gAimBone->Current.iValue], _profiler.gAutoWall->Current.iValue, &EntityList[i].flDamage);
							EntityList[i].flFOV = _mathematics.CalculateFOV(EntityList[i].vHitLocation);
						}
					}

					else
					{
						EntityList[i].flDistance = _mathematics.CalculateDistance(CG->CEntity[i].vOrigin, CG->vOrigin);
						EntityList[i].bIsVisible = ScanPosition(&CG->CEntity[i], EntityList[i].vHitLocation = EntityList[i].vBones3D[EntityList[i].iBoneIndex = (eBone)_profiler.gAimBone->Current.iValue], _profiler.gAutoWall->Current.iValue, &EntityList[i].flDamage);
						EntityList[i].flFOV = _mathematics.CalculateFOV(EntityList[i].vHitLocation);
					}
				}

				else if (_profiler.gBoneScan->Current.iValue == cProfiler::BONESCAN_IMMEDIATE)
				{
					if ((_profiler.gBoneScanPriorities->Current.iValue && Priorities[i].bIsPrioritized) ||
						(_profiler.gBoneScanRiotShielders->Current.iValue && EntityHasRiotShield(&CG->CEntity[i])))
					{
						EntityList[i].flDistance = _mathematics.CalculateDistance(CG->CEntity[i].vOrigin, CG->vOrigin);
						EntityList[i].bIsVisible = ScanBones(&CG->CEntity[i], EntityList[i].vBones3D, EntityList[i].vHitLocation = EntityList[i].vBones3D[EntityList[i].iBoneIndex], EntityList[i].iBoneIndex, _profiler.gAutoWall->Current.iValue, &EntityList[i].flDamage);
						EntityList[i].flFOV = _mathematics.CalculateFOV(EntityList[i].vHitLocation);
					}

					else if (!_profiler.gBoneScanPriorities->Current.iValue && !_profiler.gBoneScanRiotShielders->Current.iValue)
					{
						EntityList[i].flDistance = _mathematics.CalculateDistance(CG->CEntity[i].vOrigin, CG->vOrigin);
						EntityList[i].bIsVisible = ScanBones(&CG->CEntity[i], EntityList[i].vBones3D, EntityList[i].vHitLocation = EntityList[i].vBones3D[EntityList[i].iBoneIndex], EntityList[i].iBoneIndex, _profiler.gAutoWall->Current.iValue, &EntityList[i].flDamage);
						EntityList[i].flFOV = _mathematics.CalculateFOV(EntityList[i].vHitLocation);
					}

					else
					{
						EntityList[i].flDistance = _mathematics.CalculateDistance(CG->CEntity[i].vOrigin, CG->vOrigin);
						EntityList[i].bIsVisible = ScanPosition(&CG->CEntity[i], EntityList[i].vHitLocation = EntityList[i].vBones3D[EntityList[i].iBoneIndex = (eBone)_profiler.gAimBone->Current.iValue], _profiler.gAutoWall->Current.iValue, &EntityList[i].flDamage);
						EntityList[i].flFOV = _mathematics.CalculateFOV(EntityList[i].vHitLocation);
					}
				}

				else
				{
					EntityList[i].flDistance = _mathematics.CalculateDistance(CG->CEntity[i].vOrigin, CG->vOrigin);
					EntityList[i].bIsVisible = ScanPosition(&CG->CEntity[i], EntityList[i].vHitLocation = EntityList[i].vBones3D[EntityList[i].iBoneIndex = (eBone)_profiler.gAimBone->Current.iValue], _profiler.gAutoWall->Current.iValue, &EntityList[i].flDamage);
					EntityList[i].flFOV = _mathematics.CalculateFOV(EntityList[i].vHitLocation);
				}

				if (!EntityList[i].bIsVisible && Priorities[i].bDoMultiPoint)
				{
					EntityList[i].flDistance = _mathematics.CalculateDistance(CG->CEntity[i].vOrigin, CG->vOrigin);
					EntityList[i].bIsVisible = ScanMultiPoints(&CG->CEntity[i], EntityList[i].vMultiPoints3D, EntityList[i].vHitLocation = EntityList[i].vBones3D[EntityList[i].iBoneIndex], _profiler.gAutoWall->Current.iValue, &EntityList[i].flDamage);
					EntityList[i].flFOV = _mathematics.CalculateFOV(EntityList[i].vHitLocation);
				}
			}

			else if (CG->CEntity[i].NextEntityState.wEntityType == ET_ACTOR)
			{
				EntityList[i].flDistance = _mathematics.CalculateDistance(CG->CEntity[i].vOrigin, CG->vOrigin);
				EntityList[i].bIsVisible = ScanPosition(&CG->CEntity[i], EntityList[i].vHitLocation = EntityList[i].vBones3D[EntityList[i].iBoneIndex = vBones[BONE_HEAD].first], _profiler.gAutoWall->Current.iValue, &EntityList[i].flDamage);
				EntityList[i].flFOV = _mathematics.CalculateFOV(EntityList[i].vHitLocation);
			}

			if (i < MAX_CLIENTS)
				if (Priorities[i].bIsIgnored)
					continue;

			if (EntityList[i].bIsVisible && _mathematics.CalculateFOV(EntityList[i].vHitLocation) <= _profiler.gAimAngle->Current.flValue)
			{
				if (i < MAX_CLIENTS)
					TargetInfo.bIsPriority = Priorities[i].bIsPrioritized;

				TargetInfo.iIndex = i;

				TargetInfo.flDistance = EntityList[i].flDistance;
				TargetInfo.flDamage = EntityList[i].flDamage;
				TargetInfo.flFOV = EntityList[i].flFOV;

				vTargetInfo.push_back(TargetInfo);
			}

			if (i < MAX_CLIENTS)
			{
				if (Priorities[i].bIsPrioritized)
				{
					AntiAimTargetInfo.iIndex = i;

					AntiAimTargetInfo.flDistance = EntityList[i].flDistance;
					AntiAimTargetInfo.flDamage = EntityList[i].flDamage;
					AntiAimTargetInfo.flFOV = EntityList[i].flFOV;

					vAntiAimTargetInfo.push_back(AntiAimTargetInfo);
				}
			}
		}

		if (!vTargetInfo.empty())
		{
			if (_profiler.gSortMethod->Current.iValue == cProfiler::SORT_METHOD_DISTANCE)
			{
				std::stable_sort(vTargetInfo.begin(), vTargetInfo.end(), [&](const sTargetInfo& a, const sTargetInfo& b) { return a.flDistance < b.flDistance; });

				auto itTargetInfo = std::find_if(vTargetInfo.begin(), vTargetInfo.end(), [&](const sTargetInfo& targetinfo) { return targetinfo.bIsPriority; });

				if (itTargetInfo != vTargetInfo.end())
					_aimBot.AimState.iTargetNum = itTargetInfo->iIndex;

				else
					_aimBot.AimState.iTargetNum = vTargetInfo.front().iIndex;
			}

			else if (_profiler.gSortMethod->Current.iValue == cProfiler::SORT_METHOD_DAMAGE)
			{
				std::stable_sort(vTargetInfo.begin(), vTargetInfo.end(), [&](const sTargetInfo& a, const sTargetInfo& b) { return a.flDamage > b.flDamage; });

				auto itTargetInfo = std::find_if(vTargetInfo.begin(), vTargetInfo.end(), [&](const sTargetInfo& targetinfo) { return targetinfo.bIsPriority; });

				if (itTargetInfo != vTargetInfo.end())
					_aimBot.AimState.iTargetNum = itTargetInfo->iIndex;

				else
					_aimBot.AimState.iTargetNum = vTargetInfo.front().iIndex;
			}
			
			else if (_profiler.gSortMethod->Current.iValue == cProfiler::SORT_METHOD_FOV)
			{
				std::stable_sort(vTargetInfo.begin(), vTargetInfo.end(), [&](const sTargetInfo& a, const sTargetInfo& b) { return a.flFOV < b.flFOV; });

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
				std::stable_sort(vAntiAimTargetInfo.begin(), vAntiAimTargetInfo.end(), [&](const sAntiAimTargetInfo& a, const sAntiAimTargetInfo& b) { return a.flDistance < b.flDistance; });

				_aimBot.AimState.iAntiAimTargetNum = vAntiAimTargetInfo.front().iIndex;
			}

			else if (_profiler.gSortMethod->Current.iValue == cProfiler::SORT_METHOD_DAMAGE)
			{
				std::stable_sort(vAntiAimTargetInfo.begin(), vAntiAimTargetInfo.end(), [&](const sAntiAimTargetInfo& a, const sAntiAimTargetInfo& b) { return a.flDamage > b.flDamage; });

				_aimBot.AimState.iAntiAimTargetNum = vAntiAimTargetInfo.front().iIndex;
			}

			else if (_profiler.gSortMethod->Current.iValue == cProfiler::SORT_METHOD_FOV)
			{
				std::stable_sort(vAntiAimTargetInfo.begin(), vAntiAimTargetInfo.end(), [&](const sAntiAimTargetInfo& a, const sAntiAimTargetInfo& b) { return a.flFOV < b.flFOV; });

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
	bool cTargetList::ScanPosition(sCEntity* entity, ImVec3& position, bool autowall, float* damage)
	{
		if (bIsSteamVersion)
		{
			return std::async(&cTargetList::IsVisibleInternal, this, entity, position, autowall, damage).get();
		}

		else
		{
			return IsVisibleInternal(entity, position, autowall, damage);
		}
	}
	/*
	//=====================================================================================
	*/
	bool cTargetList::ScanBones(sCEntity* entity, ImVec3 bones3d[BONE_MAX], ImVec3& position, eBone& index, bool autowall, float* damage)
	{
		bool bReturn = false;

		sDamageInfo DamageInfo;
		std::vector<sDamageInfo> vDamageInfo;
		std::vector<sDamageInfo> vDamageInfoFinal;
		std::vector<std::future<bool>> vIsVisible(BONE_MAX);

		if (bIsSteamVersion)
		{
			for (auto& Bone : vBones)
			{
				vIsVisible[Bone.first] = std::async(&cTargetList::IsVisibleInternal, this, entity, bones3d[Bone.first], autowall, &DamageInfo.flDamage);

				DamageInfo.iBoneIndex = Bone.first;
				DamageInfo.vPosition = bones3d[Bone.first];

				vDamageInfo.push_back(DamageInfo);
			}

			for (auto& Bone : vBones)
			{
				if (vIsVisible[Bone.first].get())
				{
					vDamageInfoFinal.push_back(vDamageInfo[Bone.first]);

					bReturn = true;
				}
			}
		}

		else
		{
			for (auto& Bone : vBones)
			{
				if (IsVisibleInternal(entity, bones3d[Bone.first], autowall, &DamageInfo.flDamage))
				{
					DamageInfo.iBoneIndex = Bone.first;
					DamageInfo.vPosition = bones3d[Bone.first];

					vDamageInfoFinal.push_back(DamageInfo);

					bReturn = true;
				}
			}
		}

		if (!vDamageInfoFinal.empty())
		{
			std::stable_sort(vDamageInfoFinal.begin(), vDamageInfoFinal.end(), [&](const sDamageInfo& a, const sDamageInfo& b) { return a.flDamage > b.flDamage; });

			if (damage) *damage = vDamageInfoFinal.front().flDamage;

			index = vDamageInfoFinal.front().iBoneIndex;
			position = vDamageInfoFinal.front().vPosition;

			vDamageInfoFinal.clear();
		}

		return bReturn;
	}
	/*
	//=====================================================================================
	*/
	bool cTargetList::ScanMultiPoints(sCEntity* entity, std::vector<ImVec3> multipoints, ImVec3& position, bool autowall, float* damage)
	{
		bool bReturn = false;

		sDamageInfo DamageInfo;
		std::vector<sDamageInfo> vDamageInfo;
		std::vector<sDamageInfo> vDamageInfoFinal;
		std::vector<std::future<bool>> vIsVisible;

		if (bIsSteamVersion)
		{
			for (size_t i = 0; i < multipoints.size(); i++)
			{
				vIsVisible.push_back(std::async(&cTargetList::IsVisibleInternal, this, entity, multipoints[i], autowall, &DamageInfo.flDamage));

				DamageInfo.vPosition = multipoints[i];

				vDamageInfo.push_back(DamageInfo);
			}

			for (size_t i = 0; i < multipoints.size(); i++)
			{
				if (vIsVisible[i].get())
				{
					vDamageInfoFinal.push_back(vDamageInfo[i]);

					bReturn = true;
				}
			}
		}

		else
		{
			for (size_t i = 0; i < multipoints.size(); i++)
			{
				if (IsVisibleInternal(entity, multipoints[i], autowall, &DamageInfo.flDamage))
				{
					DamageInfo.vPosition = multipoints[i];

					vDamageInfoFinal.push_back(DamageInfo);

					bReturn = true;
				}
			}
		}

		if (!vDamageInfoFinal.empty())
		{
			std::stable_sort(vDamageInfoFinal.begin(), vDamageInfoFinal.end(), [&](const sDamageInfo& a, const sDamageInfo& b) { return a.flDamage > b.flDamage; });

			if (damage) *damage = vDamageInfoFinal.front().flDamage;

			position = vDamageInfoFinal.front().vPosition;

			vDamageInfoFinal.clear();
		}

		return bReturn;
	}
	/*
	//=====================================================================================
	*/
	std::vector<ImVec3> cTargetList::CalculateMultiPoints(sCEntity* entity, ImVec3 bones3d[BONE_MAX], int divisor, float radius)
	{
		std::vector<ImVec3> vMultiPoints;

		vMultiPoints.clear();

		auto CalculateInnerPoints = [&](const ImVec3 a, const ImVec3 b, const int c)
		{
			std::vector<ImVec3> vPoints;

			vPoints.clear();

			for (int i = 0; i <= c; i++)
			{
				ImVec3 vPoint = b + ((a - b) / (float)c * (float)i);
				vPoints.push_back(vPoint);
			}

			return vPoints;
		};

		auto CalculateOuterPoints = [&](const std::vector<ImVec3> a, const float b)
		{
			std::vector<ImVec3> vPoints;

			vPoints.clear();

			for (auto& vPoint : a)
			{
				ImVec3 vFront = vPoint + ImVec3(b, 0.0f, 0.0f);
				ImVec3 vBack = vPoint + ImVec3(-b, 0.0f, 0.0f);

				ImVec3 vLeft = vPoint + ImVec3(0.0f, b, 0.0f);
				ImVec3 vRight = vPoint + ImVec3(0.0f, -b, 0.0f);

				_mathematics.RotatePoint(vFront, vPoint, entity->vViewAngles.y, vFront);
				_mathematics.RotatePoint(vBack, vPoint, entity->vViewAngles.y, vBack);

				_mathematics.RotatePoint(vLeft, vPoint, entity->vViewAngles.y, vLeft);
				_mathematics.RotatePoint(vRight, vPoint, entity->vViewAngles.y, vRight);

				vPoints.push_back(vFront);
				vPoints.push_back(vBack);

				vPoints.push_back(vLeft);
				vPoints.push_back(vRight);
			}

			return vPoints;
		};

		std::vector<ImVec3> vUpperLeftLegInner = CalculateInnerPoints(bones3d[vBones[BONE_LEFT_HIP].first], bones3d[vBones[BONE_LEFT_KNEE].first], divisor);
		std::vector<ImVec3> vUpperRightLegInner = CalculateInnerPoints(bones3d[vBones[BONE_RIGHT_HIP].first], bones3d[vBones[BONE_RIGHT_KNEE].first], divisor);

		std::vector<ImVec3> vLowerLeftLegInner = CalculateInnerPoints(bones3d[vBones[BONE_LEFT_KNEE].first], bones3d[vBones[BONE_LEFT_ANKLE].first], divisor);
		std::vector<ImVec3> vLowerRightLegInner = CalculateInnerPoints(bones3d[vBones[BONE_RIGHT_KNEE].first], bones3d[vBones[BONE_RIGHT_ANKLE].first], divisor);

		std::vector<ImVec3> vLeftFootInner = CalculateInnerPoints(bones3d[vBones[BONE_LEFT_ANKLE].first], bones3d[vBones[BONE_LEFT_BALL].first], divisor);
		std::vector<ImVec3> vRightFootInner = CalculateInnerPoints(bones3d[vBones[BONE_RIGHT_ANKLE].first], bones3d[vBones[BONE_RIGHT_BALL].first], divisor);

		std::vector<ImVec3> vUpperLeftLegOuter = CalculateOuterPoints(vUpperLeftLegInner, radius);
		std::vector<ImVec3> vUpperRightLegOuter = CalculateOuterPoints(vUpperRightLegInner, radius);

		std::vector<ImVec3> vLowerLeftLegOuter = CalculateOuterPoints(vLowerLeftLegInner, radius);
		std::vector<ImVec3> vLowerRightLegOuter = CalculateOuterPoints(vLowerRightLegInner, radius);

		std::vector<ImVec3> vLeftFootOuter = CalculateOuterPoints(vLeftFootInner, radius);
		std::vector<ImVec3> vRightFootOuter = CalculateOuterPoints(vRightFootInner, radius);

		vMultiPoints.insert(vMultiPoints.end(), vUpperLeftLegInner.begin(), vUpperLeftLegInner.end());
		vMultiPoints.insert(vMultiPoints.end(), vUpperRightLegInner.begin(), vUpperRightLegInner.end());

		vMultiPoints.insert(vMultiPoints.end(), vLowerLeftLegInner.begin(), vLowerLeftLegInner.end());
		vMultiPoints.insert(vMultiPoints.end(), vLowerRightLegInner.begin(), vLowerRightLegInner.end());

		vMultiPoints.insert(vMultiPoints.end(), vLeftFootInner.begin(), vLeftFootInner.end());
		vMultiPoints.insert(vMultiPoints.end(), vRightFootInner.begin(), vRightFootInner.end());

		vMultiPoints.insert(vMultiPoints.end(), vUpperLeftLegOuter.begin(), vUpperLeftLegOuter.end());
		vMultiPoints.insert(vMultiPoints.end(), vUpperRightLegOuter.begin(), vUpperRightLegOuter.end());

		vMultiPoints.insert(vMultiPoints.end(), vLowerLeftLegOuter.begin(), vLowerLeftLegOuter.end());
		vMultiPoints.insert(vMultiPoints.end(), vLowerRightLegOuter.begin(), vLowerRightLegOuter.end());

		vMultiPoints.insert(vMultiPoints.end(), vLeftFootOuter.begin(), vLeftFootOuter.end());
		vMultiPoints.insert(vMultiPoints.end(), vRightFootOuter.begin(), vRightFootOuter.end());

		return vMultiPoints;
	}
}

//=====================================================================================