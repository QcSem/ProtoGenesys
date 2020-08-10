//=====================================================================================

#pragma once

#include "../Engine/Engine.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	class cDrawing
	{
	public:

		struct sTracer
		{
			bool bW2SSuccess;
			int iStartTime;
			ImVec2 vHitPos2D, vStartPos2D;
			ImVec3 vHitPos3D, vStartPos3D;
			ImVec4 cColorShadow, cColorHitMarker, cColorTracer;
			float flAlphaShadow, flAlphaHitMarker, flAlphaTracer;
		};

		std::vector<sTracer> vTracers;

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

		void DrawBorder(ImVec2 position, ImVec2 dimentions, bool border, bool shadow, ImVec4 color);
		void DrawCorners(ImVec2 position, ImVec2 dimentions, float length, bool shadow, ImVec4 color);
		void DrawBorder3D(ImVec2 corners2d[8], bool shadow, ImVec4 color);
		void DrawCorners3D(ImVec2 corners2d[8], float length, bool shadow, ImVec4 color);
		void DrawLine(ImVec2 point1, ImVec2 point2, bool shadow, ImVec4 color);
		void DrawBones(ImVec2 bones2d[BONE_MAX], bool skeleton, bool shadow, ImVec4 color);
		void DrawString(std::string text, ImVec2 position, bool shadow, ImVec4 color);
		void DrawMultiPoints(std::vector<ImVec2> multipoints);
		void DrawClient(sCEntity* entity, ImVec2 bones2d[BONE_MAX], ImVec2 corners2d[8], ImVec2 center, float scale, float distance, std::string name, ImVec4 color);
		void DrawEntity(std::string name, ImVec2 center, float distance, ImVec4 color);
		bool Calculate2D(ImVec3 bones3d[BONE_MAX], ImVec2 bones2d[BONE_MAX], ImVec2& position, ImVec2& dimentions);
		bool Calculate3D(sCEntity* entity, ImVec3 center, ImVec3 corners3d[8], ImVec2 corners2d[8]);
		void CalculateTracers();
		void DrawESP();
		void DrawCompass();
		void DrawRadar();
		void DrawCrosshair();
		void DrawTracers();
		void ThreatWarning();
	} extern _drawing;
}

//=====================================================================================