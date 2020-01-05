//=====================================================================================

#pragma once

#include "../Engine/Engine.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	class cDrawing
	{
	public:

		struct sCompass
		{
			float flCompassSize, flArrowSize;
			ImVec2 vCompassPosition, vArrowPosition[MAX_CLIENTS];
		} Compass;

		struct sRadar
		{
			float flScale = 10.0f, flRadarSize = 200.0f, flBlipSize = 8.0f;
			ImVec2 vRadarPosition, vBlipPosition[MAX_CLIENTS];
		} Radar;

		void DrawBones(ImVec2 bones2d[BONE_MAX], bool skeleton, ImVec4 color);
		void DrawString(std::string text, ImVec2 position, float size, bool shadow, ImVec4 color);
		void DrawCube(ImVec2 corners2d[8], ImVec4 color);
		void DrawCorners(ImVec2 position, ImVec2 dimentions, ImVec2 length, bool shadow, ImVec4 color);
		void DrawBox(ImVec2 position, ImVec2 dimentions, bool border, ImVec4 color);
		void DrawLine(ImVec2 point1, ImVec2 point2, ImVec4 color);
		void DrawPlayer(sEntity* entity, ImVec2 bones2d[BONE_MAX], ImVec2 corners2d[8], ImVec2 center, float scale, float distance, std::string name, ImVec4 color);
		bool Calculate2D(Vector3 bones3d[BONE_MAX], ImVec2 bones2d[BONE_MAX], ImVec2& position, ImVec2& dimentions);
		bool Calculate3D(sEntity* entity, Vector3 center, Vector3 corners3d[8], ImVec2 corners2d[8]);
		void DrawESP();
		void DrawCompass();
		void DrawRadar();
		void DrawCrosshair();
	} extern _drawing;
}

//=====================================================================================