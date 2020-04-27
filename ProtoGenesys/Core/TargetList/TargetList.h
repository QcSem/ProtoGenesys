//=====================================================================================

#pragma once

#include "../Engine/Engine.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	class cTargetList
	{
	public:

		typedef struct
		{
			bool bIsPriority;
			int iIndex;
			float flFOV = FLT_MAX, flDistance = FLT_MAX;
		} sTargetInfo;

		typedef struct
		{
			int iIndex;
			float flFOV = FLT_MAX, flDistance = FLT_MAX;
		} sAntiAimTargetInfo;

		typedef struct
		{
			float flDamage;
			eBone iBoneIndex;
		} sDamageInfo;

		struct sEntityList
		{
			bool bIsValid, bW2SSuccess, bIsVisible;
			eBone iBoneIndex, iLastBone;
			std::string szWeapon;
			ImVec2 vBones2D[BONE_MAX], vCorners2D[8], vCenter2D, vPosition, vDimentions, vLower, vUpper;
			Vector3 vBones3D[BONE_MAX], vCorners3D[8], vCenter3D, vHitLocation;
			ImVec4 cColor;
		} EntityList[MAX_ENTITIES];

		bool bIsPriority[MAX_CLIENTS] = { false };

		void GetInformation();
		bool IsLocalPlayerValid();
		bool IsEntityValid(int index);
		bool IsVisibleInternal(sEntity* entity, Vector3 position, short hitloc, bool autowall, float* damage);
		bool IsVisible(sEntity* entity, Vector3 bones3d[BONE_MAX], bool bonescan, bool autowall, eBone& index);
		void ApplyPrediction(sEntity* entity, Vector3 position);
	} extern _targetList;
}

//=====================================================================================
