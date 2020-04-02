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

		_profiler.gMenuColor->Custom.iValue = GetPrivateProfileInt("MenuStyle", "COLOR", cProfiler::MENU_COLOR_NEUTRAL, (acut::GetExeDirectory() + DEFAULT_CFG).c_str());
		_profiler.gMenuCursor->Custom.iValue = GetPrivateProfileInt("MenuStyle", "CURSOR", cProfiler::MENU_CURSOR_BLACK, (acut::GetExeDirectory() + DEFAULT_CFG).c_str());
		_profiler.gMenuFont->Custom.iValue = GetPrivateProfileInt("MenuStyle", "FONT", cProfiler::MENU_FONT_LIGHT, (acut::GetExeDirectory() + DEFAULT_CFG).c_str());

		RefreshInterface(_profiler.gMenuColor->Custom.iValue, _profiler.gMenuCursor->Custom.iValue, _profiler.gMenuFont->Custom.iValue);

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
			bShowProtoGenesys = !bShowProtoGenesys;

		if (GetKeyPress(VK_HOME, false))
			_profiler.LoadProfile("");

		if (GetKeyPress(VK_END, false))
			_profiler.DisableAll();

		*(bool*)dwMouseInput = !bShowProtoGenesys;

		if (bInitialized && bShowProtoGenesys && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
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
			if (_profiler.gMenuColor->Custom.iValue == cProfiler::MENU_COLOR_RAINBOW || _profiler.gMenuColor->Custom.iValue == cProfiler::MENU_COLOR_RAINBOW_NEON)
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

			ImGui::GetIO().MouseDrawCursor = bShowProtoGenesys;

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
			}

			std::string szWatermark(VariadicText("PROTOGENESYS - COD BO2 by: InUrFace | Frametime: %s, Ping: %s",
				LocalClientIsInGame() ? VariadicText("%i ms", *(int*)(dwCG + 0x48088)).c_str() : "N/A",
				LocalClientIsInGame() ? VariadicText("%i ms", *(int*)(*(DWORD_PTR*)dwClientActive + 0x68)).c_str() : "N/A"));

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

			if (bShowProtoGenesys)
			{
				if (bProtoGenesysLog)
				{
					ImGui::LogToFile();
					bProtoGenesysLog = false;
				}

				ImGui::SetNextWindowSize(ImVec2(490.0f, 324.0f));
				ImGui::Begin("PROTOGENESYS", &bShowProtoGenesys, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
				ImGui::SetColorEditOptions(ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop);

				if (ImGui::TabLabels(_profiler.gMenuTabs->MaxValue.iMax, acut::StringVectorToCharPointerArray(_profiler.gMenuTabs->szItems), _profiler.gMenuTabs->Custom.iValue, NULL, false, NULL, NULL, false, false, NULL, NULL, &ImVec2(94.0f, 25.0f)))
				{
					bProtoGenesysLog = true;
				}

				ImGui::BeginChild("ContentRegion", ImVec2(0.0f, 204.0f), true);
				ImGui::Separator();
				ImGui::NewLine();

				switch (_profiler.gMenuTabs->Custom.iValue)
				{
				case cProfiler::MENU_TAB_AIMBOT:
				{
					if (ImGui::RadioButton(_profiler.gAimBotMode->szItems[cProfiler::AIMBOT_MODE_OFF].c_str(), &_profiler.gAimBotMode->Custom.iValue, cProfiler::AIMBOT_MODE_OFF))
					{
						bProtoGenesysLog = true;
					} ImGui::SameLine(148.0f);

					if (ImGui::RadioButton(_profiler.gAimBotMode->szItems[cProfiler::AIMBOT_MODE_MANUAL].c_str(), &_profiler.gAimBotMode->Custom.iValue, cProfiler::AIMBOT_MODE_MANUAL))
					{
						bProtoGenesysLog = true;
					} ImGui::SameLine(296.0f);

					if (ImGui::RadioButton(_profiler.gAimBotMode->szItems[cProfiler::AIMBOT_MODE_AUTO].c_str(), &_profiler.gAimBotMode->Custom.iValue, cProfiler::AIMBOT_MODE_AUTO))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine(); ImGui::Separator(); ImGui::NewLine();

					if (ImGui::Checkbox(_profiler.gAutoZoom->szLabel.c_str(), &_profiler.gAutoZoom->Custom.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::SameLine(296.0f);

					if (ImGui::Checkbox(_profiler.gAutoFire->szLabel.c_str(), &_profiler.gAutoFire->Custom.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::Checkbox(_profiler.gAutoWall->szLabel.c_str(), &_profiler.gAutoWall->Custom.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::SameLine(296.0f);

					if (ImGui::Checkbox(_profiler.gApplyPrediction->szLabel.c_str(), &_profiler.gApplyPrediction->Custom.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::Checkbox(_profiler.gAntiTeamKill->szLabel.c_str(), &_profiler.gAntiTeamKill->Custom.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::SameLine(296.0f);

					if (ImGui::Checkbox(_profiler.gSilentAim->szLabel.c_str(), &_profiler.gSilentAim->Custom.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine(); ImGui::Separator(); ImGui::NewLine();

					if (ImGui::Combo(_profiler.gAntiAim->szLabel.c_str(), &_profiler.gAntiAim->Custom.iValue, acut::StringVectorToCharPointerArray(_profiler.gAntiAim->szItems), _profiler.gAntiAim->MaxValue.iMax))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::Combo(_profiler.gBoneScan->szLabel.c_str(), &_profiler.gBoneScan->Custom.iValue, acut::StringVectorToCharPointerArray(_profiler.gBoneScan->szItems), _profiler.gBoneScan->MaxValue.iMax))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::Combo(_profiler.gSortMethod->szLabel.c_str(), &_profiler.gSortMethod->Custom.iValue, acut::StringVectorToCharPointerArray(_profiler.gSortMethod->szItems), _profiler.gSortMethod->MaxValue.iMax))
					{
						bProtoGenesysLog = true;
					}
				} break;

				case cProfiler::MENU_TAB_WALLHACK:
				{
					if (ImGui::RadioButton(_profiler.gWallHackMode->szItems[cProfiler::WALLHACK_MODE_AXIS].c_str(), &_profiler.gWallHackMode->Custom.iValue, cProfiler::WALLHACK_MODE_AXIS))
					{
						bProtoGenesysLog = true;
					} ImGui::SameLine(148.0f);

					if (ImGui::RadioButton(_profiler.gWallHackMode->szItems[cProfiler::WALLHACK_MODE_ALLIES].c_str(), &_profiler.gWallHackMode->Custom.iValue, cProfiler::WALLHACK_MODE_ALLIES))
					{
						bProtoGenesysLog = true;
					} ImGui::SameLine(296.0f);

					if (ImGui::RadioButton(_profiler.gWallHackMode->szItems[cProfiler::WALLHACK_MODE_ALL].c_str(), &_profiler.gWallHackMode->Custom.iValue, cProfiler::WALLHACK_MODE_ALL))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine(); ImGui::Separator(); ImGui::NewLine();

					if (ImGui::Combo(_profiler.gPlayerBoxes->szLabel.c_str(), &_profiler.gPlayerBoxes->Custom.iValue, acut::StringVectorToCharPointerArray(_profiler.gPlayerBoxes->szItems), _profiler.gPlayerBoxes->MaxValue.iMax))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::Combo(_profiler.gPlayerBones->szLabel.c_str(), &_profiler.gPlayerBones->Custom.iValue, acut::StringVectorToCharPointerArray(_profiler.gPlayerBones->szItems), _profiler.gPlayerBones->MaxValue.iMax))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::Combo(_profiler.gPlayerSnapLines->szLabel.c_str(), &_profiler.gPlayerSnapLines->Custom.iValue, acut::StringVectorToCharPointerArray(_profiler.gPlayerSnapLines->szItems), _profiler.gPlayerSnapLines->MaxValue.iMax))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine(); ImGui::Separator(); ImGui::NewLine();

					if (ImGui::Checkbox(_profiler.gPlayerInfo->szLabel.c_str(), &_profiler.gPlayerInfo->Custom.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::SameLine(296.0f);

					if (ImGui::Checkbox(_profiler.gPlayerWeapons->szLabel.c_str(), &_profiler.gPlayerWeapons->Custom.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::Checkbox(_profiler.gPlayerEntities->szLabel.c_str(), &_profiler.gPlayerEntities->Custom.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::SameLine(296.0f);

					if (ImGui::Checkbox(_profiler.gPlayerCrossHair->szLabel.c_str(), &_profiler.gPlayerCrossHair->Custom.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::Checkbox(_profiler.gPlayerCompass->szLabel.c_str(), &_profiler.gPlayerCompass->Custom.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::SameLine(296.0f);

					if (ImGui::Checkbox(_profiler.gPlayerRadar->szLabel.c_str(), &_profiler.gPlayerRadar->Custom.bValue))
					{
						bProtoGenesysLog = true;
					}
				} break;

				case cProfiler::MENU_TAB_MISCELLANEOUS:
				{
					if (ImGui::Checkbox(_profiler.gThirdPerson->szLabel.c_str(), &_profiler.gThirdPerson->Custom.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::SameLine(296.0f);

					if (ImGui::Checkbox(_profiler.gThirdPersonAntiAim->szLabel.c_str(), &_profiler.gThirdPersonAntiAim->Custom.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::Checkbox(_profiler.gOrbitalVsat->szLabel.c_str(), &_profiler.gOrbitalVsat->Custom.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::SameLine(296.0f);

					if (ImGui::Checkbox(_profiler.gHardcoreHud->szLabel.c_str(), &_profiler.gHardcoreHud->Custom.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::Checkbox(_profiler.gDisableEmp->szLabel.c_str(), &_profiler.gDisableEmp->Custom.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::SameLine(296.0f);

					if (ImGui::Checkbox(_profiler.gNameStealer->szLabel.c_str(), &_profiler.gNameStealer->Custom.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::Checkbox(_profiler.gTrickShot->szLabel.c_str(), &_profiler.gTrickShot->Custom.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::SameLine(296.0f);

					if (ImGui::Checkbox("Console", &bShowConsole))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::Checkbox("Player List", &bShowPlayerList))
					{
						bProtoGenesysLog = true;
					} ImGui::SameLine(296.0f);

					if (ImGui::Checkbox("Memory Editor", &bShowMemoryEditor))
					{
						bProtoGenesysLog = true;
					}
				} break;

				case cProfiler::MENU_TAB_TWEAKS:
				{
					if (ImGui::SliderInt(_profiler.gAimBone->szLabel.c_str(), &_profiler.gAimBone->Custom.iValue, _profiler.gAimBone->MinValue.iMin, _profiler.gAimBone->MaxValue.iMax, szBones[_profiler.gAimBone->Custom.iValue].first.c_str()))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::SliderFloat(_profiler.gAimAngle->szLabel.c_str(), &_profiler.gAimAngle->Custom.flValue, _profiler.gAimAngle->MinValue.flMin, _profiler.gAimAngle->MaxValue.flMax, "%.0f degrees"))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::SliderInt(_profiler.gAimPower->szLabel.c_str(), &_profiler.gAimPower->Custom.iValue, _profiler.gAimPower->MinValue.iMin, _profiler.gAimPower->MaxValue.iMax, "%d%%"))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::SliderInt(_profiler.gAutoAimTime->szLabel.c_str(), &_profiler.gAutoAimTime->Custom.iValue, _profiler.gAutoAimTime->MinValue.iMin, _profiler.gAutoAimTime->MaxValue.iMax, "%d ms"))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::SliderInt(_profiler.gAutoAimDelay->szLabel.c_str(), &_profiler.gAutoAimDelay->Custom.iValue, _profiler.gAutoAimDelay->MinValue.iMin, _profiler.gAutoAimDelay->MaxValue.iMax, "%d ms"))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::SliderInt(_profiler.gAutoZoomDelay->szLabel.c_str(), &_profiler.gAutoZoomDelay->Custom.iValue, _profiler.gAutoZoomDelay->MinValue.iMin, _profiler.gAutoZoomDelay->MaxValue.iMax, "%d ms"))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::SliderInt(_profiler.gAutoFireDelay->szLabel.c_str(), &_profiler.gAutoFireDelay->Custom.iValue, _profiler.gAutoFireDelay->MinValue.iMin, _profiler.gAutoFireDelay->MaxValue.iMax, "%d ms"))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::SliderFloat(_profiler.gRecoilFactor->szLabel.c_str(), &_profiler.gRecoilFactor->Custom.flValue, _profiler.gRecoilFactor->MinValue.flMin, _profiler.gRecoilFactor->MaxValue.flMax))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::SliderFloat(_profiler.gSpreadFactor->szLabel.c_str(), &_profiler.gSpreadFactor->Custom.flValue, _profiler.gSpreadFactor->MinValue.flMin, _profiler.gSpreadFactor->MaxValue.flMax))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine(); ImGui::Separator(); ImGui::NewLine();

					if (ImGui::Button("Reset to Default", ImVec2(446.0f, 35.0f)))
					{
						_profiler.gAimBone->Custom.iValue = _profiler.gAimBone->Default.iValue;
						_profiler.gAimAngle->Custom.flValue = _profiler.gAimAngle->Default.flValue;
						_profiler.gAimPower->Custom.iValue = _profiler.gAimPower->Default.iValue;
						_profiler.gAutoAimTime->Custom.iValue = _profiler.gAutoAimTime->Default.iValue;
						_profiler.gAutoAimDelay->Custom.iValue = _profiler.gAutoAimDelay->Default.iValue;
						_profiler.gAutoZoomDelay->Custom.iValue = _profiler.gAutoZoomDelay->Default.iValue;
						_profiler.gAutoFireDelay->Custom.iValue = _profiler.gAutoFireDelay->Default.iValue;
						_profiler.gRecoilFactor->Custom.flValue = _profiler.gRecoilFactor->Default.flValue;
						_profiler.gSpreadFactor->Custom.flValue = _profiler.gSpreadFactor->Default.flValue;

						bProtoGenesysLog = true;
					}
				} break;

				case cProfiler::MENU_TAB_STYLES:
				{
					_drawing.ColorPicker(_profiler.gColorAxis->szLabel, _profiler.gColorAxis->Custom.cValue);
					ImGui::SameLine(0.0f, 4.0f);

					if (ImGui::ColorEdit4(_profiler.gColorAxis->szLabel.c_str(), _profiler.gColorAxis->Custom.cValue))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					_drawing.ColorPicker(_profiler.gColorAllies->szLabel, _profiler.gColorAllies->Custom.cValue);
					ImGui::SameLine(0.0f, 4.0f);

					if (ImGui::ColorEdit4(_profiler.gColorAllies->szLabel.c_str(), _profiler.gColorAllies->Custom.cValue))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					_drawing.ColorPicker(_profiler.gColorRiotShield->szLabel, _profiler.gColorRiotShield->Custom.cValue);
					ImGui::SameLine(0.0f, 4.0f);

					if (ImGui::ColorEdit4(_profiler.gColorRiotShield->szLabel.c_str(), _profiler.gColorRiotShield->Custom.cValue))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					_drawing.ColorPicker(_profiler.gColorCrossHair->szLabel, _profiler.gColorCrossHair->Custom.cValue);
					ImGui::SameLine(0.0f, 4.0f);

					if (ImGui::ColorEdit4(_profiler.gColorCrossHair->szLabel.c_str(), _profiler.gColorCrossHair->Custom.cValue))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					_drawing.ColorPicker(_profiler.gColorText->szLabel, _profiler.gColorText->Custom.cValue);
					ImGui::SameLine(0.0f, 4.0f);

					if (ImGui::ColorEdit4(_profiler.gColorText->szLabel.c_str(), _profiler.gColorText->Custom.cValue))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					_drawing.ColorPicker(_profiler.gColorShadow->szLabel, _profiler.gColorShadow->Custom.cValue);
					ImGui::SameLine(0.0f, 4.0f);

					if (ImGui::ColorEdit4(_profiler.gColorShadow->szLabel.c_str(), _profiler.gColorShadow->Custom.cValue))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine(); ImGui::Separator(); ImGui::NewLine();

					if (ImGui::Combo(_profiler.gMenuColor->szLabel.c_str(), &_profiler.gMenuColor->Custom.iValue, acut::StringVectorToCharPointerArray(_profiler.gMenuColor->szItems), _profiler.gMenuColor->MaxValue.iMax))
					{
						WritePrivateProfileString("MenuStyle", "COLOR", std::to_string(_profiler.gMenuColor->Custom.iValue).c_str(), (acut::GetExeDirectory() + DEFAULT_CFG).c_str());

						bStyleChanged = true;
						bProtoGenesysLog = true;
					}

					if (ImGui::Combo(_profiler.gMenuCursor->szLabel.c_str(), &_profiler.gMenuCursor->Custom.iValue, acut::StringVectorToCharPointerArray(_profiler.gMenuCursor->szItems), _profiler.gMenuCursor->MaxValue.iMax))
					{
						WritePrivateProfileString("MenuStyle", "CURSOR", std::to_string(_profiler.gMenuCursor->Custom.iValue).c_str(), (acut::GetExeDirectory() + DEFAULT_CFG).c_str());

						bStyleChanged = true;
						bProtoGenesysLog = true;
					}

					if (ImGui::Combo(_profiler.gMenuFont->szLabel.c_str(), &_profiler.gMenuFont->Custom.iValue, acut::StringVectorToCharPointerArray(_profiler.gMenuFont->szItems), _profiler.gMenuFont->MaxValue.iMax))
					{
						WritePrivateProfileString("MenuStyle", "FONT", std::to_string(_profiler.gMenuFont->Custom.iValue).c_str(), (acut::GetExeDirectory() + DEFAULT_CFG).c_str());

						bStyleChanged = true;
						bProtoGenesysLog = true;
					} ImGui::NewLine(); ImGui::Separator(); ImGui::NewLine();

					if (ImGui::Button("Reset to Default", ImVec2(446.0f, 35.0f)))
					{
						_profiler.gColorAxis->Custom.cValue = _profiler.gColorAxis->Default.cValue;
						_profiler.gColorAllies->Custom.cValue = _profiler.gColorAllies->Default.cValue;
						_profiler.gColorRiotShield->Custom.cValue = _profiler.gColorRiotShield->Default.cValue;
						_profiler.gColorCrossHair->Custom.cValue = _profiler.gColorCrossHair->Default.cValue;
						_profiler.gColorText->Custom.cValue = _profiler.gColorText->Default.cValue;
						_profiler.gColorShadow->Custom.cValue = _profiler.gColorShadow->Default.cValue;

						bProtoGenesysLog = true;
					}
				} break;
				}

				ImGui::NewLine();
				ImGui::Separator();
				ImGui::EndChild();

				if (ImGui::Button("Set as Default", ImVec2(156.0f, 30.0f)))
				{
					_profiler.SaveProfile("");
					bProtoGenesysLog = true;
				} ImGui::SameLine(0.0f, 3.0f);

				static bool bSaveButton;
				if ((bSaveButton = ImGui::Button("Save Profile", ImVec2(156.0f, 30.0f))))
				{
					bProtoGenesysLog = true;
				} ImGui::SameLine(0.0f, 3.0f);
				LPCSTR szSavePath = SaveDialog.saveFileDialog(bSaveButton, SaveDialog.getLastDirectory(), NULL, ".xml", "Save Profile");

				if (strlen(szSavePath))
				{
					_profiler.SaveProfile(szSavePath);
					strcpy_s(szProfilePath, szSavePath);
				}

				static bool bLoadButton;
				if ((bLoadButton = ImGui::Button("Load Profile", ImVec2(156.0f, 30.0f))))
				{
					bProtoGenesysLog = true;
				}
				LPCSTR szLoadPath = LoadDialog.chooseFileDialog(bLoadButton, LoadDialog.getLastDirectory(), ".xml", "Load Profile");

				if (strlen(szLoadPath))
				{
					_profiler.LoadProfile(szLoadPath);
					strcpy_s(szProfilePath, szLoadPath);
				}

				ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth());
				ImGui::InputText("", szProfilePath, IM_ARRAYSIZE(szProfilePath), ImGuiInputTextFlags_ReadOnly);
				ImGui::PopItemWidth();
				ImGui::End();

				if (bShowMemoryEditor)
				{
					MemEdit.DrawWindow("MEMORY EDITOR", &bShowMemoryEditor, hT6mp.lpBaseOfDll, hT6mp.SizeOfImage, (size_t)hT6mp.lpBaseOfDll);
				}

				if (bShowConsole)
				{
					_console.Init();
					_console.Draw("CONSOLE", &bShowConsole);
				}

				if (bShowPlayerList && LocalClientIsInGame())
				{
					if (bPlayerListLog)
					{
						ImGui::LogToFile();
						bPlayerListLog = false;
					}

					ImGui::SetNextWindowSize(ImVec2(400.0f, 480.0f));
					ImGui::Begin("PLAYER LIST", &bShowPlayerList, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
					ImGui::BeginChild("ContentRegion", ImVec2(0.0f, 0.0f), true);

					for (int i = 0; i < MAX_CLIENTS; i++)
					{
						if (CG->Client[i].iInfoValid)
						{
							ImGui::PushID(i);

							if (ImGui::RadioButton("", &_targetList.iRiotShieldTarget, i))
							{
								bPlayerListLog = true;
							} ImGui::PopID(); ImGui::SameLine();

							ImGui::PushID(i + MAX_CLIENTS);

							if (ImGui::Checkbox("", (bool*)&_targetList.vIsTarget[i]))
							{
								bPlayerListLog = true;
							} ImGui::PopID(); ImGui::SameLine();

							ImGui::PushID(i + MAX_CLIENTS * 2);

							if (ImGui::Button("Add", ImVec2(50.0f, 0.0f)))
							{
								std::ofstream file(acut::GetExeDirectory() + DEFAULT_TXT, std::ios_base::out | std::ios_base::app);
								file << (std::to_string(CG->Client[i].qwXuid) + " " + CG->Client[i].szName).c_str() << std::endl;

								bPlayerListLog = true;
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

					ImGui::EndChild();
					ImGui::End();
				}
			}

			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			if (bStyleChanged)
			{
				RefreshInterface(_profiler.gMenuColor->Custom.iValue, _profiler.gMenuCursor->Custom.iValue, _profiler.gMenuFont->Custom.iValue);
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
		}
	}
}

//=====================================================================================