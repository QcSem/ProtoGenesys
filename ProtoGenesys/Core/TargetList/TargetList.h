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
			int iIndex;

			float flDistance2D = FLT_MAX, flDistance3D = FLT_MAX;
		} sTargetInfo;

		typedef struct
		{
			float flDamage;
			int iBoneIndex;
		} sDamageInfo;

		struct sEntityInfo
		{
			bool bIsValid, bW2SSuccess, bIsVisible;
			int iBoneIndex, iLastBone;
			std::string szWeapon;
			ImVec2 vBones2D[BONE_MAX], vCorners2D[8], vCenter2D, vPosition, vDimentions, vLower, vUpper;
			Vector3 vBones3D[BONE_MAX], vCorners3D[8], vCenter3D, vHitLocation;
			ImVec4 cColor;
		} EntityInfo[MAX_ENTITIES];

		int iSelectedTarget;
		Vector3 vSelectedTarget;

		std::vector<BOOL> vIsTarget = std::vector<BOOL>(MAX_CLIENTS, FALSE);

		void GetInformation();
		bool EntityIsValid(int index);
		bool EntityIsEnemy(int index);
		bool IsVisible(sEntity* entity, Vector3 position, bool autowall, float* damage);
		bool Bonescan(sEntity* entity, Vector3 bones3d[BONE_MAX], bool autowall, int* index);
		void ApplyPrediction(sEntity* entity, Vector3 position);
	} extern _targetList;
}

//=====================================================================================
