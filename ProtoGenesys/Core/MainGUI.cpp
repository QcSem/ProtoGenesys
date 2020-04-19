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

		LoadBackgroundImage();

		bInitialized = true;
	}
	/*
	//=====================================================================================
	*/
	void cMainGUI::LoadBackgroundImage()
	{
		HRESULT hResult = S_OK;

		HRSRC hResource;
		HGLOBAL hGlobal;
		LPVOID pResourceData;
		DWORD dwResourceSize;

		if (SUCCEEDED(hResult))
		{
			hResource = FindResource(hInstDll, MAKEINTRESOURCE(IDB_BACKGROUND), "PNG");
			hResult = (hResource ? S_OK : E_FAIL);
		}

		if (SUCCEEDED(hResult))
		{
			hGlobal = LoadResource(hInstDll, hResource);
			hResult = (hGlobal ? S_OK : E_FAIL);
		}

		if (SUCCEEDED(hResult))
		{
			pResourceData = LockResource(hGlobal);
			hResult = (pResourceData ? S_OK : E_FAIL);
		}

		if (SUCCEEDED(hResult))
		{
			dwResourceSize = SizeofResource(hInstDll, hResource);
			hResult = (dwResourceSize ? S_OK : E_FAIL);
		}

		if (SUCCEEDED(hResult))
		{
			CreateWICTextureFromMemory(pDevice, pDeviceContext, (uint8_t*)pResourceData, (size_t)dwResourceSize, &pD3D11Resource, &pD3D11ShaderResourceView);
			hResult = (pD3D11Resource && pD3D11ShaderResourceView ? S_OK : E_FAIL);
		}
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

				ImGui::GetWindowDrawList()->AddImage(pD3D11ShaderResourceView, ImGui::GetWindowPos(), ImGui::GetWindowPos() + ImGui::GetWindowSize());

				if (ImGui::TabLabels(_profiler.gMenuTabs->Domain.iMax, acut::StringVectorToCharPointerArray(_profiler.gMenuTabs->szItems), _profiler.gMenuTabs->Current.iValue, NULL, false, NULL, NULL, false, false, NULL, NULL, &ImVec2(94.0f, 25.0f)))
				{
					bProtoGenesysLog = true;
				}

				ImGui::BeginChild("ContentRegion", ImVec2(0.0f, 204.0f), true);
				ImGui::Separator();
				ImGui::NewLine();

				switch (_profiler.gMenuTabs->Current.iValue)
				{
				case cProfiler::MENU_TAB_AIMBOT:
				{
					if (ImGui::RadioButton(_profiler.gAimBotMode->szItems[cProfiler::AIMBOT_MODE_OFF].c_str(), &_profiler.gAimBotMode->Current.iValue, cProfiler::AIMBOT_MODE_OFF))
					{
						bProtoGenesysLog = true;
					} ImGui::SameLine(148.0f);

					if (ImGui::RadioButton(_profiler.gAimBotMode->szItems[cProfiler::AIMBOT_MODE_MANUAL].c_str(), &_profiler.gAimBotMode->Current.iValue, cProfiler::AIMBOT_MODE_MANUAL))
					{
						bProtoGenesysLog = true;
					} ImGui::SameLine(296.0f);

					if (ImGui::RadioButton(_profiler.gAimBotMode->szItems[cProfiler::AIMBOT_MODE_AUTO].c_str(), &_profiler.gAimBotMode->Current.iValue, cProfiler::AIMBOT_MODE_AUTO))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine(); ImGui::Separator(); ImGui::NewLine();

					if (ImGui::Checkbox(_profiler.gAutoZoom->szName.c_str(), &_profiler.gAutoZoom->Current.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::SameLine(296.0f);

					if (ImGui::Checkbox(_profiler.gAutoFire->szName.c_str(), &_profiler.gAutoFire->Current.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::Checkbox(_profiler.gAutoWall->szName.c_str(), &_profiler.gAutoWall->Current.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::SameLine(296.0f);

					if (ImGui::Checkbox(_profiler.gApplyPrediction->szName.c_str(), &_profiler.gApplyPrediction->Current.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::Checkbox(_profiler.gAntiTeamKill->szName.c_str(), &_profiler.gAntiTeamKill->Current.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::SameLine(296.0f);

					if (ImGui::Checkbox(_profiler.gSilentAim->szName.c_str(), &_profiler.gSilentAim->Current.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine(); ImGui::Separator(); ImGui::NewLine();

					if (ImGui::Combo(_profiler.gAntiAim->szName.c_str(), &_profiler.gAntiAim->Current.iValue, acut::StringVectorToCharPointerArray(_profiler.gAntiAim->szItems), _profiler.gAntiAim->Domain.iMax))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::Combo(_profiler.gBoneScan->szName.c_str(), &_profiler.gBoneScan->Current.iValue, acut::StringVectorToCharPointerArray(_profiler.gBoneScan->szItems), _profiler.gBoneScan->Domain.iMax))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::Combo(_profiler.gSortMethod->szName.c_str(), &_profiler.gSortMethod->Current.iValue, acut::StringVectorToCharPointerArray(_profiler.gSortMethod->szItems), _profiler.gSortMethod->Domain.iMax))
					{
						bProtoGenesysLog = true;
					}
				} break;

				case cProfiler::MENU_TAB_WALLHACK:
				{
					if (ImGui::RadioButton(_profiler.gWallHackMode->szItems[cProfiler::WALLHACK_MODE_AXIS].c_str(), &_profiler.gWallHackMode->Current.iValue, cProfiler::WALLHACK_MODE_AXIS))
					{
						bProtoGenesysLog = true;
					} ImGui::SameLine(148.0f);

					if (ImGui::RadioButton(_profiler.gWallHackMode->szItems[cProfiler::WALLHACK_MODE_ALLIES].c_str(), &_profiler.gWallHackMode->Current.iValue, cProfiler::WALLHACK_MODE_ALLIES))
					{
						bProtoGenesysLog = true;
					} ImGui::SameLine(296.0f);

					if (ImGui::RadioButton(_profiler.gWallHackMode->szItems[cProfiler::WALLHACK_MODE_ALL].c_str(), &_profiler.gWallHackMode->Current.iValue, cProfiler::WALLHACK_MODE_ALL))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine(); ImGui::Separator(); ImGui::NewLine();

					if (ImGui::Combo(_profiler.gPlayerBoxes->szName.c_str(), &_profiler.gPlayerBoxes->Current.iValue, acut::StringVectorToCharPointerArray(_profiler.gPlayerBoxes->szItems), _profiler.gPlayerBoxes->Domain.iMax))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::Combo(_profiler.gPlayerBones->szName.c_str(), &_profiler.gPlayerBones->Current.iValue, acut::StringVectorToCharPointerArray(_profiler.gPlayerBones->szItems), _profiler.gPlayerBones->Domain.iMax))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::Combo(_profiler.gPlayerSnapLines->szName.c_str(), &_profiler.gPlayerSnapLines->Current.iValue, acut::StringVectorToCharPointerArray(_profiler.gPlayerSnapLines->szItems), _profiler.gPlayerSnapLines->Domain.iMax))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine(); ImGui::Separator(); ImGui::NewLine();

					if (ImGui::Checkbox(_profiler.gPlayerInfo->szName.c_str(), &_profiler.gPlayerInfo->Current.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::SameLine(296.0f);

					if (ImGui::Checkbox(_profiler.gPlayerWeapons->szName.c_str(), &_profiler.gPlayerWeapons->Current.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::Checkbox(_profiler.gPlayerEntities->szName.c_str(), &_profiler.gPlayerEntities->Current.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::SameLine(296.0f);

					if (ImGui::Checkbox(_profiler.gPlayerCrossHair->szName.c_str(), &_profiler.gPlayerCrossHair->Current.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::Checkbox(_profiler.gPlayerCompass->szName.c_str(), &_profiler.gPlayerCompass->Current.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::SameLine(296.0f);

					if (ImGui::Checkbox(_profiler.gPlayerRadar->szName.c_str(), &_profiler.gPlayerRadar->Current.bValue))
					{
						bProtoGenesysLog = true;
					}
				} break;

				case cProfiler::MENU_TAB_MISCELLANEOUS:
				{
					if (ImGui::Checkbox(_profiler.gThirdPerson->szName.c_str(), &_profiler.gThirdPerson->Current.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::SameLine(296.0f);

					if (ImGui::Checkbox(_profiler.gOrbitalVsat->szName.c_str(), &_profiler.gOrbitalVsat->Current.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::Checkbox(_profiler.gHardcoreHud->szName.c_str(), &_profiler.gHardcoreHud->Current.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::SameLine(296.0f);

					if (ImGui::Checkbox(_profiler.gDisableEmp->szName.c_str(), &_profiler.gDisableEmp->Current.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::Checkbox(_profiler.gHostAutoWall->szName.c_str(), &_profiler.gHostAutoWall->Current.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::SameLine(296.0f);

					if (ImGui::Checkbox(_profiler.gIdStealer->szName.c_str(), &_profiler.gIdStealer->Current.bValue))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::Checkbox(_profiler.gTrickShot->szName.c_str(), &_profiler.gTrickShot->Current.bValue))
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
					if (ImGui::SliderInt(_profiler.gAimBone->szName.c_str(), &_profiler.gAimBone->Current.iValue, _profiler.gAimBone->Domain.iMin, _profiler.gAimBone->Domain.iMax, szBones[_profiler.gAimBone->Current.iValue].first.c_str()))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::SliderFloat(_profiler.gAimAngle->szName.c_str(), &_profiler.gAimAngle->Current.flValue, _profiler.gAimAngle->Domain.flMin, _profiler.gAimAngle->Domain.flMax, "%.0f degrees"))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::SliderInt(_profiler.gAimPower->szName.c_str(), &_profiler.gAimPower->Current.iValue, _profiler.gAimPower->Domain.iMin, _profiler.gAimPower->Domain.iMax, "%d%%"))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::SliderInt(_profiler.gAutoAimTime->szName.c_str(), &_profiler.gAutoAimTime->Current.iValue, _profiler.gAutoAimTime->Domain.iMin, _profiler.gAutoAimTime->Domain.iMax, "%d ms"))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::SliderInt(_profiler.gAutoAimDelay->szName.c_str(), &_profiler.gAutoAimDelay->Current.iValue, _profiler.gAutoAimDelay->Domain.iMin, _profiler.gAutoAimDelay->Domain.iMax, "%d ms"))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::SliderInt(_profiler.gAutoZoomDelay->szName.c_str(), &_profiler.gAutoZoomDelay->Current.iValue, _profiler.gAutoZoomDelay->Domain.iMin, _profiler.gAutoZoomDelay->Domain.iMax, "%d ms"))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::SliderInt(_profiler.gAutoFireDelay->szName.c_str(), &_profiler.gAutoFireDelay->Current.iValue, _profiler.gAutoFireDelay->Domain.iMin, _profiler.gAutoFireDelay->Domain.iMax, "%d ms"))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::SliderFloat(_profiler.gRecoilFactor->szName.c_str(), &_profiler.gRecoilFactor->Current.flValue, _profiler.gRecoilFactor->Domain.flMin, _profiler.gRecoilFactor->Domain.flMax))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					if (ImGui::SliderFloat(_profiler.gSpreadFactor->szName.c_str(), &_profiler.gSpreadFactor->Current.flValue, _profiler.gSpreadFactor->Domain.flMin, _profiler.gSpreadFactor->Domain.flMax))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine(); ImGui::Separator(); ImGui::NewLine();

					if (ImGui::Button("Reset to Default", ImVec2(446.0f, 35.0f)))
					{
						_profiler.gAimBone->Current.iValue = _profiler.gAimBone->Reset.iValue;
						_profiler.gAimAngle->Current.flValue = _profiler.gAimAngle->Reset.flValue;
						_profiler.gAimPower->Current.iValue = _profiler.gAimPower->Reset.iValue;
						_profiler.gAutoAimTime->Current.iValue = _profiler.gAutoAimTime->Reset.iValue;
						_profiler.gAutoAimDelay->Current.iValue = _profiler.gAutoAimDelay->Reset.iValue;
						_profiler.gAutoZoomDelay->Current.iValue = _profiler.gAutoZoomDelay->Reset.iValue;
						_profiler.gAutoFireDelay->Current.iValue = _profiler.gAutoFireDelay->Reset.iValue;
						_profiler.gRecoilFactor->Current.flValue = _profiler.gRecoilFactor->Reset.flValue;
						_profiler.gSpreadFactor->Current.flValue = _profiler.gSpreadFactor->Reset.flValue;

						bProtoGenesysLog = true;
					}
				} break;

				case cProfiler::MENU_TAB_STYLES:
				{
					_drawing.ColorPicker(_profiler.gColorAxis->szName, _profiler.gColorAxis->Current.cValue);
					ImGui::SameLine(0.0f, 4.0f);

					if (ImGui::ColorEdit4(_profiler.gColorAxis->szName.c_str(), _profiler.gColorAxis->Current.cValue))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					_drawing.ColorPicker(_profiler.gColorAllies->szName, _profiler.gColorAllies->Current.cValue);
					ImGui::SameLine(0.0f, 4.0f);

					if (ImGui::ColorEdit4(_profiler.gColorAllies->szName.c_str(), _profiler.gColorAllies->Current.cValue))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					_drawing.ColorPicker(_profiler.gColorRiotShield->szName, _profiler.gColorRiotShield->Current.cValue);
					ImGui::SameLine(0.0f, 4.0f);

					if (ImGui::ColorEdit4(_profiler.gColorRiotShield->szName.c_str(), _profiler.gColorRiotShield->Current.cValue))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					_drawing.ColorPicker(_profiler.gColorCrossHair->szName, _profiler.gColorCrossHair->Current.cValue);
					ImGui::SameLine(0.0f, 4.0f);

					if (ImGui::ColorEdit4(_profiler.gColorCrossHair->szName.c_str(), _profiler.gColorCrossHair->Current.cValue))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					_drawing.ColorPicker(_profiler.gColorText->szName, _profiler.gColorText->Current.cValue);
					ImGui::SameLine(0.0f, 4.0f);

					if (ImGui::ColorEdit4(_profiler.gColorText->szName.c_str(), _profiler.gColorText->Current.cValue))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine();

					_drawing.ColorPicker(_profiler.gColorShadow->szName, _profiler.gColorShadow->Current.cValue);
					ImGui::SameLine(0.0f, 4.0f);

					if (ImGui::ColorEdit4(_profiler.gColorShadow->szName.c_str(), _profiler.gColorShadow->Current.cValue))
					{
						bProtoGenesysLog = true;
					} ImGui::NewLine(); ImGui::Separator(); ImGui::NewLine();

					if (ImGui::Combo(_profiler.gMenuColor->szName.c_str(), &_profiler.gMenuColor->Current.iValue, acut::StringVectorToCharPointerArray(_profiler.gMenuColor->szItems), _profiler.gMenuColor->Domain.iMax))
					{
						WritePrivateProfileString("MenuStyle", "COLOR", std::to_string(_profiler.gMenuColor->Current.iValue).c_str(), (acut::GetExeDirectory() + DEFAULT_CFG).c_str());

						bStyleChanged = true;
						bProtoGenesysLog = true;
					}

					if (ImGui::Combo(_profiler.gMenuCursor->szName.c_str(), &_profiler.gMenuCursor->Current.iValue, acut::StringVectorToCharPointerArray(_profiler.gMenuCursor->szItems), _profiler.gMenuCursor->Domain.iMax))
					{
						WritePrivateProfileString("MenuStyle", "CURSOR", std::to_string(_profiler.gMenuCursor->Current.iValue).c_str(), (acut::GetExeDirectory() + DEFAULT_CFG).c_str());

						bStyleChanged = true;
						bProtoGenesysLog = true;
					}

					if (ImGui::Combo(_profiler.gMenuFont->szName.c_str(), &_profiler.gMenuFont->Current.iValue, acut::StringVectorToCharPointerArray(_profiler.gMenuFont->szItems), _profiler.gMenuFont->Domain.iMax))
					{
						WritePrivateProfileString("MenuStyle", "FONT", std::to_string(_profiler.gMenuFont->Current.iValue).c_str(), (acut::GetExeDirectory() + DEFAULT_CFG).c_str());

						bStyleChanged = true;
						bProtoGenesysLog = true;
					} ImGui::NewLine(); ImGui::Separator(); ImGui::NewLine();

					if (ImGui::Button("Reset to Default", ImVec2(446.0f, 35.0f)))
					{
						_profiler.gColorAxis->Current.cValue = _profiler.gColorAxis->Reset.cValue;
						_profiler.gColorAllies->Current.cValue = _profiler.gColorAllies->Reset.cValue;
						_profiler.gColorRiotShield->Current.cValue = _profiler.gColorRiotShield->Reset.cValue;
						_profiler.gColorCrossHair->Current.cValue = _profiler.gColorCrossHair->Reset.cValue;
						_profiler.gColorText->Current.cValue = _profiler.gColorText->Reset.cValue;
						_profiler.gColorShadow->Current.cValue = _profiler.gColorShadow->Reset.cValue;

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
}

//=====================================================================================