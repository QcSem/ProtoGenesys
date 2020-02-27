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

			float flFOV = FLT_MAX, flDistance = FLT_MAX;
		} sTargetInfo;

		typedef struct
		{
			float flDamage;
			int iBoneIndex;
		} sDamageInfo;

		struct sEntityList
		{
			bool bIsValid, bW2SSuccess, bIsVisible;
			int iBoneIndex, iLastBone;
			std::string szWeapon;
			ImVec2 vBones2D[BONE_MAX], vCorners2D[8], vCenter2D, vPosition, vDimentions, vLower, vUpper;
			Vector3 vBones3D[BONE_MAX], vCorners3D[8], vCenter3D, vHitLocation;
			ImVec4 cColor;
		} EntityList[MAX_ENTITIES];

		int iRiotShieldTarget;
		Vector3 vRiotShieldTarget;

		std::vector<BOOL> vIsTarget = std::vector<BOOL>(MAX_CLIENTS, TRUE);

		void GetInformation();
		bool EntityIsValid(int index);
		bool EntityIsEnemy(int index);
		bool IsVisible(sEntity* entity, Vector3 position, bool autowall, float* damage);
		bool Bonescan(sEntity* entity, Vector3 bones3d[BONE_MAX], bool autowall, int* index);
		void ApplyPrediction(sEntity* entity, Vector3 position);
	} extern _targetList;
}

//=====================================================================================
