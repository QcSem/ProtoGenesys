//=====================================================================================

#pragma once

#include "../Engine/Engine.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	class cMathematics
	{
	public:

		float CalculateFOV(ImVec3 position);
		float DotProduct3D(ImVec3 left, ImVec3 right);
		float DotProduct2D(ImVec2 left, ImVec2 right);
		float VectorLength3D(ImVec3 left, ImVec3 right);
		float VectorLength2D(ImVec2 left, ImVec2 right);
		float CalculateDistance3D(ImVec3 start, ImVec3 end);
		float CalculateDistance2D(ImVec2 start, ImVec2 end);
		void VectorAngles(ImVec3 direction, ImVec3& angles);
		void AngleVectors(ImVec3 angles, ImVec3& forward, ImVec3& right, ImVec3& up);
		void VectorNormalize(ImVec3& direction);
		void ClampAngles(ImVec3& angles);
		char ClampMove(char value);
		void CalculateAimAngles(ImVec3 start, ImVec3 end, ImVec3& angles);
		void CalculateAntiAimAngles(ImVec3 start, ImVec3 end, ImVec3& angles);
		void MakeVector(ImVec3 angles, ImVec3& out);
		void MovementFix(sUserCmd* usercmd, float yaw, float oldyaw, float forward, float right);
		bool WorldToScreen(ImVec3 world, ImVec2& screen);
		void WorldToCompass(ImVec3 world, ImVec2 compasspos, float compasssize, ImVec2& screen);
		void WorldToRadar(ImVec3 world, ImVec2 radarpos, float scale, float radarsize, float blipsize, ImVec2& screen);
		void RotatePoint(ImVec3 point, ImVec3 center, float yaw, ImVec3& out);
		void ApplyPositionPrediction(sCEntity* entity);
		void ApplyAnglePrediction(sCEntity* entity);
		float EntityInterpolation(sTrajectory* trajectory, int time, ImVec3& result, float scale);
	} extern _mathematics;
}

//=====================================================================================
