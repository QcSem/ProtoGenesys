//=====================================================================================

#include "../StdAfx.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	cMainGUI _mainGui;

	void cMainGUI::InitInterface()
	{
		hWindow = Dereference((HWND)dwWindowHandle);

		if (!hWindow || !_device || !_deviceContext)
			return;

		if (!Window->iWidth || !Window->iHeight)
			return;

		oWindowProcess = (tWindowProcess)SetWindowLongPtr(hWindow, GWLP_WNDPROC, (LONG_PTR)_thunkWindowProcess.GetThunk());

		ImGui::CreateContext();
		ImGui_ImplWin32_Init(hWindow);
		ImGui_ImplDX11_Init(_device, _deviceContext);

		szIniFileName = acut::GetExeDirectory() + acut::FindAndReplaceString(DEFAULT_INI, " ", "");
		szLogFileName = acut::GetExeDirectory() + acut::FindAndReplaceString(DEFAULT_LOG, " ", "");

		ImGui::GetIO().IniFilename = szIniFileName.c_str();
		ImGui::GetIO().LogFilename = szLogFileName.c_str();

		ImGui::GetIO().FontDefault = ImGui::GetIO().Fonts->AddFontDefault();
		Bank_Gothic_Pro_Light = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(bank_gothic_pro_light_otf_compressed_data_base85, flBank_Gothic_Pro_Light = 14.0f);
		Eurostile_Extended = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(eurostile_extended_compressed_data_base85, flEurostile_Extended = Window->iHeight / 60.0f);

		bInitialized = true;
	}
	/*
	//=====================================================================================
	*/
	void cMainGUI::DrawMainGUI()
	{
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

		std::string szWatermark(VariadicText("%s - COD BO2 by: InUrFace | Frametime: %s, Ping: %s", acut::ToUpper(PROGRAM_NAME).c_str(),
			LocalClientIsInGame() ? VariadicText("%i ms", CG->iFrameTime) : "N/A",
			LocalClientIsInGame() ? VariadicText("%i ms", ClientActive->iPing) : "N/A"));

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

			ImGui::SetNextWindowSize(ImVec2(589.0f, 661.0f));
			ImGui::Begin(acut::ToUpper(PROGRAM_NAME).c_str(), &bShowWindow, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
			ImGui::SetColorEditOptions(ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop);

			if (ImGui::TabLabels(_profiler.gMenuTabs->Domain.iMax, acut::StringVectorToCharPointerArray(_profiler.gMenuTabs->szItems), _profiler.gMenuTabs->Current.iValue, NULL, false, NULL, NULL, false, false, NULL, NULL, &ImVec2(81.0f, 25.0f)))
			{
				bWriteLog = true;
			}

			ImGui::BeginChild("ContentRegion", ImVec2(0.0f, 551.0f), true);

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

				if (DrawOption(_profiler.gAntiKillTeamMates->szName, _profiler.gAntiKillTeamMates->szItems[_profiler.gAntiKillTeamMates->Current.iValue], &_profiler.gAntiKillTeamMates->Current.iValue, _profiler.gAntiKillTeamMates->Domain.iMin, _profiler.gAntiKillTeamMates->Domain.iMax, 1))
				{
					bWriteLog = true;
				} ImGui::NewLine();

				if (DrawOption(_profiler.gAntiKillIgnored->szName, _profiler.gAntiKillIgnored->szItems[_profiler.gAntiKillIgnored->Current.iValue], &_profiler.gAntiKillIgnored->Current.iValue, _profiler.gAntiKillIgnored->Domain.iMin, _profiler.gAntiKillIgnored->Domain.iMax, 1))
				{
					bWriteLog = true;
				} ImGui::NewLine();

				if (DrawOption(_profiler.gSilentAim->szName, _profiler.gSilentAim->szItems[_profiler.gSilentAim->Current.iValue], &_profiler.gSilentAim->Current.iValue, _profiler.gSilentAim->Domain.iMin, _profiler.gSilentAim->Domain.iMax, 1))
				{
					bWriteLog = true;
				} ImGui::NewLine();

				if (DrawOption(_profiler.gAntiAimPitch->szName, _profiler.gAntiAimPitch->szItems[_profiler.gAntiAimPitch->Current.iValue], &_profiler.gAntiAimPitch->Current.iValue, _profiler.gAntiAimPitch->Domain.iMin, _profiler.gAntiAimPitch->Domain.iMax, 1))
				{
					bWriteLog = true;
				} ImGui::NewLine();

				if (DrawOption(_profiler.gAntiAimYaw->szName, _profiler.gAntiAimYaw->szItems[_profiler.gAntiAimYaw->Current.iValue], &_profiler.gAntiAimYaw->Current.iValue, _profiler.gAntiAimYaw->Domain.iMin, _profiler.gAntiAimYaw->Domain.iMax, 1))
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

				ImGui::Dummy(ImGui::GetContentRegionAvail() - ImVec2(0.0f, 35.0f + ImGui::GetStyle().ItemSpacing.y));
				if (ImGui::Button("Reset to Default", ImVec2(ImGui::GetWindowContentRegionWidth(), 35.0f)))
				{
					_profiler.gAimBotMode->Current.iValue = _profiler.gAimBotMode->Reset.iValue;
					_profiler.gAutoZoom->Current.iValue = _profiler.gAutoZoom->Reset.iValue;
					_profiler.gAutoFire->Current.iValue = _profiler.gAutoFire->Reset.iValue;
					_profiler.gAutoWall->Current.iValue = _profiler.gAutoWall->Reset.iValue;
					_profiler.gApplyPrediction->Current.iValue = _profiler.gApplyPrediction->Reset.iValue;
					_profiler.gAntiKillTeamMates->Current.iValue = _profiler.gAntiKillTeamMates->Reset.iValue;
					_profiler.gSilentAim->Current.iValue = _profiler.gSilentAim->Reset.iValue;
					_profiler.gAntiAimPitch->Current.iValue = _profiler.gAntiAimPitch->Reset.iValue;
					_profiler.gAntiAimYaw->Current.iValue = _profiler.gAntiAimYaw->Reset.iValue;
					_profiler.gBoneScan->Current.iValue = _profiler.gBoneScan->Reset.iValue;
					_profiler.gSortMethod->Current.iValue = _profiler.gSortMethod->Reset.iValue;

					bWriteLog = true;
				}
			} break;

			case cProfiler::MENU_TAB_WALLHACK:
			{
				ImGui::NewLine();
				if (DrawOption(_profiler.gWallHackMode->szName, _profiler.gWallHackMode->szItems[_profiler.gWallHackMode->Current.iValue], &_profiler.gWallHackMode->Current.iValue, _profiler.gWallHackMode->Domain.iMin, _profiler.gWallHackMode->Domain.iMax, 1))
				{
					bWriteLog = true;
				} ImGui::NewLine();

				if (DrawOption(_profiler.gBoxes->szName, _profiler.gBoxes->szItems[_profiler.gBoxes->Current.iValue], &_profiler.gBoxes->Current.iValue, _profiler.gBoxes->Domain.iMin, _profiler.gBoxes->Domain.iMax, 1))
				{
					bWriteLog = true;
				} ImGui::NewLine();

				if (DrawOption(_profiler.gBones->szName, _profiler.gBones->szItems[_profiler.gBones->Current.iValue], &_profiler.gBones->Current.iValue, _profiler.gBones->Domain.iMin, _profiler.gBones->Domain.iMax, 1))
				{
					bWriteLog = true;
				} ImGui::NewLine();

				if (DrawOption(_profiler.gSnapLines->szName, _profiler.gSnapLines->szItems[_profiler.gSnapLines->Current.iValue], &_profiler.gSnapLines->Current.iValue, _profiler.gSnapLines->Domain.iMin, _profiler.gSnapLines->Domain.iMax, 1))
				{
					bWriteLog = true;
				} ImGui::NewLine();

				if (DrawOption(_profiler.gDistances->szName, _profiler.gDistances->szItems[_profiler.gDistances->Current.iValue], &_profiler.gDistances->Current.iValue, _profiler.gDistances->Domain.iMin, _profiler.gDistances->Domain.iMax, 1))
				{
					bWriteLog = true;
				} ImGui::NewLine();

				if (DrawOption(_profiler.gNames->szName, _profiler.gNames->szItems[_profiler.gNames->Current.iValue], &_profiler.gNames->Current.iValue, _profiler.gNames->Domain.iMin, _profiler.gNames->Domain.iMax, 1))
				{
					bWriteLog = true;
				} ImGui::NewLine();

				if (DrawOption(_profiler.gWeapons->szName, _profiler.gWeapons->szItems[_profiler.gWeapons->Current.iValue], &_profiler.gWeapons->Current.iValue, _profiler.gWeapons->Domain.iMin, _profiler.gWeapons->Domain.iMax, 1))
				{
					bWriteLog = true;
				} ImGui::NewLine();

				if (DrawOption(_profiler.gEntities->szName, _profiler.gEntities->szItems[_profiler.gEntities->Current.iValue], &_profiler.gEntities->Current.iValue, _profiler.gEntities->Domain.iMin, _profiler.gEntities->Domain.iMax, 1))
				{
					bWriteLog = true;
				} ImGui::NewLine();

				if (DrawOption(_profiler.gCrossHair->szName, _profiler.gCrossHair->szItems[_profiler.gCrossHair->Current.iValue], &_profiler.gCrossHair->Current.iValue, _profiler.gCrossHair->Domain.iMin, _profiler.gCrossHair->Domain.iMax, 1))
				{
					bWriteLog = true;
				} ImGui::NewLine();

				if (DrawOption(_profiler.gCompass->szName, _profiler.gCompass->szItems[_profiler.gCompass->Current.iValue], &_profiler.gCompass->Current.iValue, _profiler.gCompass->Domain.iMin, _profiler.gCompass->Domain.iMax, 1))
				{
					bWriteLog = true;
				} ImGui::NewLine();

				if (DrawOption(_profiler.gRadar->szName, _profiler.gRadar->szItems[_profiler.gRadar->Current.iValue], &_profiler.gRadar->Current.iValue, _profiler.gRadar->Domain.iMin, _profiler.gRadar->Domain.iMax, 1))
				{
					bWriteLog = true;
				} ImGui::NewLine();

				if (DrawOption(_profiler.gBulletTracers->szName, _profiler.gBulletTracers->szItems[_profiler.gBulletTracers->Current.iValue], &_profiler.gBulletTracers->Current.iValue, _profiler.gBulletTracers->Domain.iMin, _profiler.gBulletTracers->Domain.iMax, 1))
				{
					bWriteLog = true;
				} ImGui::NewLine();

				ImGui::Dummy(ImGui::GetContentRegionAvail() - ImVec2(0.0f, 35.0f + ImGui::GetStyle().ItemSpacing.y));
				if (ImGui::Button("Reset to Default", ImVec2(ImGui::GetWindowContentRegionWidth(), 35.0f)))
				{
					_profiler.gWallHackMode->Current.iValue = _profiler.gWallHackMode->Reset.iValue;
					_profiler.gBoxes->Current.iValue = _profiler.gBoxes->Reset.iValue;
					_profiler.gBones->Current.iValue = _profiler.gBones->Reset.iValue;
					_profiler.gSnapLines->Current.iValue = _profiler.gSnapLines->Reset.iValue;
					_profiler.gDistances->Current.iValue = _profiler.gDistances->Reset.iValue;
					_profiler.gNames->Current.iValue = _profiler.gNames->Reset.iValue;
					_profiler.gWeapons->Current.iValue = _profiler.gWeapons->Reset.iValue;
					_profiler.gEntities->Current.iValue = _profiler.gEntities->Reset.iValue;
					_profiler.gCrossHair->Current.iValue = _profiler.gCrossHair->Reset.iValue;
					_profiler.gCompass->Current.iValue = _profiler.gCompass->Reset.iValue;
					_profiler.gRadar->Current.iValue = _profiler.gRadar->Reset.iValue;
					_profiler.gBulletTracers->Current.iValue = _profiler.gBulletTracers->Reset.iValue;

					bWriteLog = true;
				}
			} break;

			case cProfiler::MENU_TAB_MISCELLANEOUS:
			{
				ImGui::NewLine();
				if (DrawOption(_profiler.gThirdPerson->szName, _profiler.gThirdPerson->szItems[_profiler.gThirdPerson->Current.iValue], &_profiler.gThirdPerson->Current.iValue, _profiler.gThirdPerson->Domain.iMin, _profiler.gThirdPerson->Domain.iMax, 1))
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

				if (DrawOption(_profiler.gAirStuckToggle->szName, _profiler.gAirStuckToggle->szItems[_profiler.gAirStuckToggle->Current.iValue], &_profiler.gAirStuckToggle->Current.iValue, _profiler.gAirStuckToggle->Domain.iMin, _profiler.gAirStuckToggle->Domain.iMax, 1))
				{
					bWriteLog = true;
				} ImGui::NewLine();

				if (DrawOption(_profiler.gReloadCancel->szName, _profiler.gReloadCancel->szItems[_profiler.gReloadCancel->Current.iValue], &_profiler.gReloadCancel->Current.iValue, _profiler.gReloadCancel->Domain.iMin, _profiler.gReloadCancel->Domain.iMax, 1))
				{
					bWriteLog = true;
				} ImGui::NewLine();

				if (DrawOption(_profiler.gNoFlinch->szName, _profiler.gNoFlinch->szItems[_profiler.gNoFlinch->Current.iValue], &_profiler.gNoFlinch->Current.iValue, _profiler.gNoFlinch->Domain.iMin, _profiler.gNoFlinch->Domain.iMax, 1))
				{
					bWriteLog = true;
				} ImGui::NewLine();

				if (DrawOption(_profiler.gBoneScanPriorities->szName, _profiler.gBoneScanPriorities->szItems[_profiler.gBoneScanPriorities->Current.iValue], &_profiler.gBoneScanPriorities->Current.iValue, _profiler.gBoneScanPriorities->Domain.iMin, _profiler.gBoneScanPriorities->Domain.iMax, 1))
				{
					bWriteLog = true;
				} ImGui::NewLine();

				if (DrawOption(_profiler.gBoneScanRiotShielders->szName, _profiler.gBoneScanRiotShielders->szItems[_profiler.gBoneScanRiotShielders->Current.iValue], &_profiler.gBoneScanRiotShielders->Current.iValue, _profiler.gBoneScanRiotShielders->Domain.iMin, _profiler.gBoneScanRiotShielders->Domain.iMax, 1))
				{
					bWriteLog = true;
				} ImGui::NewLine();

				if (DrawOption(_profiler.gIdSpam->szName, _profiler.gIdSpam->szItems[_profiler.gIdSpam->Current.iValue], &_profiler.gIdSpam->Current.iValue, _profiler.gIdSpam->Domain.iMin, _profiler.gIdSpam->Domain.iMax, 1))
				{
					bWriteLog = true;
				} ImGui::NewLine();

				if (DrawOption(_profiler.gTrickShot->szName, _profiler.gTrickShot->szItems[_profiler.gTrickShot->Current.iValue], &_profiler.gTrickShot->Current.iValue, _profiler.gTrickShot->Domain.iMin, _profiler.gTrickShot->Domain.iMax, 1))
				{
					bWriteLog = true;
				} ImGui::NewLine();

				ImGui::Dummy(ImGui::GetContentRegionAvail() - ImVec2(0.0f, 35.0f + ImGui::GetStyle().ItemSpacing.y));
				if (ImGui::Button("Reset to Default", ImVec2(ImGui::GetWindowContentRegionWidth(), 35.0f)))
				{
					_profiler.gThirdPerson->Current.iValue = _profiler.gThirdPerson->Reset.iValue;
					_profiler.gOrbitalVsat->Current.iValue = _profiler.gOrbitalVsat->Reset.iValue;
					_profiler.gHardcoreHud->Current.iValue = _profiler.gHardcoreHud->Reset.iValue;
					_profiler.gDisableEmp->Current.iValue = _profiler.gDisableEmp->Reset.iValue;
					_profiler.gIdStealer->Current.iValue = _profiler.gIdStealer->Reset.iValue;
					_profiler.gAirStuckToggle->Current.iValue = _profiler.gAirStuckToggle->Reset.iValue;
					_profiler.gReloadCancel->Current.iValue = _profiler.gReloadCancel->Reset.iValue;
					_profiler.gNoFlinch->Current.iValue = _profiler.gNoFlinch->Reset.iValue;
					_profiler.gBoneScanPriorities->Current.iValue = _profiler.gBoneScanPriorities->Reset.iValue;
					_profiler.gBoneScanRiotShielders->Current.iValue = _profiler.gBoneScanRiotShielders->Reset.iValue;
					_profiler.gIdSpam->Current.iValue = _profiler.gIdSpam->Reset.iValue;
					_profiler.gTrickShot->Current.iValue = _profiler.gTrickShot->Reset.iValue;

					bWriteLog = true;
				}
			} break;

			case cProfiler::MENU_TAB_TWEAKS:
			{
				ImGui::NewLine();
				if (DrawOption(_profiler.gAimBone->szName, vBones[_profiler.gAimBone->Current.iValue].second.first, &_profiler.gAimBone->Current.iValue, _profiler.gAimBone->Domain.iMin, _profiler.gAimBone->Domain.iMax, 1))
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

				if (DrawOption(_profiler.gAntiAimCustomPitch->szName, VariadicText("%.0f", _profiler.gAntiAimCustomPitch->Current.flValue), &_profiler.gAntiAimCustomPitch->Current.flValue, _profiler.gAntiAimCustomPitch->Domain.flMin, _profiler.gAntiAimCustomPitch->Domain.flMax, 1.0f))
				{
					bWriteLog = true;
				} ImGui::NewLine();

				if (DrawOption(_profiler.gAntiAimCustomYaw->szName, VariadicText("%.0f", _profiler.gAntiAimCustomYaw->Current.flValue), &_profiler.gAntiAimCustomYaw->Current.flValue, _profiler.gAntiAimCustomYaw->Domain.flMin, _profiler.gAntiAimCustomYaw->Domain.flMax, 1.0f))
				{
					bWriteLog = true;
				} ImGui::NewLine();

				if (DrawOption(_profiler.gFieldOfView->szName, VariadicText("%.0f", _profiler.gFieldOfView->Current.flValue), &_profiler.gFieldOfView->Current.flValue, _profiler.gFieldOfView->Domain.flMin, _profiler.gFieldOfView->Domain.flMax, 1.0f))
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
					_profiler.gAntiAimCustomPitch->Current.flValue = _profiler.gAntiAimCustomPitch->Reset.flValue;
					_profiler.gAntiAimCustomYaw->Current.flValue = _profiler.gAntiAimCustomYaw->Reset.flValue;
					_profiler.gFieldOfView->Current.flValue = _profiler.gFieldOfView->Reset.flValue;

					bWriteLog = true;
				}
			} break;

			case cProfiler::MENU_TAB_STYLES:
			{
				ImGui::NewLine();
				if (DrawColorPicker(_profiler.gColorAxisVisible->szName, _profiler.gColorAxisVisible->Current.cValue))
				{
					bWriteLog = true;
				} ImGui::NewLine();

				if (DrawColorPicker(_profiler.gColorAxisInvisible->szName, _profiler.gColorAxisInvisible->Current.cValue))
				{
					bWriteLog = true;
				} ImGui::NewLine();

				if (DrawColorPicker(_profiler.gColorAlliesVisible->szName, _profiler.gColorAlliesVisible->Current.cValue))
				{
					bWriteLog = true;
				} ImGui::NewLine();

				if (DrawColorPicker(_profiler.gColorAlliesInvisible->szName, _profiler.gColorAlliesInvisible->Current.cValue))
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

				ImGui::Dummy(ImGui::GetContentRegionAvail() - ImVec2(0.0f, 35.0f + ImGui::GetStyle().ItemSpacing.y));
				if (ImGui::Button("Reset to Default", ImVec2(ImGui::GetWindowContentRegionWidth(), 35.0f)))
				{
					_profiler.gColorAxisVisible->Current.cValue = _profiler.gColorAxisVisible->Reset.cValue;
					_profiler.gColorAxisInvisible->Current.cValue = _profiler.gColorAxisInvisible->Reset.cValue;
					_profiler.gColorAlliesVisible->Current.cValue = _profiler.gColorAlliesVisible->Reset.cValue;
					_profiler.gColorAlliesInvisible->Current.cValue = _profiler.gColorAlliesInvisible->Reset.cValue;
					_profiler.gColorAccents->Current.cValue = _profiler.gColorAccents->Reset.cValue;
					_profiler.gColorCrossHair->Current.cValue = _profiler.gColorCrossHair->Reset.cValue;
					_profiler.gColorText->Current.cValue = _profiler.gColorText->Reset.cValue;
					_profiler.gColorShadow->Current.cValue = _profiler.gColorShadow->Reset.cValue;

					bWriteLog = true;
				}
			} break;

			case cProfiler::MENU_TAB_PLAYERS:
			{
				ImGui::Columns(3, "PlayerList");
				ImGui::Separator();

				ImGui::Text("Name"); ImGui::NextColumn();
				ImGui::Text("IP Address"); ImGui::NextColumn();
				ImGui::Text("SteamID"); ImGui::NextColumn();

				for (int i = 0; i < MAX_CLIENTS; i++)
				{
					if ((std::string(ServerSession[i].szName).empty() && !DwordFromBytes(ServerSession[i].iIPAddress) && !ServerSession[i].qwXuid) ||
						(!IsUserRegistered(GetCurrentSession(), i) && !CG->ClientInfo[i].iInfoValid))
						continue;

					ImGui::Separator();
					ImGui::PushID(i);

					if (ImGui::Selectable(ServerSession[i].szName, &_targetList.Priorities[i].bIsPrioritized, ImGuiSelectableFlags_SpanAllColumns))
					{
						bWriteLog = true;
					}

					if (ImGui::BeginPopupContextItem(std::to_string(i).c_str()))
					{
						if (ImGui::Selectable("Add To Friend List"))
						{
							std::ofstream file(acut::GetExeDirectory() + acut::FindAndReplaceString(DEFAULT_TXT, " ", ""), std::ios_base::out | std::ios_base::app);
							file << ServerSession[i].qwXuid << " " << ServerSession[i].szName << std::endl;

							_hooks.RefreshFriends();

							bWriteLog = true;
						}

						if (ImGui::Selectable("View Profile"))
						{
							PopOverlayForSteamID(ServerSession[i].qwXuid);

							bShowWindow = false;
							bWriteLog = true;
						}

						if (ImGui::Selectable("Steal ID"))
						{
							_profiler.gNameOverRide->Current.szValue = _strdup(ServerSession[i].szName);
							_profiler.gClanOverRide->Current.szValue = _strdup(ServerSession[i].szClan);
							_profiler.gXuidOverRide->Current.szValue = VariadicText("%llx", ServerSession[i].qwXuid);

							AddReliableCommand(VariadicText("userinfo \"\\name\\%s\\clanAbbrev\\%s\\xuid\\%llx\"",
								ServerSession[i].szName,
								ServerSession[i].szClan,
								ServerSession[i].qwXuid));

							Cbuf_AddText(VariadicText("statWriteDDL clanTagStats clanName %s", ServerSession[i].szClan));

							bWriteLog = true;
						}

						if (ImGui::Selectable("Ignore"))
						{
							_targetList.Priorities[i].bIsIgnored = !_targetList.Priorities[i].bIsIgnored;

							bWriteLog = true;
						}

						if (_targetList.Priorities[i].bIsIgnored)
						{
							ImGui::SameLine();
							ImGui::RenderCheckMark(ImGui::GetCurrentWindow()->DrawList, ImGui::GetCurrentWindow()->DC.CursorPos, ImGui::GetColorU32(ImGuiCol_CheckMark), ImGui::GetCurrentContext()->FontSize);
							ImGui::NewLine();
						}

						ImGui::Separator();

						if (ImGui::Selectable("Copy Name"))
						{
							ImGui::LogToClipboard();
							ImGui::LogText(ServerSession[i].szName);
							ImGui::LogFinish();

							bWriteLog = true;
						}

						if (ImGui::Selectable("Copy IP Address"))
						{
							ImGui::LogToClipboard();
							ImGui::LogText(VariadicText("%u.%u.%u.%u",
								(BYTE)ServerSession[i].iIPAddress[0],
								(BYTE)ServerSession[i].iIPAddress[1],
								(BYTE)ServerSession[i].iIPAddress[2],
								(BYTE)ServerSession[i].iIPAddress[3]));

							ImGui::LogFinish();

							bWriteLog = true;
						}

						if (ImGui::Selectable("Copy SteamID"))
						{
							ImGui::LogToClipboard();
							ImGui::LogText(std::to_string(ServerSession[i].qwXuid).c_str());
							ImGui::LogFinish();

							bWriteLog = true;
						}

						ImGui::EndPopup();
					} ImGui::NextColumn();

					ImGui::Text(VariadicText("%u.%u.%u.%u",
						(BYTE)ServerSession[i].iIPAddress[0],
						(BYTE)ServerSession[i].iIPAddress[1],
						(BYTE)ServerSession[i].iIPAddress[2],
						(BYTE)ServerSession[i].iIPAddress[3]));

					if (ImGui::OpenPopupOnItemClick(std::to_string(i).c_str()))
					{
						bWriteLog = true;
					} ImGui::NextColumn();

					ImGui::Text(std::to_string(ServerSession[i].qwXuid).c_str());
					if (ImGui::OpenPopupOnItemClick(std::to_string(i).c_str()))
					{
						bWriteLog = true;
					} ImGui::NextColumn();

					ImGui::PopID();
				}

				ImGui::Columns(1);
				ImGui::Separator();
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
			LPCSTR szSavePath = SaveDialog.saveFileDialog(bSaveButton, szLastDirectory.c_str(), NULL, ".xml", "SAVE PROFILE");

			if (strlen(szSavePath))
			{
				_profiler.SaveProfile(szSavePath);
				szLastDirectory = SaveDialog.getLastDirectory();
			}

			static bool bLoadButton;
			if ((bLoadButton = ImGui::Button("Load Profile", ImVec2(189.0f, 30.0f))))
			{
				bWriteLog = true;
			}
			LPCSTR szLoadPath = LoadDialog.chooseFileDialog(bLoadButton, szLastDirectory.c_str(), ".xml", "LOAD PROFILE");

			if (strlen(szLoadPath))
			{
				_profiler.LoadProfile(szLoadPath);
				szLastDirectory = LoadDialog.getLastDirectory();
			}

			bool bReclaimFocus = false;
			ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth());
			if (ImGui::InputText("", _console.szInput, IM_ARRAYSIZE(_console.szInput), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory, _console._thunkTextCallback.GetThunk(), &_console))
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

				bWriteLog = true;

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

		if (hWindow != Dereference((HWND)dwWindowHandle))
		{
			_device->Release();
			_deviceContext->Release();

			ImGui_ImplWin32_Shutdown();
			ImGui_ImplDX11_Shutdown();
			ImGui::DestroyContext();

			bInitialized = false;
		}
	}
	/*
	//=====================================================================================
	*/
	template<typename Type>
	bool cMainGUI::DrawOption(std::string label, std::string option, Type* value, Type min, Type max, Type step)
	{
		bool bReturn = false;
		float flWidth = 110.0f, flSpacing = ImGui::GetWindowContentRegionWidth() - (ImGui::GetStyle().ItemSpacing.x + ImGui::GetStyle().FramePadding.x);
		ImVec2 vOptionSize = ImGui::GetIO().FontDefault->CalcTextSizeA(ImGui::GetIO().FontDefault->FontSize, FLT_MAX, 0.0f, option.c_str());

		ImGui::Text(label.c_str());
		ImGui::SameLine(flSpacing - flWidth - ImGui::GetFrameHeight());

		if (ImGui::ArrowButtonEx(VariadicText("%s_left", label.c_str()), ImGuiDir_Left, ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight()), ImGuiButtonFlags_Repeat | ImGuiButtonFlags_PressedOnClick))
		{
			*value -= step;

			if (*value < min)
				*value = min;

			bReturn = true;
		} ImGui::SameLine(flSpacing - flWidth / 2.0f - vOptionSize.x / 2.0f);

		ImGui::Text(option.c_str());
		ImGui::SameLine(flSpacing);

		if (ImGui::ArrowButtonEx(VariadicText("%s_right", label.c_str()), ImGuiDir_Right, ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight()), ImGuiButtonFlags_Repeat | ImGuiButtonFlags_PressedOnClick))
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
		bool bReturn = false;

		ImGui::Text(label.c_str());
		ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - ImGui::GetFrameHeight() - (ImGui::GetStyle().ItemSpacing.x + ImGui::GetStyle().FramePadding.x) - 110.0f);

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

		if (ImGui::ColorButton(label.c_str(), color, iMiscFlags, ImVec2(ImGui::GetFrameHeight() * 2.0f + 110.0f, ImGui::GetFrameHeight())))
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

		if (_profiler.gAirStuckToggle->Current.iValue)
		{
			if (GetKeyPress(VK_DELETE, false))
				bIsAirStuck = !bIsAirStuck;
		}

		else
			bIsAirStuck = GetKeyPress(VK_DELETE, true);

		if (GetActiveWindow() == hWindow)
			if (GetAsyncKeyState(VK_MENU) & 0x8000 && GetAsyncKeyState(VK_F4) & 0x8000)
				exit(EXIT_SUCCESS);

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
			pSwapChain->GetDevice(__uuidof(_device), (void**)&_device);
			_device->GetImmediateContext(&_deviceContext);
			InitInterface();
		}

		else
		{
			if (CG)
			{
				DrawMainGUI();
			}
		}
	}
	/*
	//=====================================================================================
	*/
	void WINAPI cMainGUI::DrawIndexed(_In_ ID3D11DeviceContext* pContext, _In_ UINT IndexCount, _In_ UINT StartIndexLocation, _In_ INT BaseVertexLocation)
	{
		return;
	}
	/*
	//=====================================================================================
	*/
	void WINAPI cMainGUI::ClearRenderTargetView(_In_ ID3D11DeviceContext* pContext, _In_ ID3D11RenderTargetView* pRenderTargetView, _In_ const FLOAT ColorRGBA[4])
	{
		return;
	}
}

//=====================================================================================