//=====================================================================================

#include "../../StdAfx.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	cTargetList _targetList;

	void cTargetList::GetInformation()
	{
		ImVec3 vViewOrigin;
		GetPlayerViewOrigin(&vViewOrigin);

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
				EntityList[i].vScanPoints.clear();

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
					if (_profiler.gBoneScan->Current.iValue &&
						(((_profiler.gBoneScanPriorities->Current.iValue && Priorities[i].bIsPrioritized) ||
						(_profiler.gBoneScanRiotShielders->Current.iValue && EntityHasRiotShield(&CG->CEntity[i]))) ||
						(!_profiler.gBoneScanPriorities->Current.iValue && !_profiler.gBoneScanRiotShielders->Current.iValue)))
					{
						for (auto& Bone : vBones)
						{
							EntityList[i].vScanPoints.push_back(EntityList[i].vBones3D[Bone.first]);
						}
					}

					else
					{
						EntityList[i].vScanPoints.push_back(EntityList[i].vBones3D[_profiler.gAimBone->Current.iValue]);
					}

					if (Priorities[i].bIsMultiPoint)
					{
						EntityList[i].vMultiPoints2D.clear();
						EntityList[i].vMultiPoints3D.clear();

						EntityList[i].vMultiPoints3D = CalculateMultiPoints(&CG->CEntity[i], EntityList[i].vBones3D, _profiler.gPointScale->Current.iValue, 2.0f);

						for (auto& MultiPoint3D : EntityList[i].vMultiPoints3D)
						{
							ImVec2 vMultiPoint2D;

							_mathematics.WorldToScreen(MultiPoint3D, vMultiPoint2D);
							EntityList[i].vMultiPoints2D.push_back(vMultiPoint2D);
							EntityList[i].vScanPoints.push_back(MultiPoint3D);
						}
					}
				}

				else if (CG->CEntity[i].NextEntityState.wEntityType == ET_ACTOR)
				{
					EntityList[i].vScanPoints.push_back(EntityList[i].vBones3D[vBones[BONE_HEAD].first]);
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

				bool bIsVisible = IsVisible(&CG->CEntity[i], std::vector<ImVec3>({ EntityList[i].vBones3D[vBones[BONE_HEAD].first] }), NULL, false, NULL);

				if (EntityIsTeammate(&CG->CEntity[i]))
				{
					EntityList[i].cColor = bIsVisible ? _profiler.gColorAlliesVisible->Current.cValue : _profiler.gColorAlliesInvisible->Current.cValue;
					continue;
				}

				EntityList[i].cColor = bIsVisible ? _profiler.gColorAxisVisible->Current.cValue : _profiler.gColorAxisInvisible->Current.cValue;
			}

			else if (CG->CEntity[i].NextEntityState.wEntityType == ET_ACTOR)
			{
				EntityList[i].bW2SSuccess = _mathematics.WorldToScreen(EntityList[i].vBones3D[vBones[BONE_HEAD].first], EntityList[i].vCenter2D);

				if (!_profiler.gTargetK9Unit->Current.iValue || EntityIsTeammate(&CG->CEntity[i]))
					continue;
			}

			else
			{
				EntityList[i].bW2SSuccess = _mathematics.WorldToScreen(CG->CEntity[i].vOrigin, EntityList[i].vCenter2D);
				continue;
			}

			EntityList[i].flDistance = _mathematics.CalculateDistance3D(CG->CEntity[i].vOrigin, CG->vOrigin);
			EntityList[i].bIsVisible = IsVisible(&CG->CEntity[i], EntityList[i].vScanPoints, &EntityList[i].vHitLocation, _profiler.gAutoWall->Current.iValue, &EntityList[i].flDamage);
			EntityList[i].flFOV = _mathematics.CalculateFOV(EntityList[i].vHitLocation, vViewOrigin, CG->vRefDefViewAngles);

			if (i < MAX_CLIENTS)
				if (Priorities[i].bIsIgnored)
					continue;

			if (EntityList[i].bIsVisible && EntityList[i].flFOV <= _profiler.gAimAngle->Current.flValue)
			{
				if (i < MAX_CLIENTS)
					TargetInfo.bIsPriority = Priorities[i].bIsPrioritized;

				TargetInfo.iIndex = i;

				TargetInfo.flDistance = EntityList[i].flDistance;
				TargetInfo.flDamage = EntityList[i].flDamage;
				TargetInfo.flFOV = EntityList[i].flFOV;

				vTargetInfo.push_back(TargetInfo);
			}
		}

		if (!vTargetInfo.empty())
		{
			if (_profiler.gSortMethodTargets->Current.iValue == cProfiler::SORT_METHOD_DISTANCE)
			{
				std::stable_sort(vTargetInfo.begin(), vTargetInfo.end(), [&](const sTargetInfo& a, const sTargetInfo& b) { return a.flDistance < b.flDistance; });

				auto itTargetInfo = std::find_if(vTargetInfo.begin(), vTargetInfo.end(), [&](const sTargetInfo& targetinfo) { return targetinfo.bIsPriority; });

				if (itTargetInfo != vTargetInfo.end())
					_aimBot.AimState.iTargetNum = itTargetInfo->iIndex;

				else
					_aimBot.AimState.iTargetNum = vTargetInfo.front().iIndex;
			}

			else if (_profiler.gSortMethodTargets->Current.iValue == cProfiler::SORT_METHOD_DAMAGE)
			{
				std::stable_sort(vTargetInfo.begin(), vTargetInfo.end(), [&](const sTargetInfo& a, const sTargetInfo& b) { return a.flDamage > b.flDamage; });

				auto itTargetInfo = std::find_if(vTargetInfo.begin(), vTargetInfo.end(), [&](const sTargetInfo& targetinfo) { return targetinfo.bIsPriority; });

				if (itTargetInfo != vTargetInfo.end())
					_aimBot.AimState.iTargetNum = itTargetInfo->iIndex;

				else
					_aimBot.AimState.iTargetNum = vTargetInfo.front().iIndex;
			}
			
			else if (_profiler.gSortMethodTargets->Current.iValue == cProfiler::SORT_METHOD_FOV)
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
	bool cTargetList::IsVisible(sCEntity* entity, std::vector<ImVec3> scanpoints, ImVec3* position, bool autowall, float* damage)
	{
		ImVec3 vViewOrigin;
		GetPlayerViewOrigin(&vViewOrigin);

		sDamageInfo DamageInfo;
		std::vector<sDamageInfo> vDamageInfo;
		std::vector<sDamageInfo> vDamageInfoFinal;
		std::vector<std::future<bool>> vIsVisible;

		if (bIsSteamVersion)
		{
			for (size_t i = 0; i < scanpoints.size(); i++)
			{
				DamageInfo.flDistance = _mathematics.CalculateDistance3D(scanpoints[i], vViewOrigin);
				vIsVisible.push_back(std::async(&cTargetList::IsVisibleInternal, this, entity, scanpoints[i], autowall, &DamageInfo.flDamage));
				DamageInfo.flFOV = _mathematics.CalculateFOV(scanpoints[i], vViewOrigin, CG->vRefDefViewAngles);

				DamageInfo.vPosition = scanpoints[i];

				vDamageInfo.push_back(DamageInfo);
			}

			for (size_t i = 0; i < scanpoints.size(); i++)
			{
				if (vIsVisible[i].get())
				{
					vDamageInfoFinal.push_back(vDamageInfo[i]);
				}
			}
		}

		else
		{
			for (size_t i = 0; i < scanpoints.size(); i++)
			{
				if (IsVisibleInternal(entity, scanpoints[i], autowall, &DamageInfo.flDamage))
				{
					DamageInfo.flDistance = _mathematics.CalculateDistance3D(scanpoints[i], vViewOrigin);
					DamageInfo.flFOV = _mathematics.CalculateFOV(scanpoints[i], vViewOrigin, CG->vRefDefViewAngles);

					DamageInfo.vPosition = scanpoints[i];

					vDamageInfoFinal.push_back(DamageInfo);
				}
			}
		}

		if (!vDamageInfoFinal.empty())
		{
			if (_profiler.gSortMethodBones->Current.iValue == cProfiler::SORT_METHOD_DISTANCE)
			{
				std::stable_sort(vDamageInfoFinal.begin(), vDamageInfoFinal.end(), [&](const sDamageInfo& a, const sDamageInfo& b) { return a.flDistance < b.flDistance; });

				if (damage)
					*damage = vDamageInfoFinal.front().flDamage;

				if (position)
					*position = vDamageInfoFinal.front().vPosition;
			}

			if (_profiler.gSortMethodBones->Current.iValue == cProfiler::SORT_METHOD_DAMAGE)
			{
				std::stable_sort(vDamageInfoFinal.begin(), vDamageInfoFinal.end(), [&](const sDamageInfo& a, const sDamageInfo& b) { return a.flDamage > b.flDamage; });

				if (damage)
					*damage = vDamageInfoFinal.front().flDamage;

				if (position)
					*position = vDamageInfoFinal.front().vPosition;
			}

			if (_profiler.gSortMethodBones->Current.iValue == cProfiler::SORT_METHOD_FOV)
			{
				std::stable_sort(vDamageInfoFinal.begin(), vDamageInfoFinal.end(), [&](const sDamageInfo& a, const sDamageInfo& b) { return a.flFOV < b.flFOV; });

				if (damage)
					*damage = vDamageInfoFinal.front().flDamage;

				if (position)
					*position = vDamageInfoFinal.front().vPosition;
			}

			vDamageInfoFinal.clear();

			return true;
		}

		return false;
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