//=====================================================================================

#include "../StdAfx.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	cMainGUI _mainGui;

	void cMainGUI::InitInterface()
	{
		hWindow = *(HWND*)dwWindowHandle;

		if (!hWindow || !pDevice || !pDeviceContext)
			return;

		oWindowProcess = (tWindowProcess)SetWindowLongPtr(hWindow, GWLP_WNDPROC, (LONG_PTR)_thunkWindowProcess.GetThunk());

		ImGui::CreateContext();
		ImGui_ImplWin32_Init(hWindow);
		ImGui_ImplDX11_Init(pDevice, pDeviceContext);

		szIniFileName = acut::GetExeDirectory() + DEFAULT_INI;
		szLogFileName = acut::GetExeDirectory() + DEFAULT_LOG;

		ImGui::GetIO().IniFilename = szIniFileName.c_str();
		ImGui::GetIO().LogFilename = szLogFileName.c_str();

		_profiler.gMenuColor->Current.iValue = GetPrivateProfileInt("MenuStyle", "COLOR", cProfiler::MENU_COLOR_NEUTRAL, (acut::GetExeDirectory() + DEFAULT_CFG).c_str());
		_profiler.gMenuCursor->Current.iValue = GetPrivateProfileInt("MenuStyle", "CURSOR", cProfiler::MENU_CURSOR_BLACK, (acut::GetExeDirectory() + DEFAULT_CFG).c_str());
		_profiler.gMenuFont->Current.iValue = GetPrivateProfileInt("MenuStyle", "FONT", cProfiler::MENU_FONT_LIGHT, (acut::GetExeDirectory() + DEFAULT_CFG).c_str());

		RefreshInterface(_profiler.gMenuColor->Current.iValue, _profiler.gMenuCursor->Current.iValue, _profiler.gMenuFont->Current.iValue);

		bInitialized = true;
	}
	/*
	//=====================================================================================
	*/
	void cMainGUI::SetMenuColor(int index)
	{
		switch (index)
		{
		case cProfiler::MENU_COLOR_NEUTRAL:
			ImGui::StyleColorsNeutral();
			break;

		case cProfiler::MENU_COLOR_NEUTRAL_NEON:
			ImGui::StyleColorsNeutralNeon();
			break;

		case cProfiler::MENU_COLOR_RED:
			ImGui::StyleColorsRed();
			break;

		case cProfiler::MENU_COLOR_RED_NEON:
			ImGui::StyleColorsRedNeon();
			break;

		case cProfiler::MENU_COLOR_ORANGE:
			ImGui::StyleColorsOrange();
			break;

		case cProfiler::MENU_COLOR_ORANGE_NEON:
			ImGui::StyleColorsOrangeNeon();
			break;

		case cProfiler::MENU_COLOR_YELLOW:
			ImGui::StyleColorsYellow();
			break;

		case cProfiler::MENU_COLOR_YELLOW_NEON:
			ImGui::StyleColorsYellowNeon();
			break;

		case cProfiler::MENU_COLOR_GREEN:
			ImGui::StyleColorsGreen();
			break;

		case cProfiler::MENU_COLOR_GREEN_NEON:
			ImGui::StyleColorsGreenNeon();
			break;

		case cProfiler::MENU_COLOR_BLUE:
			ImGui::StyleColorsBlue();
			break;

		case cProfiler::MENU_COLOR_BLUE_NEON:
			ImGui::StyleColorsBlueNeon();
			break;

		case cProfiler::MENU_COLOR_PURPLE:
			ImGui::StyleColorsPurple();
			break;

		case cProfiler::MENU_COLOR_PURPLE_NEON:
			ImGui::StyleColorsPurpleNeon();
			break;

		case cProfiler::MENU_COLOR_RAINBOW:
			ImGui::StyleColorsRed();
			break;

		case cProfiler::MENU_COLOR_RAINBOW_NEON:
			ImGui::StyleColorsRedNeon();
			break;

		default:
			ImGui::StyleColorsNeutral();
			break;
		}
	}
	/*
	//=====================================================================================
	*/
	void cMainGUI::SetMenuCursor(int index)
	{
		switch (index)
		{
		case cProfiler::MENU_CURSOR_BLACK:
			ImGui::StyleCursorsBlack();
			break;

		case cProfiler::MENU_CURSOR_WHITE:
			ImGui::StyleCursorsWhite();
			break;

		default:
			ImGui::StyleCursorsBlack();
			break;
		}
	}
	/*
	//=====================================================================================
	*/
	void cMainGUI::SetMenuFont(int index)
	{
		switch (index)
		{
		case cProfiler::MENU_FONT_LIGHT:
			ImGui::GetIO().FontDefault = ImGui::GetIO().Fonts->Fonts[cProfiler::MENU_FONT_LIGHT];
			break;

		case cProfiler::MENU_FONT_MEDIUM:
			ImGui::GetIO().FontDefault = ImGui::GetIO().Fonts->Fonts[cProfiler::MENU_FONT_MEDIUM];
			break;

		case cProfiler::MENU_FONT_BOLD:
			ImGui::GetIO().FontDefault = ImGui::GetIO().Fonts->Fonts[cProfiler::MENU_FONT_BOLD];
			break;

		default:
			ImGui::GetIO().FontDefault = ImGui::GetIO().Fonts->Fonts[cProfiler::MENU_FONT_LIGHT];
			break;
		}
	}
	/*
	//=====================================================================================
	*/
	void cMainGUI::RefreshInterface(int color, int cursor, int font)
	{
		SetMenuColor(color);

		ImGui::GetIO().Fonts->Clear();

		SetMenuCursor(cursor);

		ImGui::GetIO().Fonts->AddFontLight();
		ImGui::GetIO().Fonts->AddFontMedium();
		ImGui::GetIO().Fonts->AddFontBold();

		Eurostile_Bold = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(eurostile_bold_compressed_data_base85, flEurostile_Bold = Window->iHeight / 80.0f);
		Eurostile_Extended = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(eurostile_extended_compressed_data_base85, flEurostile_Extended = Window->iHeight / 60.0f);
		Eurostile_Regular = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(eurostile_regular_compressed_data_base85, flEurostile_Regular = Window->iHeight / 80.0f);

		ImGui_ImplDX11_CreateDeviceObjects();

		SetMenuFont(font);
	}
	/*
	//=====================================================================================
	*/
	bool cMainGUI::GetKeyPress(int vkey, bool immediate)
	{
		if (VirtualKeys[vkey].bKey)
		{
			VirtualKeys[vkey].bUp = false;
			VirtualKeys[vkey].bDown = true;
		}

		else if (!VirtualKeys[vkey].bKey && VirtualKeys[vkey].bDown)
		{
			VirtualKeys[vkey].bUp = true;
			VirtualKeys[vkey].bDown = false;
		}

		else
		{
			VirtualKeys[vkey].bUp = false;
			VirtualKeys[vkey].bDown = false;
		}

		if (immediate)
			return VirtualKeys[vkey].bDown;

		else
			return VirtualKeys[vkey].bUp;
	}
	/*
	//=====================================================================================
	*/
	LRESULT cMainGUI::WindowProcess(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_LBUTTONDOWN:
			VirtualKeys[VK_LBUTTON].bKey = true;
			break;

		case WM_LBUTTONUP:
			VirtualKeys[VK_LBUTTON].bKey = false;
			break;

		case WM_RBUTTONDOWN:
			VirtualKeys[VK_RBUTTON].bKey = true;
			break;

		case WM_RBUTTONUP:
			VirtualKeys[VK_RBUTTON].bKey = false;
			break;

		case WM_KEYDOWN:
			VirtualKeys[wParam].bKey = true;
			break;

		case WM_KEYUP:
			VirtualKeys[wParam].bKey = false;
			break;
		}

		if (GetKeyPress(VK_INSERT, false))
			bShowWindow = !bShowWindow;

		if (GetKeyPress(VK_HOME, false))
			_profiler.LoadProfile("");

		if (GetKeyPress(VK_END, false))
			_profiler.DisableAll();

		*(bool*)dwMouseInput = !bShowWindow;

		if (bInitialized && bShowWindow && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
			return TRUE;

		return CallWindowProc(oWindowProcess, hWnd, uMsg, wParam, lParam);
	}
	/*
	//=====================================================================================
	*/
	void WINAPI cMainGUI::Present(_In_ IDXGISwapChain* pSwapChain, _In_ UINT SyncInterval, _In_ UINT Flags)
	{
		if (!bInitialized)
		{
			pSwapChain->GetDevice(__uuidof(pDevice), (void**)&pDevice);
			pDevice->GetImmediateContext(&pDeviceContext);
			InitInterface();
		}

		else
		{
			if (_profiler.gMenuColor->Current.iValue == cProfiler::MENU_COLOR_RAINBOW || _profiler.gMenuColor->Current.iValue == cProfiler::MENU_COLOR_RAINBOW_NEON)
			{
				static float flHue = 0.0f;

				if (flHue > 255.0f)
					flHue = 0.0f;

				for (int i = 0; i < ImGuiCol_COUNT; i++)
				{
					Vector3 vHSV;

					ImGui::ColorConvertRGBtoHSV(ImGui::GetStyle().Colors[i].x, ImGui::GetStyle().Colors[i].y, ImGui::GetStyle().Colors[i].z, vHSV[0], vHSV[1], vHSV[2]);
					ImGui::ColorConvertHSVtoRGB(ByteToFloat(flHue), vHSV[1], vHSV[2], ImGui::GetStyle().Colors[i].x, ImGui::GetStyle().Colors[i].y, ImGui::GetStyle().Colors[i].z);
				}

				for (int i = 0; i < ImGui::TabLabelStyle::Col_TabLabel_Count; i++)
				{
					Vector3 vHSV;
					ImVec4 vColor = ImGui::ColorConvertU32ToFloat4(ImGui::TabLabelStyle::Get().colors[i]);

					ImGui::ColorConvertRGBtoHSV(vColor.x, vColor.y, vColor.z, vHSV[0], vHSV[1], vHSV[2]);
					ImGui::ColorConvertHSVtoRGB(ByteToFloat(flHue), vHSV[1], vHSV[2], vColor.x, vColor.y, vColor.z);

					ImGui::TabLabelStyle::Get().colors[i] = ImGui::ColorConvertFloat4ToU32(vColor);
				}

				flHue += (60.0f / ImGui::GetIO().Framerate);
			}

			ImGui::GetIO().MouseDrawCursor = bShowWindow;

			ImGui_ImplWin32_NewFrame();
			ImGui_ImplDX11_NewFrame();
			ImGui::NewFrame();

			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

			ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
			ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

			ImGui::Begin("INVISIBLE", (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs);
			ImGui::GetWindowDrawList()->PushClipRectFullScreen();

			ImGui::PopStyleColor(2);

			if (LocalClientIsInGame() && CG->PlayerState.iOtherFlags & 0x4)
			{
				_drawing.DrawESP();
				_drawing.DrawCompass();
				_drawing.DrawRadar();
				_drawing.DrawCrosshair();
				_drawing.DrawTracers();
			}

			ClientActive = *(sClientActive**)dwClientActive;

			std::string szWatermark(VariadicText("PROTOGENESYS - COD BO2 by: InUrFace | Frametime: %s, Ping: %s",
				LocalClientIsInGame() ? VariadicText("%i ms", CG->iFrameTime).c_str() : "N/A",
				LocalClientIsInGame() ? VariadicText("%i ms", ClientActive->iPing).c_str() : "N/A"));

			ImVec2 vWatermark(Eurostile_Extended->CalcTextSizeA(flEurostile_Extended, FLT_MAX, 0.0f, szWatermark.c_str()));

			ImGui::GetWindowDrawList()->AddText(Eurostile_Extended, flEurostile_Extended,
				ImVec2(vWatermark.y + 1.0f, flEurostile_Extended - vWatermark.y + 1.0f),
				0xFF000000, szWatermark.c_str());

			ImGui::GetWindowDrawList()->AddText(Eurostile_Extended, flEurostile_Extended,
				ImVec2(vWatermark.y, flEurostile_Extended - vWatermark.y),
				0xFFFFFFFF, szWatermark.c_str());

			std::string szFramesPerSecond(VariadicText("%i", (int)ImGui::GetIO().Framerate));
			ImVec2 vFramesPerSecond(Eurostile_Extended->CalcTextSizeA(flEurostile_Extended, FLT_MAX, 0.0f, szFramesPerSecond.c_str()));

			ImGui::GetWindowDrawList()->AddText(Eurostile_Extended, flEurostile_Extended,
				ImVec2(ImGui::GetIO().DisplaySize.x - vFramesPerSecond.x - vFramesPerSecond.y + 1.0f, flEurostile_Extended - vFramesPerSecond.y + 1.0f),
				0xFF000000, szFramesPerSecond.c_str());

			ImGui::GetWindowDrawList()->AddText(Eurostile_Extended, flEurostile_Extended,
				ImVec2(ImGui::GetIO().DisplaySize.x - vFramesPerSecond.x - vFramesPerSecond.y, flEurostile_Extended - vFramesPerSecond.y),
				0xFF00FFFF, szFramesPerSecond.c_str());

			ImGui::End();

			if (bShowWindow)
			{
				if (bWriteLog)
				{
					ImGui::LogToFile();
					bWriteLog = false;
				}

				ImGui::SetNextWindowSize(ImVec2(589.0f, 624.0f));
				ImGui::Begin("PROTOGENESYS", &bShowWindow, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
				ImGui::SetColorEditOptions(ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop);

				if (ImGui::TabLabels(_profiler.gMenuTabs->Domain.iMax, acut::StringVectorToCharPointerArray(_profiler.gMenuTabs->szItems), _profiler.gMenuTabs->Current.iValue, NULL, false, NULL, NULL, false, false, NULL, NULL, &ImVec2(81.0f, 25.0f)))
				{
					bWriteLog = true;
				}

				ImGui::BeginChild("ContentRegion", ImVec2(0.0f, 504.0f), true);

				switch (_profiler.gMenuTabs->Current.iValue)
				{
				case cProfiler::MENU_TAB_AIMBOT:
				{
					ImGui::NewLine();
					if (DrawOption(_profiler.gAimBotMode->szName, _profiler.gAimBotMode->szItems[_profiler.gAimBotMode->Current.iValue], &_profiler.gAimBotMode->Current.iValue, _profiler.gAimBotMode->Domain.iMin, _profiler.gAimBotMode->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gAutoZoom->szName, _profiler.gAutoZoom->szItems[_profiler.gAutoZoom->Current.iValue], &_profiler.gAutoZoom->Current.iValue, _profiler.gAutoZoom->Domain.iMin, _profiler.gAutoZoom->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gAutoFire->szName, _profiler.gAutoFire->szItems[_profiler.gAutoFire->Current.iValue], &_profiler.gAutoFire->Current.iValue, _profiler.gAutoFire->Domain.iMin, _profiler.gAutoFire->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gAutoWall->szName, _profiler.gAutoWall->szItems[_profiler.gAutoWall->Current.iValue], &_profiler.gAutoWall->Current.iValue, _profiler.gAutoWall->Domain.iMin, _profiler.gAutoWall->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gApplyPrediction->szName, _profiler.gApplyPrediction->szItems[_profiler.gApplyPrediction->Current.iValue], &_profiler.gApplyPrediction->Current.iValue, _profiler.gApplyPrediction->Domain.iMin, _profiler.gApplyPrediction->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gAntiTeamKill->szName, _profiler.gAntiTeamKill->szItems[_profiler.gAntiTeamKill->Current.iValue], &_profiler.gAntiTeamKill->Current.iValue, _profiler.gAntiTeamKill->Domain.iMin, _profiler.gAntiTeamKill->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gSilentAim->szName, _profiler.gSilentAim->szItems[_profiler.gSilentAim->Current.iValue], &_profiler.gSilentAim->Current.iValue, _profiler.gSilentAim->Domain.iMin, _profiler.gSilentAim->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gAntiAim->szName, _profiler.gAntiAim->szItems[_profiler.gAntiAim->Current.iValue], &_profiler.gAntiAim->Current.iValue, _profiler.gAntiAim->Domain.iMin, _profiler.gAntiAim->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gBoneScan->szName, _profiler.gBoneScan->szItems[_profiler.gBoneScan->Current.iValue], &_profiler.gBoneScan->Current.iValue, _profiler.gBoneScan->Domain.iMin, _profiler.gBoneScan->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gSortMethod->szName, _profiler.gSortMethod->szItems[_profiler.gSortMethod->Current.iValue], &_profiler.gSortMethod->Current.iValue, _profiler.gSortMethod->Domain.iMin, _profiler.gSortMethod->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();
				} break;

				case cProfiler::MENU_TAB_WALLHACK:
				{
					ImGui::NewLine();
					if (DrawOption(_profiler.gWallHackMode->szName, _profiler.gWallHackMode->szItems[_profiler.gWallHackMode->Current.iValue], &_profiler.gWallHackMode->Current.iValue, _profiler.gWallHackMode->Domain.iMin, _profiler.gWallHackMode->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gPlayerBoxes->szName, _profiler.gPlayerBoxes->szItems[_profiler.gPlayerBoxes->Current.iValue], &_profiler.gPlayerBoxes->Current.iValue, _profiler.gPlayerBoxes->Domain.iMin, _profiler.gPlayerBoxes->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gPlayerBones->szName, _profiler.gPlayerBones->szItems[_profiler.gPlayerBones->Current.iValue], &_profiler.gPlayerBones->Current.iValue, _profiler.gPlayerBones->Domain.iMin, _profiler.gPlayerBones->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gPlayerSnapLines->szName, _profiler.gPlayerSnapLines->szItems[_profiler.gPlayerSnapLines->Current.iValue], &_profiler.gPlayerSnapLines->Current.iValue, _profiler.gPlayerSnapLines->Domain.iMin, _profiler.gPlayerSnapLines->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gPlayerInfo->szName, _profiler.gPlayerInfo->szItems[_profiler.gPlayerInfo->Current.iValue], &_profiler.gPlayerInfo->Current.iValue, _profiler.gPlayerInfo->Domain.iMin, _profiler.gPlayerInfo->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gPlayerWeapons->szName, _profiler.gPlayerWeapons->szItems[_profiler.gPlayerWeapons->Current.iValue], &_profiler.gPlayerWeapons->Current.iValue, _profiler.gPlayerWeapons->Domain.iMin, _profiler.gPlayerWeapons->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gPlayerEntities->szName, _profiler.gPlayerEntities->szItems[_profiler.gPlayerEntities->Current.iValue], &_profiler.gPlayerEntities->Current.iValue, _profiler.gPlayerEntities->Domain.iMin, _profiler.gPlayerEntities->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gPlayerCrossHair->szName, _profiler.gPlayerCrossHair->szItems[_profiler.gPlayerCrossHair->Current.iValue], &_profiler.gPlayerCrossHair->Current.iValue, _profiler.gPlayerCrossHair->Domain.iMin, _profiler.gPlayerCrossHair->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gPlayerCompass->szName, _profiler.gPlayerCompass->szItems[_profiler.gPlayerCompass->Current.iValue], &_profiler.gPlayerCompass->Current.iValue, _profiler.gPlayerCompass->Domain.iMin, _profiler.gPlayerCompass->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gPlayerRadar->szName, _profiler.gPlayerRadar->szItems[_profiler.gPlayerRadar->Current.iValue], &_profiler.gPlayerRadar->Current.iValue, _profiler.gPlayerRadar->Domain.iMin, _profiler.gPlayerRadar->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gPlayerBulletTracers->szName, _profiler.gPlayerBulletTracers->szItems[_profiler.gPlayerBulletTracers->Current.iValue], &_profiler.gPlayerBulletTracers->Current.iValue, _profiler.gPlayerBulletTracers->Domain.iMin, _profiler.gPlayerBulletTracers->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();
				} break;

				case cProfiler::MENU_TAB_MISCELLANEOUS:
				{
					ImGui::NewLine();
					if (DrawOption(_profiler.gThirdPerson->szName, _profiler.gThirdPerson->szItems[_profiler.gThirdPerson->Current.iValue], &_profiler.gThirdPerson->Current.iValue, _profiler.gThirdPerson->Domain.iMin, _profiler.gThirdPerson->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gThirdPersonAntiAim->szName, _profiler.gThirdPersonAntiAim->szItems[_profiler.gThirdPersonAntiAim->Current.iValue], &_profiler.gThirdPersonAntiAim->Current.iValue, _profiler.gThirdPersonAntiAim->Domain.iMin, _profiler.gThirdPersonAntiAim->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gOrbitalVsat->szName, _profiler.gOrbitalVsat->szItems[_profiler.gOrbitalVsat->Current.iValue], &_profiler.gOrbitalVsat->Current.iValue, _profiler.gOrbitalVsat->Domain.iMin, _profiler.gOrbitalVsat->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gHardcoreHud->szName, _profiler.gHardcoreHud->szItems[_profiler.gHardcoreHud->Current.iValue], &_profiler.gHardcoreHud->Current.iValue, _profiler.gHardcoreHud->Domain.iMin, _profiler.gHardcoreHud->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gDisableEmp->szName, _profiler.gDisableEmp->szItems[_profiler.gDisableEmp->Current.iValue], &_profiler.gDisableEmp->Current.iValue, _profiler.gDisableEmp->Domain.iMin, _profiler.gDisableEmp->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gIdStealer->szName, _profiler.gIdStealer->szItems[_profiler.gIdStealer->Current.iValue], &_profiler.gIdStealer->Current.iValue, _profiler.gIdStealer->Domain.iMin, _profiler.gIdStealer->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gTrickShot->szName, _profiler.gTrickShot->szItems[_profiler.gTrickShot->Current.iValue], &_profiler.gTrickShot->Current.iValue, _profiler.gTrickShot->Domain.iMin, _profiler.gTrickShot->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();
				} break;

				case cProfiler::MENU_TAB_TWEAKS:
				{
					ImGui::NewLine();
					if (DrawOption(_profiler.gAimBone->szName, szBones[_profiler.gAimBone->Current.iValue].first, &_profiler.gAimBone->Current.iValue, _profiler.gAimBone->Domain.iMin, _profiler.gAimBone->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gAimAngle->szName, VariadicText("%d degrees", _profiler.gAimAngle->Current.iValue), &_profiler.gAimAngle->Current.iValue, _profiler.gAimAngle->Domain.iMin, _profiler.gAimAngle->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gAimPower->szName, VariadicText("%d percent", _profiler.gAimPower->Current.iValue), &_profiler.gAimPower->Current.iValue, _profiler.gAimPower->Domain.iMin, _profiler.gAimPower->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gAutoAimTime->szName, VariadicText("%d ms", _profiler.gAutoAimTime->Current.iValue), &_profiler.gAutoAimTime->Current.iValue, _profiler.gAutoAimTime->Domain.iMin, _profiler.gAutoAimTime->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gAutoAimDelay->szName, VariadicText("%d ms", _profiler.gAutoAimDelay->Current.iValue), &_profiler.gAutoAimDelay->Current.iValue, _profiler.gAutoAimDelay->Domain.iMin, _profiler.gAutoAimDelay->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gAutoZoomDelay->szName, VariadicText("%d ms", _profiler.gAutoZoomDelay->Current.iValue), &_profiler.gAutoZoomDelay->Current.iValue, _profiler.gAutoZoomDelay->Domain.iMin, _profiler.gAutoZoomDelay->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gAutoFireDelay->szName, VariadicText("%d ms", _profiler.gAutoFireDelay->Current.iValue), &_profiler.gAutoFireDelay->Current.iValue, _profiler.gAutoFireDelay->Domain.iMin, _profiler.gAutoFireDelay->Domain.iMax, 1))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gRecoilFactor->szName, VariadicText("%.1f", _profiler.gRecoilFactor->Current.flValue), &_profiler.gRecoilFactor->Current.flValue, _profiler.gRecoilFactor->Domain.flMin, _profiler.gRecoilFactor->Domain.flMax, 0.1f))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gSpreadFactor->szName, VariadicText("%.1f", _profiler.gSpreadFactor->Current.flValue), &_profiler.gSpreadFactor->Current.flValue, _profiler.gSpreadFactor->Domain.flMin, _profiler.gSpreadFactor->Domain.flMax, 0.1f))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					ImGui::Dummy(ImGui::GetContentRegionAvail() - ImVec2(0.0f, 35.0f + ImGui::GetStyle().ItemSpacing.y));
					if (ImGui::Button("Reset to Default", ImVec2(ImGui::GetWindowContentRegionWidth(), 35.0f)))
					{
						_profiler.gAimBone->Current.iValue = _profiler.gAimBone->Reset.iValue;
						_profiler.gAimAngle->Current.iValue = _profiler.gAimAngle->Reset.iValue;
						_profiler.gAimPower->Current.iValue = _profiler.gAimPower->Reset.iValue;
						_profiler.gAutoAimTime->Current.iValue = _profiler.gAutoAimTime->Reset.iValue;
						_profiler.gAutoAimDelay->Current.iValue = _profiler.gAutoAimDelay->Reset.iValue;
						_profiler.gAutoZoomDelay->Current.iValue = _profiler.gAutoZoomDelay->Reset.iValue;
						_profiler.gAutoFireDelay->Current.iValue = _profiler.gAutoFireDelay->Reset.iValue;
						_profiler.gRecoilFactor->Current.flValue = _profiler.gRecoilFactor->Reset.flValue;
						_profiler.gSpreadFactor->Current.flValue = _profiler.gSpreadFactor->Reset.flValue;

						bWriteLog = true;
					}
				} break;

				case cProfiler::MENU_TAB_STYLES:
				{
					ImGui::NewLine();

					if (DrawColorPicker(_profiler.gColorAxis->szName, _profiler.gColorAxis->Current.cValue))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawColorPicker(_profiler.gColorAllies->szName, _profiler.gColorAllies->Current.cValue))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawColorPicker(_profiler.gColorAccents->szName, _profiler.gColorAccents->Current.cValue))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawColorPicker(_profiler.gColorCrossHair->szName, _profiler.gColorCrossHair->Current.cValue))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawColorPicker(_profiler.gColorText->szName, _profiler.gColorText->Current.cValue))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawColorPicker(_profiler.gColorShadow->szName, _profiler.gColorShadow->Current.cValue))
					{
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gMenuColor->szName, _profiler.gMenuColor->szItems[_profiler.gMenuColor->Current.iValue], &_profiler.gMenuColor->Current.iValue, _profiler.gMenuColor->Domain.iMin, _profiler.gMenuColor->Domain.iMax, 1))
					{
						WritePrivateProfileString("MenuStyle", "COLOR", std::to_string(_profiler.gMenuColor->Current.iValue).c_str(), (acut::GetExeDirectory() + DEFAULT_CFG).c_str());

						bStyleChanged = true;
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gMenuCursor->szName, _profiler.gMenuCursor->szItems[_profiler.gMenuCursor->Current.iValue], &_profiler.gMenuCursor->Current.iValue, _profiler.gMenuCursor->Domain.iMin, _profiler.gMenuCursor->Domain.iMax, 1))
					{
						WritePrivateProfileString("MenuStyle", "CURSOR", std::to_string(_profiler.gMenuCursor->Current.iValue).c_str(), (acut::GetExeDirectory() + DEFAULT_CFG).c_str());

						bStyleChanged = true;
						bWriteLog = true;
					} ImGui::NewLine();

					if (DrawOption(_profiler.gMenuFont->szName, _profiler.gMenuFont->szItems[_profiler.gMenuFont->Current.iValue], &_profiler.gMenuFont->Current.iValue, _profiler.gMenuFont->Domain.iMin, _profiler.gMenuFont->Domain.iMax, 1))
					{
						WritePrivateProfileString("MenuStyle", "FONT", std::to_string(_profiler.gMenuFont->Current.iValue).c_str(), (acut::GetExeDirectory() + DEFAULT_CFG).c_str());

						bStyleChanged = true;
						bWriteLog = true;
					} ImGui::NewLine();

					ImGui::Dummy(ImGui::GetContentRegionAvail() - ImVec2(0.0f, 35.0f + ImGui::GetStyle().ItemSpacing.y));
					if (ImGui::Button("Reset to Default", ImVec2(ImGui::GetWindowContentRegionWidth(), 35.0f)))
					{
						_profiler.gColorAxis->Current.cValue = _profiler.gColorAxis->Reset.cValue;
						_profiler.gColorAllies->Current.cValue = _profiler.gColorAllies->Reset.cValue;
						_profiler.gColorAccents->Current.cValue = _profiler.gColorAccents->Reset.cValue;
						_profiler.gColorCrossHair->Current.cValue = _profiler.gColorCrossHair->Reset.cValue;
						_profiler.gColorText->Current.cValue = _profiler.gColorText->Reset.cValue;
						_profiler.gColorShadow->Current.cValue = _profiler.gColorShadow->Reset.cValue;

						bWriteLog = true;
					}
				} break;

				case cProfiler::MENU_TAB_PLAYERS:
				{
					ImGui::NewLine();
					for (int i = 0; i < MAX_CLIENTS; i++)
					{
						if (CG->Client[i].iInfoValid)
						{
							ImGui::PushID(i);

							if (ImGui::RadioButton("", &_targetList.iRiotShieldTarget, i))
							{
								bWriteLog = true;
							} ImGui::PopID(); ImGui::SameLine();

							ImGui::PushID(i + MAX_CLIENTS);

							if (ImGui::Checkbox("", (bool*)&_targetList.vIsTarget[i]))
							{
								bWriteLog = true;
							} ImGui::PopID(); ImGui::SameLine();

							ImGui::PushID(i + MAX_CLIENTS * 2);

							if (ImGui::Button("Add", ImVec2(50.0f, 0.0f)))
							{
								std::ofstream file(acut::GetExeDirectory() + DEFAULT_TXT, std::ios_base::out | std::ios_base::app);
								file << (std::to_string(CG->Client[i].qwXuid) + " " + CG->Client[i].szName).c_str() << std::endl;

								bWriteLog = true;
							} ImGui::PopID(); ImGui::SameLine();

							ImGui::PushItemWidth(150.0f);
							ImGui::InputText(VariadicText("%i: %s", CG->Client[i].iClientNum, CG->Client[i].szName).c_str(),
								(LPSTR)VariadicText("%u.%u.%u.%u",
									(BYTE)ServerSession[i].szIP[0],
									(BYTE)ServerSession[i].szIP[1],
									(BYTE)ServerSession[i].szIP[2],
									(BYTE)ServerSession[i].szIP[3]).c_str(),
								1024, ImGuiInputTextFlags_ReadOnly);
							ImGui::PopItemWidth();
						}

						else
						{
							_targetList.vIsTarget[i] = TRUE;
						}
					}
					ImGui::NewLine();
				} break;

				case cProfiler::MENU_TAB_CONSOLE:
				{
					_console.Init();
					_console.Draw();
				} break;
				}

				ImGui::EndChild();

				if (ImGui::Button("Set as Default", ImVec2(189.0f, 30.0f)))
				{
					_profiler.SaveProfile("");
					bWriteLog = true;
				} ImGui::SameLine(0.0f, 3.0f);

				static bool bSaveButton;
				if ((bSaveButton = ImGui::Button("Save Profile", ImVec2(189.0f, 30.0f))))
				{
					bWriteLog = true;
				} ImGui::SameLine(0.0f, 3.0f);
				LPCSTR szSavePath = SaveDialog.saveFileDialog(bSaveButton, SaveDialog.getLastDirectory(), NULL, ".xml", "Save Profile");

				if (strlen(szSavePath))
				{
					_profiler.SaveProfile(szSavePath);
				}

				static bool bLoadButton;
				if ((bLoadButton = ImGui::Button("Load Profile", ImVec2(189.0f, 30.0f))))
				{
					bWriteLog = true;
				}
				LPCSTR szLoadPath = LoadDialog.chooseFileDialog(bLoadButton, LoadDialog.getLastDirectory(), ".xml", "Load Profile");

				if (strlen(szLoadPath))
				{
					_profiler.LoadProfile(szLoadPath);
				}

				bool bReclaimFocus = false;
				ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth());
				if (ImGui::InputText("", _console.szInput, IM_ARRAYSIZE(_console.szInput), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory, &cConsole::TextEditCallbackStub, &_console))
				{
					LPSTR szInputEnd = _console.szInput + strlen(_console.szInput);

					while (szInputEnd > _console.szInput && szInputEnd[-1] == ' ')
					{
						szInputEnd--;
					} *szInputEnd = 0;

					if (_console.szInput[0])
						_console.ExecCommand(_console.szInput);

					ZeroMemory(_console.szInput, sizeof(_console.szInput));
					bReclaimFocus = true;

					_mainGui.bWriteLog = true;

					_profiler.gMenuTabs->Current.iValue = cProfiler::MENU_TAB_CONSOLE;
				}

				ImGui::PopItemWidth();
				ImGui::SetItemDefaultFocus();

				if (bReclaimFocus)
					ImGui::SetKeyboardFocusHere(-1);

				ImGui::End();
			}

			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			if (bStyleChanged)
			{
				RefreshInterface(_profiler.gMenuColor->Current.iValue, _profiler.gMenuCursor->Current.iValue, _profiler.gMenuFont->Current.iValue);
				bStyleChanged = false;
			}

			if (hWindow != *(HWND*)dwWindowHandle)
			{
				pDevice->Release();
				pDeviceContext->Release();

				ImGui_ImplWin32_Shutdown();
				ImGui_ImplDX11_Shutdown();
				ImGui::DestroyContext();

				bInitialized = false;
			}

			if (!LocalClientIsInGame())
			{
				for (auto& IsTarget : _targetList.vIsTarget)
					IsTarget = TRUE;
			}
		}
	}
	/*
	//=====================================================================================
	*/
	template<typename Type>
	bool cMainGUI::DrawOption(std::string label, std::string option, Type* value, Type min, Type max, Type step)
	{
		bool bReturn = false;

		ImGui::Text(label.c_str());
		ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - ImGui::GetFrameHeight() - (ImGui::GetStyle().ItemSpacing.x + ImGui::GetStyle().FramePadding.x) - 110.0f);

		if (ImGui::ArrowButtonEx(VariadicText("%s_left", label.c_str()).c_str(), ImGuiDir_Left, ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight()), ImGuiButtonFlags_Repeat | ImGuiButtonFlags_PressedOnClick))
		{
			*value -= step;

			if (*value < min)
				*value = min;

			bReturn = true;
		} ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - (ImGui::GetStyle().ItemSpacing.x + ImGui::GetStyle().FramePadding.x) - 55.0f - ImGui::GetIO().FontDefault->CalcTextSizeA(ImGui::GetIO().FontDefault->FontSize, FLT_MAX, 0.0f, option.c_str()).x / 2.0f);

		ImGui::Text(option.c_str());
		ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - (ImGui::GetStyle().ItemSpacing.x + ImGui::GetStyle().FramePadding.x));

		if (ImGui::ArrowButtonEx(VariadicText("%s_right", label.c_str()).c_str(), ImGuiDir_Right, ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight()), ImGuiButtonFlags_Repeat | ImGuiButtonFlags_PressedOnClick))
		{
			*value += step;

			if (*value > max)
				*value = max;

			bReturn = true;
		}

		return bReturn;
	}
	/*
	//=====================================================================================
	*/
	bool cMainGUI::DrawColorPicker(std::string label, ImVec4& color)
	{
		ImGui::Text(label.c_str());
		ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - (ImGui::GetStyle().ItemSpacing.x + ImGui::GetStyle().FramePadding.x) - 55.0f - ImGui::GetFrameHeight() / 2.0f);
		return _drawing.ColorPicker(label, color);
	}
}

//=====================================================================================