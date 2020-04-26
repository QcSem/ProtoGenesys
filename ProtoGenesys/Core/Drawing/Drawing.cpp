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
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(position.x - 1.0f, position.y - 1.0f), ImVec2(position.x + dimentions.x / length + 2.0f, position.y + 2.0f), ImGui::GetColorU32(_profiler.gColorShadow->Current.cValue));
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(position.x - 1.0f, position.y - 1.0f), ImVec2(position.x + 2.0f, position.y + dimentions.y / length + 2.0f), ImGui::GetColorU32(_profiler.gColorShadow->Current.cValue));

			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(position.x + dimentions.x - dimentions.x / length - 1.0f, position.y - 1.0f), ImVec2(position.x + dimentions.x + 1.0f, position.y + 2.0f), ImGui::GetColorU32(_profiler.gColorShadow->Current.cValue));
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(position.x + dimentions.x - 2.0f, position.y - 1.0f), ImVec2(position.x + dimentions.x + 1.0f, position.y + dimentions.y / length + 2.0f), ImGui::GetColorU32(_profiler.gColorShadow->Current.cValue));

			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(position.x - 1.0f, position.y + dimentions.y - 2.0f), ImVec2(position.x + dimentions.x / length + 2.0f, position.y + dimentions.y + 1.0f), ImGui::GetColorU32(_profiler.gColorShadow->Current.cValue));
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(position.x - 1.0f, position.y + dimentions.y - dimentions.y / length - 1.0f), ImVec2(position.x + 2.0f, position.y + dimentions.y + 1.0f), ImGui::GetColorU32(_profiler.gColorShadow->Current.cValue));

			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(position.x + dimentions.x - dimentions.x / length - 1.0f, position.y + dimentions.y - 2.0f), ImVec2(position.x + dimentions.x + 1.0f, position.y + dimentions.y + 1.0f), ImGui::GetColorU32(_profiler.gColorShadow->Current.cValue));
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(position.x + dimentions.x - 2.0f, position.y + dimentions.y - dimentions.y / length - 1.0f), ImVec2(position.x + dimentions.x + 1.0f, position.y + dimentions.y + 1.0f), ImGui::GetColorU32(_profiler.gColorShadow->Current.cValue));
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
			DrawLine(bones2d[vBones[BONE_NECK].first],
				bones2d[vBones[BONE_LEFT_SHOULDER].first],
				color);

			DrawLine(bones2d[vBones[BONE_LEFT_SHOULDER].first],
				bones2d[vBones[BONE_LEFT_ELBOW].first],
				color);

			DrawLine(bones2d[vBones[BONE_LEFT_ELBOW].first],
				bones2d[vBones[BONE_LEFT_WRIST].first],
				color);

			DrawLine(bones2d[vBones[BONE_NECK].first],
				bones2d[vBones[BONE_RIGHT_SHOULDER].first],
				color);

			DrawLine(bones2d[vBones[BONE_RIGHT_SHOULDER].first],
				bones2d[vBones[BONE_RIGHT_ELBOW].first],
				color);

			DrawLine(bones2d[vBones[BONE_RIGHT_ELBOW].first],
				bones2d[vBones[BONE_RIGHT_WRIST].first],
				color);

			DrawLine(bones2d[vBones[BONE_NECK].first],
				bones2d[vBones[BONE_UPPER_SPINE].first],
				color);

			DrawLine(bones2d[vBones[BONE_UPPER_SPINE].first],
				bones2d[vBones[BONE_MIDDLE_SPINE].first],
				color);

			DrawLine(bones2d[vBones[BONE_MIDDLE_SPINE].first],
				bones2d[vBones[BONE_LOWER_SPINE].first],
				color);

			DrawLine(bones2d[vBones[BONE_LOWER_SPINE].first],
				bones2d[vBones[BONE_LEFT_HIP].first],
				color);

			DrawLine(bones2d[vBones[BONE_LEFT_HIP].first],
				bones2d[vBones[BONE_LEFT_KNEE].first],
				color);

			DrawLine(bones2d[vBones[BONE_LEFT_KNEE].first],
				bones2d[vBones[BONE_LEFT_ANKLE].first],
				color);

			DrawLine(bones2d[vBones[BONE_LOWER_SPINE].first],
				bones2d[vBones[BONE_RIGHT_HIP].first],
				color);

			DrawLine(bones2d[vBones[BONE_RIGHT_HIP].first],
				bones2d[vBones[BONE_RIGHT_KNEE].first],
				color);

			DrawLine(bones2d[vBones[BONE_RIGHT_KNEE].first],
				bones2d[vBones[BONE_RIGHT_ANKLE].first],
				color);
		}

		else
		{
			ImGui::GetWindowDrawList()->AddRectFilled(bones2d[vBones[BONE_NECK].first],
				bones2d[vBones[BONE_NECK].first] + ImVec2(1.0f, 1.0f),
				ImGui::GetColorU32(color));

			ImGui::GetWindowDrawList()->AddRectFilled(bones2d[vBones[BONE_UPPER_SPINE].first],
				bones2d[vBones[BONE_UPPER_SPINE].first] + ImVec2(1.0f, 1.0f),
				ImGui::GetColorU32(color));

			ImGui::GetWindowDrawList()->AddRectFilled(bones2d[vBones[BONE_MIDDLE_SPINE].first],
				bones2d[vBones[BONE_MIDDLE_SPINE].first] + ImVec2(1.0f, 1.0f),
				ImGui::GetColorU32(color));

			ImGui::GetWindowDrawList()->AddRectFilled(bones2d[vBones[BONE_LOWER_SPINE].first],
				bones2d[vBones[BONE_LOWER_SPINE].first] + ImVec2(1.0f, 1.0f),
				ImGui::GetColorU32(color));

			ImGui::GetWindowDrawList()->AddRectFilled(bones2d[vBones[BONE_LEFT_SHOULDER].first],
				bones2d[vBones[BONE_LEFT_SHOULDER].first] + ImVec2(1.0f, 1.0f),
				ImGui::GetColorU32(color));

			ImGui::GetWindowDrawList()->AddRectFilled(bones2d[vBones[BONE_RIGHT_SHOULDER].first],
				bones2d[vBones[BONE_RIGHT_SHOULDER].first] + ImVec2(1.0f, 1.0f),
				ImGui::GetColorU32(color));

			ImGui::GetWindowDrawList()->AddRectFilled(bones2d[vBones[BONE_LEFT_HIP].first],
				bones2d[vBones[BONE_LEFT_HIP].first] + ImVec2(1.0f, 1.0f),
				ImGui::GetColorU32(color));

			ImGui::GetWindowDrawList()->AddRectFilled(bones2d[vBones[BONE_RIGHT_HIP].first],
				bones2d[vBones[BONE_RIGHT_HIP].first] + ImVec2(1.0f, 1.0f),
				ImGui::GetColorU32(color));

			ImGui::GetWindowDrawList()->AddRectFilled(bones2d[vBones[BONE_LEFT_ELBOW].first],
				bones2d[vBones[BONE_LEFT_ELBOW].first] + ImVec2(1.0f, 1.0f),
				ImGui::GetColorU32(color));

			ImGui::GetWindowDrawList()->AddRectFilled(bones2d[vBones[BONE_RIGHT_ELBOW].first],
				bones2d[vBones[BONE_RIGHT_ELBOW].first] + ImVec2(1.0f, 1.0f),
				ImGui::GetColorU32(color));

			ImGui::GetWindowDrawList()->AddRectFilled(bones2d[vBones[BONE_LEFT_KNEE].first],
				bones2d[vBones[BONE_LEFT_KNEE].first] + ImVec2(1.0f, 1.0f),
				ImGui::GetColorU32(color));

			ImGui::GetWindowDrawList()->AddRectFilled(bones2d[vBones[BONE_RIGHT_KNEE].first],
				bones2d[vBones[BONE_RIGHT_KNEE].first] + ImVec2(1.0f, 1.0f),
				ImGui::GetColorU32(color));

			ImGui::GetWindowDrawList()->AddRectFilled(bones2d[vBones[BONE_LEFT_WRIST].first],
				bones2d[vBones[BONE_LEFT_WRIST].first] + ImVec2(1.0f, 1.0f),
				ImGui::GetColorU32(color));

			ImGui::GetWindowDrawList()->AddRectFilled(bones2d[vBones[BONE_RIGHT_WRIST].first],
				bones2d[vBones[BONE_RIGHT_WRIST].first] + ImVec2(1.0f, 1.0f),
				ImGui::GetColorU32(color));

			ImGui::GetWindowDrawList()->AddRectFilled(bones2d[vBones[BONE_LEFT_ANKLE].first],
				bones2d[vBones[BONE_LEFT_ANKLE].first] + ImVec2(1.0f, 1.0f),
				ImGui::GetColorU32(color));

			ImGui::GetWindowDrawList()->AddRectFilled(bones2d[vBones[BONE_RIGHT_ANKLE].first],
				bones2d[vBones[BONE_RIGHT_ANKLE].first] + ImVec2(1.0f, 1.0f),
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
			ImGui::GetWindowDrawList()->AddText(ImGui::GetIO().FontDefault, ImGui::GetIO().FontDefault->FontSize, ImVec2(position.x - 1.0f, position.y - 1.0f), ImGui::GetColorU32(ImGui::GetColorU32(_profiler.gColorShadow->Current.cValue)), text.c_str());
			ImGui::GetWindowDrawList()->AddText(ImGui::GetIO().FontDefault, ImGui::GetIO().FontDefault->FontSize, ImVec2(position.x, position.y - 1.0f), ImGui::GetColorU32(ImGui::GetColorU32(_profiler.gColorShadow->Current.cValue)), text.c_str());

			ImGui::GetWindowDrawList()->AddText(ImGui::GetIO().FontDefault, ImGui::GetIO().FontDefault->FontSize, ImVec2(position.x + 1.0f, position.y - 1.0f), ImGui::GetColorU32(ImGui::GetColorU32(_profiler.gColorShadow->Current.cValue)), text.c_str());
			ImGui::GetWindowDrawList()->AddText(ImGui::GetIO().FontDefault, ImGui::GetIO().FontDefault->FontSize, ImVec2(position.x + 1.0f, position.y), ImGui::GetColorU32(ImGui::GetColorU32(_profiler.gColorShadow->Current.cValue)), text.c_str());

			ImGui::GetWindowDrawList()->AddText(ImGui::GetIO().FontDefault, ImGui::GetIO().FontDefault->FontSize, ImVec2(position.x + 1.0f, position.y + 1.0f), ImGui::GetColorU32(ImGui::GetColorU32(_profiler.gColorShadow->Current.cValue)), text.c_str());
			ImGui::GetWindowDrawList()->AddText(ImGui::GetIO().FontDefault, ImGui::GetIO().FontDefault->FontSize, ImVec2(position.x, position.y + 1.0f), ImGui::GetColorU32(ImGui::GetColorU32(_profiler.gColorShadow->Current.cValue)), text.c_str());

			ImGui::GetWindowDrawList()->AddText(ImGui::GetIO().FontDefault, ImGui::GetIO().FontDefault->FontSize, ImVec2(position.x - 1.0f, position.y + 1.0f), ImGui::GetColorU32(ImGui::GetColorU32(_profiler.gColorShadow->Current.cValue)), text.c_str());
			ImGui::GetWindowDrawList()->AddText(ImGui::GetIO().FontDefault, ImGui::GetIO().FontDefault->FontSize, ImVec2(position.x - 1.0f, position.y), ImGui::GetColorU32(ImGui::GetColorU32(_profiler.gColorShadow->Current.cValue)), text.c_str());
		}

		ImGui::GetWindowDrawList()->AddText(ImGui::GetIO().FontDefault, ImGui::GetIO().FontDefault->FontSize, position, ImGui::GetColorU32(color), text.c_str());
	}
	/*
	//=====================================================================================
	*/
	void cDrawing::DrawClient(sEntity* entity, ImVec2 bones2d[BONE_MAX], ImVec2 corners2d[8], ImVec2 center, float scale, float distance, std::string name, ImVec4 color)
	{
		float flWidth, flHeight, flPadding = 3.0f;

		if (entity->NextEntityState.LerpEntityState.eFlags1 & EF1_PRONE)
		{
			flWidth = scale / 1.5f;
			flHeight = scale / 2.0f;
		}

		else if (entity->NextEntityState.LerpEntityState.eFlags1 & EF1_CROUCH)
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

		if (_profiler.gPlayerBoxes->Current.iValue == cProfiler::PLAYER_BOXES_BORDER)
			DrawBorder(ImVec2(center.x - flWidth / 2.0f, center.y - flHeight / 2.0f), ImVec2(flWidth, flHeight), true, bHasRiotShield ? _profiler.gColorAccents->Current.cValue : color);

		else if (_profiler.gPlayerBoxes->Current.iValue == cProfiler::PLAYER_BOXES_CORNER)
			DrawCorners(ImVec2(center.x - flWidth / 2.0f, center.y - flHeight / 2.0f), ImVec2(flWidth, flHeight), 3.0f, false, bHasRiotShield ? _profiler.gColorAccents->Current.cValue : color);

		else if (_profiler.gPlayerBoxes->Current.iValue == cProfiler::PLAYER_BOXES_BORDER_FILLED)
		{
			DrawBorder(ImVec2(center.x - flWidth / 2.0f, center.y - flHeight / 2.0f), ImVec2(flWidth, flHeight), false, (bHasRiotShield ? _profiler.gColorAccents->Current.cValue : color) * ImVec4(1.0f, 1.0f, 1.0f, 0.25f));
			DrawBorder(ImVec2(center.x - flWidth / 2.0f, center.y - flHeight / 2.0f), ImVec2(flWidth, flHeight), true, bHasRiotShield ? _profiler.gColorAccents->Current.cValue : color);
		}

		else if (_profiler.gPlayerBoxes->Current.iValue == cProfiler::PLAYER_BOXES_CORNER_FILLED)
		{
			DrawBorder(ImVec2(center.x - flWidth / 2.0f, center.y - flHeight / 2.0f), ImVec2(flWidth, flHeight), false, (bHasRiotShield ? _profiler.gColorAccents->Current.cValue : color) * ImVec4(1.0f, 1.0f, 1.0f, 0.25f));
			DrawCorners(ImVec2(center.x - flWidth / 2.0f, center.y - flHeight / 2.0f), ImVec2(flWidth, flHeight), 3.0f, false, bHasRiotShield ? _profiler.gColorAccents->Current.cValue : color);
		}

		else if (_profiler.gPlayerBoxes->Current.iValue == cProfiler::PLAYER_BOXES_BORDER_3D)
			DrawBorder3D(corners2d, bHasRiotShield ? _profiler.gColorAccents->Current.cValue : color);

		else if (_profiler.gPlayerBoxes->Current.iValue == cProfiler::PLAYER_BOXES_CORNER_3D)
			DrawCorners3D(corners2d, 3.0f, bHasRiotShield ? _profiler.gColorAccents->Current.cValue : color);

		if (_profiler.gPlayerBones->Current.iValue == cProfiler::PLAYER_BONES_DOTS)
			DrawBones(bones2d, false, color);

		else if (_profiler.gPlayerBones->Current.iValue == cProfiler::PLAYER_BONES_LINES)
			DrawBones(bones2d, true, color);

		if (_profiler.gPlayerSnapLines->Current.iValue == cProfiler::PLAYER_SNAPLINES_TOP)
			DrawLine(ImVec2(center.x, center.y - flHeight / 2.0f), ImVec2(ImGui::GetIO().DisplaySize.x / 2.0f, 0.0f), color);

		else if (_profiler.gPlayerSnapLines->Current.iValue == cProfiler::PLAYER_SNAPLINES_BOTTOM)
			DrawLine(ImVec2(center.x, center.y + flHeight / 2.0f), ImVec2(ImGui::GetIO().DisplaySize.x / 2.0f, ImGui::GetIO().DisplaySize.y), color);

		else if (_profiler.gPlayerSnapLines->Current.iValue == cProfiler::PLAYER_SNAPLINES_CROSSHAIR)
			DrawLine(ImVec2(center.x, center.y), ImVec2(ImGui::GetIO().DisplaySize.x / 2.0f, ImGui::GetIO().DisplaySize.y / 2.0f), color);

		if (_profiler.gPlayerInfo->Current.bValue)
		{
			std::string szText(acut::StripColorCodes(VariadicText("[%im] %s", (int)(distance / M_METERS), name.c_str())));
			ImVec2 vStringSize = ImGui::GetIO().FontDefault->CalcTextSizeA(ImGui::GetIO().FontDefault->FontSize, FLT_MAX, 0.0f, szText.c_str());

			DrawString(szText, ImVec2(center.x - vStringSize.x / 2.0f, center.y - flHeight / 2.0f - flPadding - vStringSize.y), false, _profiler.gColorText->Current.cValue);
		}

		if (_profiler.gPlayerWeapons->Current.bValue)
		{
			std::string szText(acut::StripColorCodes(_targetList.EntityList[entity->NextEntityState.iEntityNum].szWeapon));
			ImVec2 vStringSize = ImGui::GetIO().FontDefault->CalcTextSizeA(ImGui::GetIO().FontDefault->FontSize, FLT_MAX, 0.0f, szText.c_str());

			DrawString(szText, ImVec2(center.x - vStringSize.x / 2.0f, center.y + flHeight / 2.0f + flPadding), false, _profiler.gColorText->Current.cValue);
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

		for (auto& Bone : vBones)
		{
			if (!WorldToScreen(bones3d[Bone.first], vTemp))
				return false;

			if (vTemp.x < vMinTemp.x)
				vMinTemp.x = vTemp.x;

			if (vTemp.x > vMaxTemp.x)
				vMaxTemp.x = vTemp.x;

			if (vTemp.y < vMinTemp.y)
				vMinTemp.y = vTemp.y;

			if (vTemp.y > vMaxTemp.y)
				vMaxTemp.y = vTemp.y;

			bones2d[Bone.first] = vTemp;
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

		if (entity->NextEntityState.LerpEntityState.eFlags1 & EF1_PRONE)
		{
			vMinimum[0] = center[0] - 32.5f;
			vMinimum[1] = center[1] - 20.0f;
			vMinimum[2] = center[2] - 10.0f;

			vMaximum[0] = center[0] + 32.5f;
			vMaximum[1] = center[1] + 20.0f;
			vMaximum[2] = center[2] + 10.0f;
		}

		else if (entity->NextEntityState.LerpEntityState.eFlags1 & EF1_CROUCH)
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
			_mathematics.RotatePoint(corners3d[i], center, entity->vViewAngles[1], corners3d[i]);

			if (!WorldToScreen(corners3d[i], corners2d[i]))
				return false;
		}

		return true;
	}
	/*
	//=====================================================================================
	*/
	void cDrawing::CalculateTracers()
	{
		for (auto Tracer = vTracers.begin(); Tracer != vTracers.end();)
		{
			int iDeltaTime = CG->PlayerState.iServerTime - Tracer->iStartTime;

			if (Tracer->bW2SSuccess = WorldToScreen(Tracer->vHitPos3D, Tracer->vHitPos2D))
			{
				WorldToScreen(Tracer->vStartPos3D, Tracer->vStartPos2D);

				Tracer->cColorTracer[3] = 1.0f - ((float)iDeltaTime / 1000);
				Tracer->cColorHitPos[3] = 1.0f - ((float)iDeltaTime / 1000);
				Tracer->cColorShadow[3] = 1.0f - ((float)iDeltaTime / 1000);
			}

			if (Tracer->cColorTracer[3] <= 0.0f || Tracer->cColorHitPos[3] <= 0.0f || Tracer->cColorShadow[3] <= 0.0f)
				Tracer = vTracers.erase(Tracer);

			else
				++Tracer;
		}
	}
	/*
	//=====================================================================================
	*/
	void cDrawing::DrawESP()
	{
		for (int i = 0; i < MAX_ENTITIES; i++)
		{
			if ((_profiler.gWallHackMode->Current.iValue == cProfiler::WALLHACK_MODE_AXIS && !EntityIsTeammate(&CG->Entity[i])) ||
				(_profiler.gWallHackMode->Current.iValue == cProfiler::WALLHACK_MODE_ALLIES && EntityIsTeammate(&CG->Entity[i])) ||
				_profiler.gWallHackMode->Current.iValue == cProfiler::WALLHACK_MODE_ALL)
			{
				if (_targetList.EntityList[i].bW2SSuccess && _targetList.EntityList[i].bIsValid)
				{
					switch (CG->Entity[i].NextEntityState.wEntityType)
					{
					case ET_PLAYER:
					{
						DrawClient(&CG->Entity[i], _targetList.EntityList[i].vBones2D, _targetList.EntityList[i].vCorners2D, _targetList.EntityList[i].vCenter2D, _targetList.EntityList[i].vLower.y - _targetList.EntityList[i].vUpper.y, _mathematics.CalculateDistance(CG->Entity[i].vOrigin, CG->vOrigin), CG->Client[i].szName, _targetList.EntityList[i].cColor);
					} break;

					case ET_ITEM:
					{
						if (_profiler.gPlayerEntities->Current.bValue)
							DrawEntity(_targetList.EntityList[i].szWeapon, _targetList.EntityList[i].vCenter2D, _mathematics.CalculateDistance(CG->Entity[i].vOrigin, CG->PlayerState.vOrigin), _profiler.gColorText->Current.cValue);
					} break;

					case ET_MISSILE:
					{
						if (_profiler.gPlayerEntities->Current.bValue)
							DrawEntity(_targetList.EntityList[i].szWeapon, _targetList.EntityList[i].vCenter2D, _mathematics.CalculateDistance(CG->Entity[i].vOrigin, CG->PlayerState.vOrigin), _profiler.gColorText->Current.cValue);
					} break;

					case ET_TURRET:
					{
						if (_profiler.gPlayerEntities->Current.bValue)
							DrawEntity("Turret", _targetList.EntityList[i].vCenter2D, _mathematics.CalculateDistance(CG->Entity[i].vOrigin, CG->PlayerState.vOrigin), _profiler.gColorText->Current.cValue);
					} break;

					case ET_HELICOPTER:
					{
						if (_profiler.gPlayerEntities->Current.bValue)
							DrawEntity("Helicopter", _targetList.EntityList[i].vCenter2D, _mathematics.CalculateDistance(CG->Entity[i].vOrigin, CG->PlayerState.vOrigin), _profiler.gColorText->Current.cValue);
					} break;

					case ET_PLANE:
					{
						if (_profiler.gPlayerEntities->Current.bValue)
							DrawEntity("Plane", _targetList.EntityList[i].vCenter2D, _mathematics.CalculateDistance(CG->Entity[i].vOrigin, CG->PlayerState.vOrigin), _profiler.gColorText->Current.cValue);
					} break;

					case ET_VEHICLE:
					{
						if (_profiler.gPlayerEntities->Current.bValue)
							DrawEntity("Vehicle", _targetList.EntityList[i].vCenter2D, _mathematics.CalculateDistance(CG->Entity[i].vOrigin, CG->PlayerState.vOrigin), _profiler.gColorText->Current.cValue);
					} break;

					case ET_ACTOR:
					{
						if (_profiler.gPlayerEntities->Current.bValue)
							DrawEntity("Actor", _targetList.EntityList[i].vCenter2D, _mathematics.CalculateDistance(CG->Entity[i].vOrigin, CG->PlayerState.vOrigin), _profiler.gColorText->Current.cValue);
					} break;
					}
				}
			}
		}
	}
	/*
	//=====================================================================================
	*/
	void cDrawing::DrawCompass()
	{
		if (_profiler.gPlayerCompass->Current.bValue)
		{
			Compass.flArrowSize = ImGui::GetIO().DisplaySize.y / 40.0f;
			Compass.flCompassSize = ImGui::GetIO().DisplaySize.y / 1.5f;

			Compass.vCompassPosition = ImGui::GetIO().DisplaySize / 2.0f;

			for (int i = 0; i < MAX_CLIENTS; i++)
			{
				if (_targetList.EntityList[i].bIsValid)
				{
					if ((_profiler.gWallHackMode->Current.iValue == cProfiler::WALLHACK_MODE_AXIS && !EntityIsTeammate(&CG->Entity[i])) ||
						(_profiler.gWallHackMode->Current.iValue == cProfiler::WALLHACK_MODE_ALLIES && EntityIsTeammate(&CG->Entity[i])) ||
						_profiler.gWallHackMode->Current.iValue == cProfiler::WALLHACK_MODE_ALL)
					{
						float flAngle = (((CG->vRefDefViewAngles[1] - CG->Entity[i].vViewAngles[1]) + 180.0f) / 360.0f - 0.25f) * M_PI_DOUBLE;

						ImVec2 vBaseX = ImVec2(Compass.flArrowSize / -2.0f, 0.0f);
						ImVec2 vBaseY = ImVec2(Compass.flArrowSize / 2.0f, Compass.flArrowSize / 2.0f * -0.75f);
						ImVec2 vBaseZ = ImVec2(Compass.flArrowSize / 2.0f, Compass.flArrowSize / 2.0f * +0.75f);

						ImVec2 vFinalX = Compass.vArrowPosition[i] + ImRotate(vBaseX, ImCos(flAngle), ImSin(flAngle));
						ImVec2 vFinalY = Compass.vArrowPosition[i] + ImRotate(vBaseY, ImCos(flAngle), ImSin(flAngle));
						ImVec2 vFinalZ = Compass.vArrowPosition[i] + ImRotate(vBaseZ, ImCos(flAngle), ImSin(flAngle));

						bool bHasRiotShield = EntityHasRiotShield(&CG->Entity[i]);

						ImGui::GetWindowDrawList()->AddTriangleFilled(vFinalX, vFinalY, vFinalZ,
							ImGui::GetColorU32(_targetList.EntityList[i].cColor * ImVec4(1.0f, 1.0f, 1.0f, 0.25f)));

						ImGui::GetWindowDrawList()->AddLine(vFinalX, vFinalY, ImGui::GetColorU32(bHasRiotShield ? _profiler.gColorAccents->Current.cValue : _targetList.EntityList[i].cColor));
						ImGui::GetWindowDrawList()->AddLine(vFinalY, vFinalZ, ImGui::GetColorU32(bHasRiotShield ? _profiler.gColorAccents->Current.cValue : _targetList.EntityList[i].cColor));
						ImGui::GetWindowDrawList()->AddLine(vFinalZ, vFinalX, ImGui::GetColorU32(bHasRiotShield ? _profiler.gColorAccents->Current.cValue : _targetList.EntityList[i].cColor));
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
		if (_profiler.gPlayerRadar->Current.bValue)
		{
			ImGui::SetNextWindowSize(ImVec2(Radar.flRadarSize, Radar.flRadarSize + ImGui::GetFrameHeight()));
			ImGui::Begin("RADAR", &_profiler.gPlayerRadar->Current.bValue, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
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
					if ((_profiler.gWallHackMode->Current.iValue == cProfiler::WALLHACK_MODE_AXIS && !EntityIsTeammate(&CG->Entity[i])) ||
						(_profiler.gWallHackMode->Current.iValue == cProfiler::WALLHACK_MODE_ALLIES && EntityIsTeammate(&CG->Entity[i])) ||
						_profiler.gWallHackMode->Current.iValue == cProfiler::WALLHACK_MODE_ALL)
					{
						bool bHasRiotShield = EntityHasRiotShield(&CG->Entity[i]);

						ImGui::GetWindowDrawList()->AddCircleFilled(Radar.vBlipPosition[i], Radar.flBlipSize / 2.0f,
							ImGui::GetColorU32(_targetList.EntityList[i].cColor * ImVec4(1.0f, 1.0f, 1.0f, 0.25f)));

						ImGui::GetWindowDrawList()->AddCircle(Radar.vBlipPosition[i], Radar.flBlipSize / 2.0f,
							ImGui::GetColorU32(bHasRiotShield ? _profiler.gColorAccents->Current.cValue : _targetList.EntityList[i].cColor));
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
		if (_profiler.gPlayerCrossHair->Current.bValue)
		{
			if (!(CG->Entity[CG->iClientNum].NextEntityState.LerpEntityState.eFlags1 & EF1_ZOOM))
			{
				ImGui::GetWindowDrawList()->AddLine(ImVec2(ImGui::GetIO().DisplaySize.x / 2.0f - 12.0f, ImGui::GetIO().DisplaySize.y / 2.0f - 1.0f), ImVec2(ImGui::GetIO().DisplaySize.x / 2.0f - 4.0f, ImGui::GetIO().DisplaySize.y / 2.0f - 1.0f), ImGui::GetColorU32(_profiler.gColorCrossHair->Current.cValue));
				ImGui::GetWindowDrawList()->AddLine(ImVec2(ImGui::GetIO().DisplaySize.x / 2.0f + 3.0f, ImGui::GetIO().DisplaySize.y / 2.0f - 1.0f), ImVec2(ImGui::GetIO().DisplaySize.x / 2.0f + 11.0f, ImGui::GetIO().DisplaySize.y / 2.0f - 1.0f), ImGui::GetColorU32(_profiler.gColorCrossHair->Current.cValue));

				ImGui::GetWindowDrawList()->AddLine(ImVec2(ImGui::GetIO().DisplaySize.x / 2.0f - 1.0f, ImGui::GetIO().DisplaySize.y / 2.0f - 12.0f), ImVec2(ImGui::GetIO().DisplaySize.x / 2.0f - 1.0f, ImGui::GetIO().DisplaySize.y / 2.0f - 4.0f), ImGui::GetColorU32(_profiler.gColorCrossHair->Current.cValue));
				ImGui::GetWindowDrawList()->AddLine(ImVec2(ImGui::GetIO().DisplaySize.x / 2.0f - 1.0f, ImGui::GetIO().DisplaySize.y / 2.0f + 3.0f), ImVec2(ImGui::GetIO().DisplaySize.x / 2.0f - 1.0f, ImGui::GetIO().DisplaySize.y / 2.0f + 11.0f), ImGui::GetColorU32(_profiler.gColorCrossHair->Current.cValue));
			}
		}
	}
	/*
	//=====================================================================================
	*/
	void cDrawing::DrawTracers()
	{
		for (auto& Tracer : vTracers)
		{
			if (Tracer.bW2SSuccess)
			{
				ImGui::GetWindowDrawList()->AddLine(Tracer.vStartPos2D, Tracer.vHitPos2D, ImGui::GetColorU32(Tracer.cColorTracer), 1.0f);

				ImGui::GetWindowDrawList()->AddLine(Tracer.vHitPos2D + ImVec2(+6.0f, +6.0f), Tracer.vHitPos2D + ImVec2(-6.0f, -6.0f), ImGui::GetColorU32(Tracer.cColorShadow), 3.0f);
				ImGui::GetWindowDrawList()->AddLine(Tracer.vHitPos2D + ImVec2(+6.0f, -6.0f), Tracer.vHitPos2D + ImVec2(-6.0f, +6.0f), ImGui::GetColorU32(Tracer.cColorShadow), 3.0f);

				ImGui::GetWindowDrawList()->AddLine(Tracer.vHitPos2D + ImVec2(+5.0f, +5.0f), Tracer.vHitPos2D + ImVec2(-5.0f, -5.0f), ImGui::GetColorU32(Tracer.cColorHitPos), 1.0f);
				ImGui::GetWindowDrawList()->AddLine(Tracer.vHitPos2D + ImVec2(+5.0f, -5.0f), Tracer.vHitPos2D + ImVec2(-5.0f, +5.0f), ImGui::GetColorU32(Tracer.cColorHitPos), 1.0f);
			}
		}
	}
	/*
	//=====================================================================================
	*/
	bool cDrawing::ColorPicker(std::string label, ImVec4& color)
	{
		bool bReturn = false;

		int iMiscFlags = ImGuiColorEditFlags_AlphaPreview;
		static bool bSavedPaletteInited = false;
		static ImVec4 cSavedPalette[40];

		if (!bSavedPaletteInited)
		{
			for (int n = 0; n < IM_ARRAYSIZE(cSavedPalette); n++)
			{
				ImGui::ColorConvertHSVtoRGB(n / 39.0f, 0.8f, 0.8f, cSavedPalette[n].x, cSavedPalette[n].y, cSavedPalette[n].z);
				cSavedPalette[n].w = 1.0f;
			}
		}

		bSavedPaletteInited = true;

		static ImVec4 cBackupColor;

		if (ImGui::ColorButton(label.c_str(), color, iMiscFlags))
		{
			ImGui::OpenPopup(label.c_str());
			cBackupColor = color;

			bReturn = true;
		}

		if (ImGui::BeginPopup(label.c_str()))
		{
			ImGui::Text(("CUSTOM COLOR PICKER FOR " + acut::ToUpper(label)).c_str());
			ImGui::Separator();
			ImGui::ColorPicker4("##picker", (float*)&color, iMiscFlags | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
			ImGui::SameLine();
			ImGui::BeginGroup();
			ImGui::Text("Current");
			ImGui::ColorButton("##current", color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40));
			ImGui::Text("Previous");

			if (ImGui::ColorButton("##previous", cBackupColor, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40)))
				color = cBackupColor;

			ImGui::Separator();
			ImGui::Text("Palette");

			for (int i = 0; i < IM_ARRAYSIZE(cSavedPalette); i++)
			{
				ImGui::PushID(i);

				if ((i % 8) != 0)
					ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.y);

				if (ImGui::ColorButton("##palette", cSavedPalette[i], ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(20, 20)))
					color = ImVec4(cSavedPalette[i].x, cSavedPalette[i].y, cSavedPalette[i].z, color.w);

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
						memcpy((float*)&cSavedPalette[i], payload->Data, sizeof(float) * 3);

					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
						memcpy((float*)&cSavedPalette[i], payload->Data, sizeof(float) * 4);

					ImGui::EndDragDropTarget();
				}

				ImGui::PopID();
			}

			ImGui::EndGroup();
			ImGui::EndPopup();
		}

		return bReturn;
	}
}

//=====================================================================================