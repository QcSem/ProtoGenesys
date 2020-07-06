//=====================================================================================

#pragma once

#include "../Engine/Engine.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	class cTargetList
	{
	public:

		struct sTargetInfo
		{
			int iIndex;
			float flDistance = FLT_MAX, flDamage = FLT_MAX, flFOV = FLT_MAX;
			bool bIsPriority;
		};

		struct sAntiAimTargetInfo
		{
			int iIndex;
			float flDistance = FLT_MAX, flDamage = FLT_MAX, flFOV = FLT_MAX;
		};

		struct sDamageInfo
		{
			float flDamage;
			eBone iBoneIndex;
			ImVec3 vPosition;
		};

		struct sEntityList
		{
			bool bIsValid, bW2SSuccess, bIsVisible;
			eBone iBoneIndex, iLastBone;
			std::string szWeapon;
			std::vector<ImVec2> vMultiPoints2D;
			std::vector<ImVec3> vMultiPoints3D;
			float flDamage;
			ImVec2 vBones2D[BONE_MAX], vCorners2D[8], vCenter2D, vPosition, vDimentions, vLower, vUpper;
			ImVec3 vBones3D[BONE_MAX], vCorners3D[8], vCenter3D, vHitLocation;
			ImVec4 cColor;
		} EntityList[MAX_ENTITIES];

		struct sPriorities
		{
			bool bIsPrioritized, bIsIgnored, bDoMultiPoint;
		} Priorities[MAX_CLIENTS];

		void GetInformation();
		bool IsVisibleInternal(sCEntity* entity, ImVec3 position, bool autowall, float* damage);
		bool ScanPosition(sCEntity* entity, ImVec3 position, bool autowall, float* damage);
		bool ScanBones(sCEntity* entity, ImVec3 bones3d[BONE_MAX], ImVec3& position, bool autowall, eBone& index, float* damage);
		bool ScanMultiPoints(sCEntity* entity, std::vector<ImVec3> multipoints, ImVec3& position, bool autowall, float* damage);
		std::vector<ImVec3> CalculateMultiPoints(sCEntity* entity, ImVec3 bones3d[BONE_MAX], int divisor);
	} extern _targetList;
}

//=====================================================================================
