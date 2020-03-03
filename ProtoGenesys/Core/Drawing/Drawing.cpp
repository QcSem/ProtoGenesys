//=====================================================================================

#include "../../StdAfx.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	cDrawing _drawing;

	void cDrawing::DrawBorder(ImVec2 position, ImVec2 dimentions, bool border, ImVec4 color)
	{
		if (border) ImGui::GetWindowDrawList()->AddRect(position, position + dimentions, ImGui::GetColorU32(color));

		else ImGui::GetWindowDrawList()->AddRectFilled(position, position + dimentions, ImGui::GetColorU32(color));
	}
	/*
	//=====================================================================================
	*/
	void cDrawing::DrawCorners(ImVec2 position, ImVec2 dimentions, float length, bool shadow, ImVec4 color)
	{
		if (shadow)
		{
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(position.x - 1.0f, position.y - 1.0f), ImVec2(position.x + dimentions.x / length + 2.0f, position.y + 2.0f), ImGui::GetColorU32(_profiler.gColorShadow->Custom.cValue));
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(position.x - 1.0f, position.y - 1.0f), ImVec2(position.x + 2.0f, position.y + dimentions.y / length + 2.0f), ImGui::GetColorU32(_profiler.gColorShadow->Custom.cValue));

			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(position.x + dimentions.x - dimentions.x / length - 1.0f, position.y - 1.0f), ImVec2(position.x + dimentions.x + 1.0f, position.y + 2.0f), ImGui::GetColorU32(_profiler.gColorShadow->Custom.cValue));
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(position.x + dimentions.x - 2.0f, position.y - 1.0f), ImVec2(position.x + dimentions.x + 1.0f, position.y + dimentions.y / length + 2.0f), ImGui::GetColorU32(_profiler.gColorShadow->Custom.cValue));

			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(position.x - 1.0f, position.y + dimentions.y - 2.0f), ImVec2(position.x + dimentions.x / length + 2.0f, position.y + dimentions.y + 1.0f), ImGui::GetColorU32(_profiler.gColorShadow->Custom.cValue));
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(position.x - 1.0f, position.y + dimentions.y - dimentions.y / length - 1.0f), ImVec2(position.x + 2.0f, position.y + dimentions.y + 1.0f), ImGui::GetColorU32(_profiler.gColorShadow->Custom.cValue));

			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(position.x + dimentions.x - dimentions.x / length - 1.0f, position.y + dimentions.y - 2.0f), ImVec2(position.x + dimentions.x + 1.0f, position.y + dimentions.y + 1.0f), ImGui::GetColorU32(_profiler.gColorShadow->Custom.cValue));
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(position.x + dimentions.x - 2.0f, position.y + dimentions.y - dimentions.y / length - 1.0f), ImVec2(position.x + dimentions.x + 1.0f, position.y + dimentions.y + 1.0f), ImGui::GetColorU32(_profiler.gColorShadow->Custom.cValue));
		}

		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(position.x, position.y), ImVec2(position.x + dimentions.x / length + 1.0f, position.y + 1.0f), ImGui::GetColorU32(color));
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(position.x, position.y), ImVec2(position.x + 1.0f, position.y + dimentions.y / length + 1.0f), ImGui::GetColorU32(color));

		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(position.x + dimentions.x - dimentions.x / length, position.y), ImVec2(position.x + dimentions.x, position.y + 1.0f), ImGui::GetColorU32(color));
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(position.x + dimentions.x - 1.0f, position.y), ImVec2(position.x + dimentions.x, position.y + dimentions.y / length + 1.0f), ImGui::GetColorU32(color));

		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(position.x, position.y + dimentions.y - 1.0f), ImVec2(position.x + dimentions.x / length + 1.0f, position.y + dimentions.y), ImGui::GetColorU32(color));
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(position.x, position.y + dimentions.y - dimentions.y / length), ImVec2(position.x + 1.0f, position.y + dimentions.y), ImGui::GetColorU32(color));

		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(position.x + dimentions.x - dimentions.x / length, position.y + dimentions.y - 1.0f), ImVec2(position.x + dimentions.x, position.y + dimentions.y), ImGui::GetColorU32(color));
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(position.x + dimentions.x - 1.0f, position.y + dimentions.y - dimentions.y / length), ImVec2(position.x + dimentions.x, position.y + dimentions.y), ImGui::GetColorU32(color));
	}
	/*
	//=====================================================================================
	*/
	void cDrawing::DrawBorder3D(ImVec2 corners2d[8], ImVec4 color)
	{
		DrawLine(corners2d[0], corners2d[1], color);
		DrawLine(corners2d[0], corners2d[2], color);
		DrawLine(corners2d[0], corners2d[4], color);
		DrawLine(corners2d[1], corners2d[3], color);
		DrawLine(corners2d[1], corners2d[5], color);
		DrawLine(corners2d[2], corners2d[3], color);
		DrawLine(corners2d[2], corners2d[6], color);
		DrawLine(corners2d[3], corners2d[7], color);
		DrawLine(corners2d[4], corners2d[5], color);
		DrawLine(corners2d[4], corners2d[6], color);
		DrawLine(corners2d[6], corners2d[7], color);
		DrawLine(corners2d[5], corners2d[7], color);
	}
	/*
	//=====================================================================================
	*/
	void cDrawing::DrawCorners3D(ImVec2 corners2d[8], float length, ImVec4 color)
	{
		ImVec2 vMidPoints[12][2];

		auto CalculateMidPoints = [&](const ImVec2 a, const ImVec2 b, ImVec2& c, ImVec2& d, const float e)
		{
			c.x = b.x + (a.x - b.x) / e * (e - 1.0f);
			c.y = b.y + (a.y - b.y) / e * (e - 1.0f);

			d.x = b.x + (a.x - b.x) / e;
			d.y = b.y + (a.y - b.y) / e;
		};

		CalculateMidPoints(corners2d[0], corners2d[1], vMidPoints[0][0], vMidPoints[0][1], length);
		CalculateMidPoints(corners2d[0], corners2d[2], vMidPoints[1][0], vMidPoints[1][1], length);
		CalculateMidPoints(corners2d[0], corners2d[4], vMidPoints[2][0], vMidPoints[2][1], length);
		CalculateMidPoints(corners2d[1], corners2d[3], vMidPoints[3][0], vMidPoints[3][1], length);
		CalculateMidPoints(corners2d[1], corners2d[5], vMidPoints[4][0], vMidPoints[4][1], length);
		CalculateMidPoints(corners2d[2], corners2d[3], vMidPoints[5][0], vMidPoints[5][1], length);
		CalculateMidPoints(corners2d[2], corners2d[6], vMidPoints[6][0], vMidPoints[6][1], length);
		CalculateMidPoints(corners2d[3], corners2d[7], vMidPoints[7][0], vMidPoints[7][1], length);
		CalculateMidPoints(corners2d[4], corners2d[5], vMidPoints[8][0], vMidPoints[8][1], length);
		CalculateMidPoints(corners2d[4], corners2d[6], vMidPoints[9][0], vMidPoints[9][1], length);
		CalculateMidPoints(corners2d[6], corners2d[7], vMidPoints[10][0], vMidPoints[10][1], length);
		CalculateMidPoints(corners2d[5], corners2d[7], vMidPoints[11][0], vMidPoints[11][1], length);

		DrawLine(corners2d[0], vMidPoints[0][0], color);
		DrawLine(corners2d[0], vMidPoints[1][0], color);
		DrawLine(corners2d[0], vMidPoints[2][0], color);
		DrawLine(corners2d[1], vMidPoints[3][0], color);
		DrawLine(corners2d[1], vMidPoints[4][0], color);
		DrawLine(corners2d[2], vMidPoints[5][0], color);
		DrawLine(corners2d[2], vMidPoints[6][0], color);
		DrawLine(corners2d[3], vMidPoints[7][0], color);
		DrawLine(corners2d[4], vMidPoints[8][0], color);
		DrawLine(corners2d[4], vMidPoints[9][0], color);
		DrawLine(corners2d[6], vMidPoints[10][0], color);
		DrawLine(corners2d[5], vMidPoints[11][0], color);

		DrawLine(corners2d[1], vMidPoints[0][1], color);
		DrawLine(corners2d[2], vMidPoints[1][1], color);
		DrawLine(corners2d[4], vMidPoints[2][1], color);
		DrawLine(corners2d[3], vMidPoints[3][1], color);
		DrawLine(corners2d[5], vMidPoints[4][1], color);
		DrawLine(corners2d[3], vMidPoints[5][1], color);
		DrawLine(corners2d[6], vMidPoints[6][1], color);
		DrawLine(corners2d[7], vMidPoints[7][1], color);
		DrawLine(corners2d[5], vMidPoints[8][1], color);
		DrawLine(corners2d[6], vMidPoints[9][1], color);
		DrawLine(corners2d[7], vMidPoints[10][1], color);
		DrawLine(corners2d[7], vMidPoints[11][1], color);
	}
	/*
	//=====================================================================================
	*/
	void cDrawing::DrawLine(ImVec2 point1, ImVec2 point2, ImVec4 color)
	{
		ImGui::GetWindowDrawList()->AddLine(point1, point2, ImGui::GetColorU32(color));
	}
	/*
	//=====================================================================================
	*/
	void cDrawing::DrawBones(ImVec2 bones2d[BONE_MAX], bool skeleton, ImVec4 color)
	{
		if (skeleton)
		{
			DrawLine(bones2d[BONE_NECK],
				bones2d[BONE_LEFT_SHOULDER],
				color);

			DrawLine(bones2d[BONE_LEFT_SHOULDER],
				bones2d[BONE_LEFT_ELBOW],
				color);

			DrawLine(bones2d[BONE_LEFT_ELBOW],
				bones2d[BONE_LEFT_WRIST],
				color);

			DrawLine(bones2d[BONE_NECK],
				bones2d[BONE_RIGHT_SHOULDER],
				color);

			DrawLine(bones2d[BONE_RIGHT_SHOULDER],
				bones2d[BONE_RIGHT_ELBOW],
				color);

			DrawLine(bones2d[BONE_RIGHT_ELBOW],
				bones2d[BONE_RIGHT_WRIST],
				color);

			DrawLine(bones2d[BONE_NECK],
				bones2d[BONE_UPPER_SPINE],
				color);

			DrawLine(bones2d[BONE_UPPER_SPINE],
				bones2d[BONE_LOWER_SPINE],
				color);

			DrawLine(bones2d[BONE_LOWER_SPINE],
				bones2d[BONE_LEFT_HIP],
				color);

			DrawLine(bones2d[BONE_LEFT_HIP],
				bones2d[BONE_LEFT_KNEE],
				color);

			DrawLine(bones2d[BONE_LEFT_KNEE],
				bones2d[BONE_LEFT_ANKLE],
				color);

			DrawLine(bones2d[BONE_LOWER_SPINE],
				bones2d[BONE_RIGHT_HIP],
				color);

			DrawLine(bones2d[BONE_RIGHT_HIP],
				bones2d[BONE_RIGHT_KNEE],
				color);

			DrawLine(bones2d[BONE_RIGHT_KNEE],
				bones2d[BONE_RIGHT_ANKLE],
				color);
		}

		else
		{
			ImGui::GetWindowDrawList()->AddRectFilled(bones2d[BONE_NECK],
				bones2d[BONE_NECK] + ImVec2(1.0f, 1.0f),
				ImGui::GetColorU32(color));

			ImGui::GetWindowDrawList()->AddRectFilled(bones2d[BONE_UPPER_SPINE],
				bones2d[BONE_UPPER_SPINE] + ImVec2(1.0f, 1.0f),
				ImGui::GetColorU32(color));

			ImGui::GetWindowDrawList()->AddRectFilled(bones2d[BONE_LOWER_SPINE],
				bones2d[BONE_LOWER_SPINE] + ImVec2(1.0f, 1.0f),
				ImGui::GetColorU32(color));

			ImGui::GetWindowDrawList()->AddRectFilled(bones2d[BONE_LEFT_SHOULDER],
				bones2d[BONE_LEFT_SHOULDER] + ImVec2(1.0f, 1.0f),
				ImGui::GetColorU32(color));

			ImGui::GetWindowDrawList()->AddRectFilled(bones2d[BONE_RIGHT_SHOULDER],
				bones2d[BONE_RIGHT_SHOULDER] + ImVec2(1.0f, 1.0f),
				ImGui::GetColorU32(color));

			ImGui::GetWindowDrawList()->AddRectFilled(bones2d[BONE_LEFT_HIP],
				bones2d[BONE_LEFT_HIP] + ImVec2(1.0f, 1.0f),
				ImGui::GetColorU32(color));

			ImGui::GetWindowDrawList()->AddRectFilled(bones2d[BONE_RIGHT_HIP],
				bones2d[BONE_RIGHT_HIP] + ImVec2(1.0f, 1.0f),
				ImGui::GetColorU32(color));

			ImGui::GetWindowDrawList()->AddRectFilled(bones2d[BONE_LEFT_ELBOW],
				bones2d[BONE_LEFT_ELBOW] + ImVec2(1.0f, 1.0f),
				ImGui::GetColorU32(color));

			ImGui::GetWindowDrawList()->AddRectFilled(bones2d[BONE_RIGHT_ELBOW],
				bones2d[BONE_RIGHT_ELBOW] + ImVec2(1.0f, 1.0f),
				ImGui::GetColorU32(color));

			ImGui::GetWindowDrawList()->AddRectFilled(bones2d[BONE_LEFT_KNEE],
				bones2d[BONE_LEFT_KNEE] + ImVec2(1.0f, 1.0f),
				ImGui::GetColorU32(color));

			ImGui::GetWindowDrawList()->AddRectFilled(bones2d[BONE_RIGHT_KNEE],
				bones2d[BONE_RIGHT_KNEE] + ImVec2(1.0f, 1.0f),
				ImGui::GetColorU32(color));

			ImGui::GetWindowDrawList()->AddRectFilled(bones2d[BONE_LEFT_WRIST],
				bones2d[BONE_LEFT_WRIST] + ImVec2(1.0f, 1.0f),
				ImGui::GetColorU32(color));

			ImGui::GetWindowDrawList()->AddRectFilled(bones2d[BONE_RIGHT_WRIST],
				bones2d[BONE_RIGHT_WRIST] + ImVec2(1.0f, 1.0f),
				ImGui::GetColorU32(color));

			ImGui::GetWindowDrawList()->AddRectFilled(bones2d[BONE_LEFT_ANKLE],
				bones2d[BONE_LEFT_ANKLE] + ImVec2(1.0f, 1.0f),
				ImGui::GetColorU32(color));

			ImGui::GetWindowDrawList()->AddRectFilled(bones2d[BONE_RIGHT_ANKLE],
				bones2d[BONE_RIGHT_ANKLE] + ImVec2(1.0f, 1.0f),
				ImGui::GetColorU32(color));
		}
	}
	/*
	//=====================================================================================
	*/
	void cDrawing::DrawString(std::string text, ImVec2 position, bool shadow, ImVec4 color)
	{
		if (shadow)
		{
			ImGui::GetWindowDrawList()->AddText(ImGui::GetIO().FontDefault, ImGui::GetIO().FontDefault->FontSize, ImVec2(position.x - 1.0f, position.y - 1.0f), ImGui::GetColorU32(ImGui::GetColorU32(_profiler.gColorShadow->Custom.cValue)), text.c_str());
			ImGui::GetWindowDrawList()->AddText(ImGui::GetIO().FontDefault, ImGui::GetIO().FontDefault->FontSize, ImVec2(position.x, position.y - 1.0f), ImGui::GetColorU32(ImGui::GetColorU32(_profiler.gColorShadow->Custom.cValue)), text.c_str());

			ImGui::GetWindowDrawList()->AddText(ImGui::GetIO().FontDefault, ImGui::GetIO().FontDefault->FontSize, ImVec2(position.x + 1.0f, position.y - 1.0f), ImGui::GetColorU32(ImGui::GetColorU32(_profiler.gColorShadow->Custom.cValue)), text.c_str());
			ImGui::GetWindowDrawList()->AddText(ImGui::GetIO().FontDefault, ImGui::GetIO().FontDefault->FontSize, ImVec2(position.x + 1.0f, position.y), ImGui::GetColorU32(ImGui::GetColorU32(_profiler.gColorShadow->Custom.cValue)), text.c_str());

			ImGui::GetWindowDrawList()->AddText(ImGui::GetIO().FontDefault, ImGui::GetIO().FontDefault->FontSize, ImVec2(position.x + 1.0f, position.y + 1.0f), ImGui::GetColorU32(ImGui::GetColorU32(_profiler.gColorShadow->Custom.cValue)), text.c_str());
			ImGui::GetWindowDrawList()->AddText(ImGui::GetIO().FontDefault, ImGui::GetIO().FontDefault->FontSize, ImVec2(position.x, position.y + 1.0f), ImGui::GetColorU32(ImGui::GetColorU32(_profiler.gColorShadow->Custom.cValue)), text.c_str());

			ImGui::GetWindowDrawList()->AddText(ImGui::GetIO().FontDefault, ImGui::GetIO().FontDefault->FontSize, ImVec2(position.x - 1.0f, position.y + 1.0f), ImGui::GetColorU32(ImGui::GetColorU32(_profiler.gColorShadow->Custom.cValue)), text.c_str());
			ImGui::GetWindowDrawList()->AddText(ImGui::GetIO().FontDefault, ImGui::GetIO().FontDefault->FontSize, ImVec2(position.x - 1.0f, position.y), ImGui::GetColorU32(ImGui::GetColorU32(_profiler.gColorShadow->Custom.cValue)), text.c_str());
		}

		ImGui::GetWindowDrawList()->AddText(ImGui::GetIO().FontDefault, ImGui::GetIO().FontDefault->FontSize, position, ImGui::GetColorU32(color), text.c_str());
	}
	/*
	//=====================================================================================
	*/
	void cDrawing::DrawClient(sEntity* entity, ImVec2 bones2d[BONE_MAX], ImVec2 corners2d[8], ImVec2 center, float scale, float distance, std::string name, ImVec4 color)
	{
		float flWidth, flHeight, flPadding = 3.0f;

		if (entity->NextEntityState.LerpEntityState.eFlags1 & EF_PRONE)
		{
			flWidth = scale / 1.5f;
			flHeight = scale / 2.0f;
		}

		else if (entity->NextEntityState.LerpEntityState.eFlags1 & EF_CROUCH)
		{
			flWidth = scale / 2.0f;
			flHeight = scale / 2.0f;
		}

		else
		{
			flWidth = scale / 2.0f;
			flHeight = scale / 1.5f;
		}

		flWidth *= 2.0f;
		flHeight *= 2.0f;

		bool bHasRiotShield = EntityHasRiotShield(entity);

		if (_profiler.gPlayerBoxes->Custom.iValue == cProfiler::PLAYER_BOXES_BORDER)
			DrawBorder(ImVec2(center.x - flWidth / 2.0f, center.y - flHeight / 2.0f), ImVec2(flWidth, flHeight), true, bHasRiotShield ? _profiler.gColorRiotShield->Custom.cValue : color);

		else if (_profiler.gPlayerBoxes->Custom.iValue == cProfiler::PLAYER_BOXES_CORNER)
			DrawCorners(ImVec2(center.x - flWidth / 2.0f, center.y - flHeight / 2.0f), ImVec2(flWidth, flHeight), 3.0f, false, bHasRiotShield ? _profiler.gColorRiotShield->Custom.cValue : color);

		else if (_profiler.gPlayerBoxes->Custom.iValue == cProfiler::PLAYER_BOXES_BORDER_FILLED)
		{
			DrawBorder(ImVec2(center.x - flWidth / 2.0f, center.y - flHeight / 2.0f), ImVec2(flWidth, flHeight), false, (bHasRiotShield ? _profiler.gColorRiotShield->Custom.cValue : color) * ImVec4(1.0f, 1.0f, 1.0f, 0.25f));
			DrawBorder(ImVec2(center.x - flWidth / 2.0f, center.y - flHeight / 2.0f), ImVec2(flWidth, flHeight), true, bHasRiotShield ? _profiler.gColorRiotShield->Custom.cValue : color);
		}

		else if (_profiler.gPlayerBoxes->Custom.iValue == cProfiler::PLAYER_BOXES_CORNER_FILLED)
		{
			DrawBorder(ImVec2(center.x - flWidth / 2.0f, center.y - flHeight / 2.0f), ImVec2(flWidth, flHeight), false, (bHasRiotShield ? _profiler.gColorRiotShield->Custom.cValue : color) * ImVec4(1.0f, 1.0f, 1.0f, 0.25f));
			DrawCorners(ImVec2(center.x - flWidth / 2.0f, center.y - flHeight / 2.0f), ImVec2(flWidth, flHeight), 3.0f, false, bHasRiotShield ? _profiler.gColorRiotShield->Custom.cValue : color);
		}

		else if (_profiler.gPlayerBoxes->Custom.iValue == cProfiler::PLAYER_BOXES_BORDER_3D)
			DrawBorder3D(corners2d, bHasRiotShield ? _profiler.gColorRiotShield->Custom.cValue : color);

		else if (_profiler.gPlayerBoxes->Custom.iValue == cProfiler::PLAYER_BOXES_CORNER_3D)
			DrawCorners3D(corners2d, 3.0f, bHasRiotShield ? _profiler.gColorRiotShield->Custom.cValue : color);

		if (_profiler.gPlayerBones->Custom.iValue == cProfiler::PLAYER_BONES_DOTS)
			DrawBones(bones2d, false, color);

		else if (_profiler.gPlayerBones->Custom.iValue == cProfiler::PLAYER_BONES_LINES)
			DrawBones(bones2d, true, color);

		if (_profiler.gPlayerSnapLines->Custom.iValue == cProfiler::PLAYER_SNAPLINES_TOP)
			DrawLine(ImVec2(center.x, center.y - flHeight / 2.0f), ImVec2(ImGui::GetIO().DisplaySize.x / 2.0f, 0.0f), color);

		else if (_profiler.gPlayerSnapLines->Custom.iValue == cProfiler::PLAYER_SNAPLINES_BOTTOM)
			DrawLine(ImVec2(center.x, center.y + flHeight / 2.0f), ImVec2(ImGui::GetIO().DisplaySize.x / 2.0f, ImGui::GetIO().DisplaySize.y), color);

		else if (_profiler.gPlayerSnapLines->Custom.iValue == cProfiler::PLAYER_SNAPLINES_CROSSHAIR)
			DrawLine(ImVec2(center.x, center.y), ImVec2(ImGui::GetIO().DisplaySize.x / 2.0f, ImGui::GetIO().DisplaySize.y / 2.0f), color);

		if (_profiler.gPlayerInfo->Custom.bValue)
		{
			std::string szText(acut::StripColorCodes(VariadicText("[%im] %s", (int)(distance / M_METERS), name.c_str())));
			ImVec2 vStringSize = ImGui::GetIO().FontDefault->CalcTextSizeA(ImGui::GetIO().FontDefault->FontSize, FLT_MAX, 0.0f, szText.c_str());
			
			DrawString(szText, ImVec2(center.x - vStringSize.x / 2.0f, center.y - flHeight / 2.0f - flPadding - vStringSize.y), false, _profiler.gColorText->Custom.cValue);
		}

		if (_profiler.gPlayerWeapons->Custom.bValue)
		{
			std::string szText(acut::StripColorCodes(_targetList.EntityList[entity->NextEntityState.iEntityNum].szWeapon));
			ImVec2 vStringSize = ImGui::GetIO().FontDefault->CalcTextSizeA(ImGui::GetIO().FontDefault->FontSize, FLT_MAX, 0.0f, szText.c_str());
			
			DrawString(szText, ImVec2(center.x - vStringSize.x / 2.0f, center.y + flHeight / 2.0f + flPadding), false, _profiler.gColorText->Custom.cValue);
		}
	}
	/*
	//=====================================================================================
	*/
	void cDrawing::DrawEntity(std::string name, ImVec2 center, float distance, ImVec4 color)
	{
		std::string szText(acut::StripColorCodes(VariadicText("[%im] %s", (int)(distance / M_METERS), name.c_str())));
		ImVec2 vStringSize = ImGui::GetIO().FontDefault->CalcTextSizeA(ImGui::GetIO().FontDefault->FontSize, FLT_MAX, 0.0f, szText.c_str());

		DrawString(szText, ImVec2(center.x - vStringSize.x / 2.0f, center.y - vStringSize.y / 2.0f), true, color);
	}
	/*
	//=====================================================================================
	*/
	bool cDrawing::Calculate2D(Vector3 bones3d[BONE_MAX], ImVec2 bones2d[BONE_MAX], ImVec2& position, ImVec2& dimentions)
	{
		ImVec2 vTemp, vMaxTemp = { -FLT_MAX, -FLT_MAX }, vMinTemp = { FLT_MAX, FLT_MAX };

		for (int i = BONE_NECK; i < BONE_MAX; i++)
		{
			if (!_mathematics.WorldToScreen(bones3d[i], vTemp))
				return false;

			if (vTemp.x < vMinTemp.x)
				vMinTemp.x = vTemp.x;

			if (vTemp.x > vMaxTemp.x)
				vMaxTemp.x = vTemp.x;

			if (vTemp.y < vMinTemp.y)
				vMinTemp.y = vTemp.y;

			if (vTemp.y > vMaxTemp.y)
				vMaxTemp.y = vTemp.y;

			bones2d[i] = vTemp;
		}

		dimentions = vMaxTemp - vMinTemp;
		position = vMinTemp;

		return true;
	}
	/*
	//=====================================================================================
	*/
	bool cDrawing::Calculate3D(sEntity* entity, Vector3 center, Vector3 corners3d[8], ImVec2 corners2d[8])
	{
		Vector3 vMinimum, vMaximum;

		if (entity->NextEntityState.LerpEntityState.eFlags1 & EF_PRONE)
		{
			vMinimum[0] = center[0] - 32.5f;
			vMinimum[1] = center[1] - 20.0f;
			vMinimum[2] = center[2] - 10.0f;

			vMaximum[0] = center[0] + 32.5f;
			vMaximum[1] = center[1] + 20.0f;
			vMaximum[2] = center[2] + 10.0f;
		}

		else if (entity->NextEntityState.LerpEntityState.eFlags1 & EF_CROUCH)
		{
			vMinimum[0] = center[0] - 20.0f;
			vMinimum[1] = center[1] - 20.0f;
			vMinimum[2] = center[2] - 21.0f;

			vMaximum[0] = center[0] + 20.0f;
			vMaximum[1] = center[1] + 20.0f;
			vMaximum[2] = center[2] + 21.0f;
		}

		else
		{
			vMinimum[0] = center[0] - 20.0f;
			vMinimum[1] = center[1] - 20.0f;
			vMinimum[2] = center[2] - 32.5f;

			vMaximum[0] = center[0] + 20.0f;
			vMaximum[1] = center[1] + 20.0f;
			vMaximum[2] = center[2] + 32.5f;
		}

		VectorCopy(vMinimum, corners3d[0]);
		VectorCopy(vMaximum, corners3d[7]);
		VectorCopy(vMinimum, corners3d[1]);
		VectorCopy(vMinimum, corners3d[2]);
		VectorCopy(vMinimum, corners3d[3]);

		corners3d[1][0] = vMaximum[0];
		corners3d[2][1] = vMaximum[1];
		corners3d[3][0] = vMaximum[0];
		corners3d[3][1] = vMaximum[1];

		VectorCopy(corners3d[0], corners3d[4]);
		corners3d[4][2] = vMaximum[2];

		VectorCopy(corners3d[1], corners3d[5]);
		corners3d[5][2] = vMaximum[2];

		VectorCopy(corners3d[2], corners3d[6]);
		corners3d[6][2] = vMaximum[2];

		for (int i = 0; i < 8; i++)
		{
			_mathematics.RotatePoint(corners3d[i], center, entity->vAngles[1], corners3d[i]);

			if (!_mathematics.WorldToScreen(corners3d[i], corners2d[i]))
				return false;
		}

		return true;
	}
	/*
	//=====================================================================================
	*/
	void cDrawing::DrawESP()
	{
		for (int i = 0; i < MAX_ENTITIES; i++)
		{
			if (_targetList.EntityList[i].bW2SSuccess && _targetList.EntityList[i].bIsValid)
			{
				switch (CG->Entity[i].NextEntityState.wEntityType)
				{
				case ET_PLAYER:
				{
					if ((_profiler.gWallHackMode->Custom.iValue == cProfiler::WALLHACK_MODE_AXIS && _targetList.EntityIsEnemy(i)) ||
						(_profiler.gWallHackMode->Custom.iValue == cProfiler::WALLHACK_MODE_ALLIES && !_targetList.EntityIsEnemy(i)) ||
						_profiler.gWallHackMode->Custom.iValue == cProfiler::WALLHACK_MODE_ALL)
					{
						DrawClient(&CG->Entity[i], _targetList.EntityList[i].vBones2D, _targetList.EntityList[i].vCorners2D, _targetList.EntityList[i].vCenter2D, _targetList.EntityList[i].vLower.y - _targetList.EntityList[i].vUpper.y, _mathematics.CalculateDistance(CG->Entity[i].vOrigin, CG->vOrigin), CG->Client[i].szName, _targetList.EntityList[i].cColor);
					}
				} break;

				case ET_ITEM:
				{
					if (_profiler.gPlayerEntities->Custom.bValue)
						DrawEntity("Item", _targetList.EntityList[i].vCenter2D, _mathematics.CalculateDistance(CG->Entity[i].vOrigin, CG->PlayerState.vOrigin), _profiler.gColorText->Custom.cValue);
				} break;

				case ET_MISSILE:
				{
					if (_profiler.gPlayerEntities->Custom.bValue)
						DrawEntity("Missile", _targetList.EntityList[i].vCenter2D, _mathematics.CalculateDistance(CG->Entity[i].vOrigin, CG->PlayerState.vOrigin), _profiler.gColorText->Custom.cValue);
				} break;

				case ET_TURRET:
				{
					if (_profiler.gPlayerEntities->Custom.bValue)
						DrawEntity("Turret", _targetList.EntityList[i].vCenter2D, _mathematics.CalculateDistance(CG->Entity[i].vOrigin, CG->PlayerState.vOrigin), _profiler.gColorText->Custom.cValue);
				} break;

				case ET_HELICOPTER:
				{
					if (_profiler.gPlayerEntities->Custom.bValue)
						DrawEntity("Helicopter", _targetList.EntityList[i].vCenter2D, _mathematics.CalculateDistance(CG->Entity[i].vOrigin, CG->PlayerState.vOrigin), _profiler.gColorText->Custom.cValue);
				} break;

				case ET_PLANE:
				{
					if (_profiler.gPlayerEntities->Custom.bValue)
						DrawEntity("Plane", _targetList.EntityList[i].vCenter2D, _mathematics.CalculateDistance(CG->Entity[i].vOrigin, CG->PlayerState.vOrigin), _profiler.gColorText->Custom.cValue);
				} break;

				case ET_VEHICLE:
				{
					if (_profiler.gPlayerEntities->Custom.bValue)
						DrawEntity("Vehicle", _targetList.EntityList[i].vCenter2D, _mathematics.CalculateDistance(CG->Entity[i].vOrigin, CG->PlayerState.vOrigin), _profiler.gColorText->Custom.cValue);
				} break;

				case ET_ACTOR:
				{
					if (_profiler.gPlayerEntities->Custom.bValue)
						DrawEntity("Actor", _targetList.EntityList[i].vCenter2D, _mathematics.CalculateDistance(CG->Entity[i].vOrigin, CG->PlayerState.vOrigin), _profiler.gColorText->Custom.cValue);
				} break;
				}
			}
		}
	}
	/*
	//=====================================================================================
	*/
	void cDrawing::DrawCompass()
	{
		if (_profiler.gPlayerCompass->Custom.bValue)
		{
			_drawing.Compass.flArrowSize = ImGui::GetIO().DisplaySize.y / 40.0f;
			_drawing.Compass.flCompassSize = ImGui::GetIO().DisplaySize.y / 1.5f;

			_drawing.Compass.vCompassPosition = ImGui::GetIO().DisplaySize / 2.0f;

			for (int i = 0; i < MAX_CLIENTS; i++)
			{
				if (_targetList.EntityList[i].bIsValid)
				{
					if ((_profiler.gWallHackMode->Custom.iValue == cProfiler::WALLHACK_MODE_AXIS && _targetList.EntityIsEnemy(i)) ||
						(_profiler.gWallHackMode->Custom.iValue == cProfiler::WALLHACK_MODE_ALLIES && !_targetList.EntityIsEnemy(i)) ||
						_profiler.gWallHackMode->Custom.iValue == cProfiler::WALLHACK_MODE_ALL)
					{
						float flAngle = (((CG->vRefDefViewAngles[1] - CG->Entity[i].vAngles[1]) + 180.0f) / 360.0f - 0.25f) * M_PI_DOUBLE;

						ImVec2 vBaseX = ImVec2(Compass.flArrowSize / -2.0f, 0.0f);
						ImVec2 vBaseY = ImVec2(Compass.flArrowSize / 2.0f, Compass.flArrowSize / 2.0f * -0.75f);
						ImVec2 vBaseZ = ImVec2(Compass.flArrowSize / 2.0f, Compass.flArrowSize / 2.0f * +0.75f);

						ImVec2 vFinalX = Compass.vArrowPosition[i] + ImRotate(vBaseX, ImCos(flAngle), ImSin(flAngle));
						ImVec2 vFinalY = Compass.vArrowPosition[i] + ImRotate(vBaseY, ImCos(flAngle), ImSin(flAngle));
						ImVec2 vFinalZ = Compass.vArrowPosition[i] + ImRotate(vBaseZ, ImCos(flAngle), ImSin(flAngle));

						ImGui::GetWindowDrawList()->AddTriangleFilled(vFinalX, vFinalY, vFinalZ,
							ImGui::GetColorU32(_targetList.EntityList[i].cColor * ImVec4(1.0f, 1.0f, 1.0f, 0.25f)));

						ImGui::GetWindowDrawList()->AddLine(vFinalX, vFinalY, ImGui::GetColorU32(_targetList.EntityList[i].cColor));
						ImGui::GetWindowDrawList()->AddLine(vFinalY, vFinalZ, ImGui::GetColorU32(_targetList.EntityList[i].cColor));
						ImGui::GetWindowDrawList()->AddLine(vFinalZ, vFinalX, ImGui::GetColorU32(_targetList.EntityList[i].cColor));
					}
				}
			}
		}
	}
	/*
	//=====================================================================================
	*/
	void cDrawing::DrawRadar()
	{
		if (_profiler.gPlayerRadar->Custom.bValue)
		{
			ImGui::SetNextWindowSize(ImVec2(Radar.flRadarSize, Radar.flRadarSize + ImGui::GetFrameHeight()));
			ImGui::Begin("RADAR", &_profiler.gPlayerRadar->Custom.bValue, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
			ImGui::GetWindowDrawList()->PushClipRectFullScreen();

			Radar.vRadarPosition = ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x / 2.0f,
				(ImGui::GetWindowPos().y + ImGui::GetFrameHeight()) + (ImGui::GetWindowSize().y - ImGui::GetFrameHeight()) / 2.0f);

			ImGui::GetWindowDrawList()->AddLine(ImVec2(Radar.vRadarPosition.x - Radar.flRadarSize / 2.0f + 1.0f, Radar.vRadarPosition.y),
				ImVec2(Radar.vRadarPosition.x + Radar.flRadarSize / 2.0f - 1.0f, Radar.vRadarPosition.y), ImGui::GetColorU32(ImVec4(0.50f, 0.50f, 0.50f, 0.50f)));

			ImGui::GetWindowDrawList()->AddLine(ImVec2(Radar.vRadarPosition.x, Radar.vRadarPosition.y - Radar.flRadarSize / 2.0f),
				ImVec2(Radar.vRadarPosition.x, Radar.vRadarPosition.y + Radar.flRadarSize / 2.0f - 1.0f), ImGui::GetColorU32(ImVec4(0.50f, 0.50f, 0.50f, 0.50f)));

			ImGui::GetWindowDrawList()->AddRectFilled(
				Radar.vRadarPosition - ImVec2(2.0f, 2.0f),
				Radar.vRadarPosition + ImVec2(3.0f, 3.0f),
				ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)));

			for (int i = 0; i < MAX_CLIENTS; i++)
			{
				if (_targetList.EntityList[i].bIsValid)
				{
					if ((_profiler.gWallHackMode->Custom.iValue == cProfiler::WALLHACK_MODE_AXIS && _targetList.EntityIsEnemy(i)) ||
						(_profiler.gWallHackMode->Custom.iValue == cProfiler::WALLHACK_MODE_ALLIES && !_targetList.EntityIsEnemy(i)) ||
						_profiler.gWallHackMode->Custom.iValue == cProfiler::WALLHACK_MODE_ALL)
					{
						ImGui::GetWindowDrawList()->AddCircleFilled(Radar.vBlipPosition[i], Radar.flBlipSize / 2.0f,
							ImGui::GetColorU32(_targetList.EntityList[i].cColor * ImVec4(1.0f, 1.0f, 1.0f, 0.25f)));

						ImGui::GetWindowDrawList()->AddCircle(Radar.vBlipPosition[i], Radar.flBlipSize / 2.0f,
							ImGui::GetColorU32(_targetList.EntityList[i].cColor));
					}
				}
			}

			ImGui::End();
		}
	}
	/*
	//=====================================================================================
	*/
	void cDrawing::DrawCrosshair()
	{
		if (_profiler.gPlayerCrossHair->Custom.bValue)
		{
			if (!(CG->Entity[CG->iClientNum].NextEntityState.LerpEntityState.eFlags1 & EF_ZOOM))
			{
				ImGui::GetWindowDrawList()->AddLine(ImVec2(ImGui::GetIO().DisplaySize.x / 2.0f - 12.0f, ImGui::GetIO().DisplaySize.y / 2.0f - 1.0f), ImVec2(ImGui::GetIO().DisplaySize.x / 2.0f - 4.0f, ImGui::GetIO().DisplaySize.y / 2.0f - 1.0f), ImGui::GetColorU32(_profiler.gColorCrossHair->Custom.cValue));
				ImGui::GetWindowDrawList()->AddLine(ImVec2(ImGui::GetIO().DisplaySize.x / 2.0f + 3.0f, ImGui::GetIO().DisplaySize.y / 2.0f - 1.0f), ImVec2(ImGui::GetIO().DisplaySize.x / 2.0f + 11.0f, ImGui::GetIO().DisplaySize.y / 2.0f - 1.0f), ImGui::GetColorU32(_profiler.gColorCrossHair->Custom.cValue));

				ImGui::GetWindowDrawList()->AddLine(ImVec2(ImGui::GetIO().DisplaySize.x / 2.0f - 1.0f, ImGui::GetIO().DisplaySize.y / 2.0f - 12.0f), ImVec2(ImGui::GetIO().DisplaySize.x / 2.0f - 1.0f, ImGui::GetIO().DisplaySize.y / 2.0f - 4.0f), ImGui::GetColorU32(_profiler.gColorCrossHair->Custom.cValue));
				ImGui::GetWindowDrawList()->AddLine(ImVec2(ImGui::GetIO().DisplaySize.x / 2.0f - 1.0f, ImGui::GetIO().DisplaySize.y / 2.0f + 3.0f), ImVec2(ImGui::GetIO().DisplaySize.x / 2.0f - 1.0f, ImGui::GetIO().DisplaySize.y / 2.0f + 11.0f), ImGui::GetColorU32(_profiler.gColorCrossHair->Custom.cValue));
			}
		}
	}
}

//=====================================================================================