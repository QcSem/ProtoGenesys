//=====================================================================================

#include "../../StdAfx.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	cProfiler _profiler;

	bool cProfiler::SaveProfile(const std::string& path)
	{
		try
		{
			XML.create_document();
			std::wstring szFilePath;

			if (path.empty())
				szFilePath = acut::AnsiToWstring(acut::GetExeDirectory() + acut::FindAndReplaceString(DEFAULT_XML, " ", ""), CP_ACP);

			else
				szFilePath = acut::AnsiToWstring(path, CP_ACP);

			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".AimBot.Mode", gAimBotMode->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".AimBot.AutoZoom", gAutoZoom->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".AimBot.AutoFire", gAutoFire->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".AimBot.AutoWall", gAutoWall->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".AimBot.ApplyPrediction", gApplyPrediction->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".AimBot.AntiTeamKill", gAntiTeamKill->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".AimBot.SilentAim", gSilentAim->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".AimBot.AntiAimPitch", gAntiAimPitch->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".AimBot.AntiAimYaw", gAntiAimYaw->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".AimBot.BoneScan", gBoneScan->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".AimBot.SortMethod", gSortMethod->Current.iValue);

			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".WallHack.Mode", gWallHackMode->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".WallHack.Boxes", gPlayerBoxes->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".WallHack.Bones", gPlayerBones->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".WallHack.SnapLines", gPlayerSnapLines->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".WallHack.Info", gPlayerInfo->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".WallHack.Weapons", gPlayerWeapons->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".WallHack.Entities", gPlayerEntities->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".WallHack.CrossHair", gPlayerCrossHair->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".WallHack.Compass", gPlayerCompass->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".WallHack.Radar", gPlayerRadar->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".WallHack.BulletTracers", gPlayerBulletTracers->Current.iValue);

			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Miscellaneous.ThirdPerson", gThirdPerson->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Miscellaneous.OrbitalVsat", gOrbitalVsat->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Miscellaneous.HardcoreHud", gHardcoreHud->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Miscellaneous.DisableEmp", gDisableEmp->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Miscellaneous.IdStealer", gIdStealer->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Miscellaneous.AirStuckToggle", gAirStuckToggle->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Miscellaneous.ReloadCancel", gReloadCancel->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Miscellaneous.NoFlinch", gNoFlinch->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Miscellaneous.BoneScanPriorities", gBoneScanPriorities->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Miscellaneous.NamePrestigeSpam", gNamePrestigeSpam->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Miscellaneous.TrickShot", gTrickShot->Current.iValue);

			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Tweaks.AimBone", gAimBone->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Tweaks.AimAngle", gAimAngle->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Tweaks.AimPower", gAimPower->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Tweaks.AutoAimTime", gAutoAimTime->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Tweaks.AutoAimDelay", gAutoAimDelay->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Tweaks.AutoZoomDelay", gAutoZoomDelay->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Tweaks.AutoFireDelay", gAutoFireDelay->Current.iValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Tweaks.RecoilFactor", gRecoilFactor->Current.flValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Tweaks.SpreadFactor", gSpreadFactor->Current.flValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Tweaks.AntiAimCustomPitch", gAntiAimCustomPitch->Current.flValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Tweaks.AntiAimCustomYaw", gAntiAimCustomYaw->Current.flValue);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Tweaks.FieldOfView", gFieldOfView->Current.flValue);

			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.AxisVisible.Red", gColorAxisVisible->Current.cValue[0]);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.AxisVisible.Green", gColorAxisVisible->Current.cValue[1]);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.AxisVisible.Blue", gColorAxisVisible->Current.cValue[2]);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.AxisVisible.Alpha", gColorAxisVisible->Current.cValue[3]);

			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.AxisInvisible.Red", gColorAxisInvisible->Current.cValue[0]);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.AxisInvisible.Green", gColorAxisInvisible->Current.cValue[1]);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.AxisInvisible.Blue", gColorAxisInvisible->Current.cValue[2]);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.AxisInvisible.Alpha", gColorAxisInvisible->Current.cValue[3]);

			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.AlliesVisible.Red", gColorAlliesVisible->Current.cValue[0]);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.AlliesVisible.Green", gColorAlliesVisible->Current.cValue[1]);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.AlliesVisible.Blue", gColorAlliesVisible->Current.cValue[2]);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.AlliesVisible.Alpha", gColorAlliesVisible->Current.cValue[3]);

			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.AlliesInvisible.Red", gColorAlliesInvisible->Current.cValue[0]);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.AlliesInvisible.Green", gColorAlliesInvisible->Current.cValue[1]);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.AlliesInvisible.Blue", gColorAlliesInvisible->Current.cValue[2]);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.AlliesInvisible.Alpha", gColorAlliesInvisible->Current.cValue[3]);

			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.Accents.Red", gColorAccents->Current.cValue[0]);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.Accents.Green", gColorAccents->Current.cValue[1]);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.Accents.Blue", gColorAccents->Current.cValue[2]);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.Accents.Alpha", gColorAccents->Current.cValue[3]);

			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.CrossHair.Red", gColorCrossHair->Current.cValue[0]);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.CrossHair.Green", gColorCrossHair->Current.cValue[1]);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.CrossHair.Blue", gColorCrossHair->Current.cValue[2]);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.CrossHair.Alpha", gColorCrossHair->Current.cValue[3]);

			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.Text.Red", gColorText->Current.cValue[0]);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.Text.Green", gColorText->Current.cValue[1]);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.Text.Blue", gColorText->Current.cValue[2]);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.Text.Alpha", gColorText->Current.cValue[3]);

			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.Shadow.Red", gColorShadow->Current.cValue[0]);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.Shadow.Green", gColorShadow->Current.cValue[1]);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.Shadow.Blue", gColorShadow->Current.cValue[2]);
			XML.set(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.Shadow.Alpha", gColorShadow->Current.cValue[3]);

			XML.write_document(szFilePath);

			return true;
		}

		catch (const std::runtime_error & error)
		{
			MessageBox(_mainGui.hWindow, error.what(), "Error", MB_OK | MB_ICONERROR);

			return false;
		}
	}
	/*
	//=====================================================================================
	*/
	bool cProfiler::LoadProfile(const std::string& path)
	{
		try
		{
			std::wstring szFilePath;

			if (path.empty())
				szFilePath = acut::AnsiToWstring(acut::GetExeDirectory() + acut::FindAndReplaceString(DEFAULT_XML, " ", ""), CP_ACP);

			else
				szFilePath = acut::AnsiToWstring(path, CP_ACP);

			if (GetFileAttributes(acut::WstringToAnsi(szFilePath, CP_ACP).c_str()) == INVALID_FILE_ATTRIBUTES)
				return false;

			XML.read_from_file(szFilePath);

			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".AimBot.Mode", gAimBotMode->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".AimBot.AutoZoom", gAutoZoom->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".AimBot.AutoFire", gAutoFire->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".AimBot.AutoWall", gAutoWall->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".AimBot.ApplyPrediction", gApplyPrediction->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".AimBot.AntiTeamKill", gAntiTeamKill->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".AimBot.SilentAim", gSilentAim->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".AimBot.AntiAimPitch", gAntiAimPitch->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".AimBot.AntiAimYaw", gAntiAimYaw->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".AimBot.BoneScan", gBoneScan->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".AimBot.SortMethod", gSortMethod->Current.iValue);

			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".WallHack.Mode", gWallHackMode->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".WallHack.Boxes", gPlayerBoxes->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".WallHack.Bones", gPlayerBones->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".WallHack.SnapLines", gPlayerSnapLines->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".WallHack.Info", gPlayerInfo->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".WallHack.Weapons", gPlayerWeapons->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".WallHack.Entities", gPlayerEntities->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".WallHack.CrossHair", gPlayerCrossHair->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".WallHack.Compass", gPlayerCompass->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".WallHack.Radar", gPlayerRadar->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".WallHack.BulletTracers", gPlayerBulletTracers->Current.iValue);

			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Miscellaneous.ThirdPerson", gThirdPerson->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Miscellaneous.OrbitalVsat", gOrbitalVsat->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Miscellaneous.HardcoreHud", gHardcoreHud->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Miscellaneous.DisableEmp", gDisableEmp->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Miscellaneous.IdStealer", gIdStealer->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Miscellaneous.AirStuckToggle", gAirStuckToggle->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Miscellaneous.ReloadCancel", gReloadCancel->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Miscellaneous.NoFlinch", gNoFlinch->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Miscellaneous.BoneScanPriorities", gBoneScanPriorities->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Miscellaneous.NamePrestigeSpam", gNamePrestigeSpam->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Miscellaneous.TrickShot", gTrickShot->Current.iValue);

			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Tweaks.AimBone", gAimBone->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Tweaks.AimAngle", gAimAngle->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Tweaks.AimPower", gAimPower->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Tweaks.AutoAimTime", gAutoAimTime->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Tweaks.AutoAimDelay", gAutoAimDelay->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Tweaks.AutoZoomDelay", gAutoZoomDelay->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Tweaks.AutoFireDelay", gAutoFireDelay->Current.iValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Tweaks.RecoilFactor", gRecoilFactor->Current.flValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Tweaks.SpreadFactor", gSpreadFactor->Current.flValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Tweaks.AntiAimCustomPitch", gAntiAimCustomPitch->Current.flValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Tweaks.AntiAimCustomYaw", gAntiAimCustomYaw->Current.flValue);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Tweaks.FieldOfView", gFieldOfView->Current.flValue);

			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.AxisVisible.Red", gColorAxisVisible->Current.cValue[0]);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.AxisVisible.Green", gColorAxisVisible->Current.cValue[1]);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.AxisVisible.Blue", gColorAxisVisible->Current.cValue[2]);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.AxisVisible.Alpha", gColorAxisVisible->Current.cValue[3]);

			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.AxisInvisible.Red", gColorAxisInvisible->Current.cValue[0]);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.AxisInvisible.Green", gColorAxisInvisible->Current.cValue[1]);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.AxisInvisible.Blue", gColorAxisInvisible->Current.cValue[2]);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.AxisInvisible.Alpha", gColorAxisInvisible->Current.cValue[3]);

			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.AlliesVisible.Red", gColorAlliesVisible->Current.cValue[0]);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.AlliesVisible.Green", gColorAlliesVisible->Current.cValue[1]);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.AlliesVisible.Blue", gColorAlliesVisible->Current.cValue[2]);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.AlliesVisible.Alpha", gColorAlliesVisible->Current.cValue[3]);

			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.AlliesInvisible.Red", gColorAlliesInvisible->Current.cValue[0]);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.AlliesInvisible.Green", gColorAlliesInvisible->Current.cValue[1]);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.AlliesInvisible.Blue", gColorAlliesInvisible->Current.cValue[2]);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.AlliesInvisible.Alpha", gColorAlliesInvisible->Current.cValue[3]);

			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.Accents.Red", gColorAccents->Current.cValue[0]);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.Accents.Green", gColorAccents->Current.cValue[1]);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.Accents.Blue", gColorAccents->Current.cValue[2]);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.Accents.Alpha", gColorAccents->Current.cValue[3]);

			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.CrossHair.Red", gColorCrossHair->Current.cValue[0]);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.CrossHair.Green", gColorCrossHair->Current.cValue[1]);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.CrossHair.Blue", gColorCrossHair->Current.cValue[2]);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.CrossHair.Alpha", gColorCrossHair->Current.cValue[3]);

			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.Text.Red", gColorText->Current.cValue[0]);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.Text.Green", gColorText->Current.cValue[1]);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.Text.Blue", gColorText->Current.cValue[2]);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.Text.Alpha", gColorText->Current.cValue[3]);

			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.Shadow.Red", gColorShadow->Current.cValue[0]);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.Shadow.Green", gColorShadow->Current.cValue[1]);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.Shadow.Blue", gColorShadow->Current.cValue[2]);
			XML.get_if_present(acut::FindAndReplaceString(PROGRAM_NAME, " ", "") + ".Colors.Shadow.Alpha", gColorShadow->Current.cValue[3]);

			return true;
		}

		catch (const std::runtime_error & error)
		{
			MessageBox(_mainGui.hWindow, error.what(), "Error", MB_OK | MB_ICONERROR);

			return false;
		}
	}
	/*
	//=====================================================================================
	*/
	void cProfiler::DisableAll()
	{
		gAimBotMode->Current.iValue = AIMBOT_MODE_OFF;
		gAutoZoom->Current.iValue = FALSE;
		gAutoFire->Current.iValue = FALSE;
		gAutoWall->Current.iValue = FALSE;
		gApplyPrediction->Current.iValue = FALSE;
		gAntiTeamKill->Current.iValue = FALSE;
		gSilentAim->Current.iValue = FALSE;
		gAntiAimPitch->Current.iValue = ANTIAIM_PITCH_OFF;
		gAntiAimYaw->Current.iValue = ANTIAIM_YAW_OFF;
		gBoneScan->Current.iValue = BONESCAN_OFF;
		gSortMethod->Current.iValue = SORT_METHOD_FOV;

		gWallHackMode->Current.iValue = WALLHACK_MODE_AXIS;
		gPlayerBoxes->Current.iValue = PLAYER_BOXES_OFF;
		gPlayerBones->Current.iValue = PLAYER_BONES_OFF;
		gPlayerSnapLines->Current.iValue = PLAYER_SNAPLINES_OFF;
		gPlayerInfo->Current.iValue = FALSE;
		gPlayerWeapons->Current.iValue = FALSE;
		gPlayerEntities->Current.iValue = FALSE;
		gPlayerCrossHair->Current.iValue = FALSE;
		gPlayerCompass->Current.iValue = FALSE;
		gPlayerRadar->Current.iValue = FALSE;
		gPlayerBulletTracers->Current.iValue = FALSE;

		gThirdPerson->Current.iValue = FALSE;
		gOrbitalVsat->Current.iValue = FALSE;
		gHardcoreHud->Current.iValue = FALSE;
		gDisableEmp->Current.iValue = FALSE;
		gIdStealer->Current.iValue = FALSE;
		gAirStuckToggle->Current.iValue = FALSE;
		gReloadCancel->Current.iValue = FALSE;
		gNoFlinch->Current.iValue = FALSE;
		gBoneScanPriorities->Current.iValue = FALSE;
		gNamePrestigeSpam->Current.iValue = FALSE;
		gTrickShot->Current.iValue = FALSE;
	}
}

//=====================================================================================