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
				szFilePath = acut::AnsiToWstring(acut::GetExeDirectory() + DEFAULT_XML, CP_ACP);

			else
				szFilePath = acut::AnsiToWstring(path, CP_ACP);

			XML.set("ProtoGenesys.AimBot.Mode", gAimBotMode->Current.iValue);
			XML.set("ProtoGenesys.AimBot.AutoZoom", gAutoZoom->Current.iValue);
			XML.set("ProtoGenesys.AimBot.AutoFire", gAutoFire->Current.iValue);
			XML.set("ProtoGenesys.AimBot.AutoWall", gAutoWall->Current.iValue);
			XML.set("ProtoGenesys.AimBot.ApplyPrediction", gApplyPrediction->Current.iValue);
			XML.set("ProtoGenesys.AimBot.AntiTeamKill", gAntiTeamKill->Current.iValue);
			XML.set("ProtoGenesys.AimBot.SilentAim", gSilentAim->Current.iValue);
			XML.set("ProtoGenesys.AimBot.AntiAimPitch", gAntiAimPitch->Current.iValue);
			XML.set("ProtoGenesys.AimBot.AntiAimYaw", gAntiAimYaw->Current.iValue);
			XML.set("ProtoGenesys.AimBot.BoneScan", gBoneScan->Current.iValue);
			XML.set("ProtoGenesys.AimBot.SortMethod", gSortMethod->Current.iValue);

			XML.set("ProtoGenesys.WallHack.Mode", gWallHackMode->Current.iValue);
			XML.set("ProtoGenesys.WallHack.Boxes", gPlayerBoxes->Current.iValue);
			XML.set("ProtoGenesys.WallHack.Bones", gPlayerBones->Current.iValue);
			XML.set("ProtoGenesys.WallHack.SnapLines", gPlayerSnapLines->Current.iValue);
			XML.set("ProtoGenesys.WallHack.Info", gPlayerInfo->Current.iValue);
			XML.set("ProtoGenesys.WallHack.Weapons", gPlayerWeapons->Current.iValue);
			XML.set("ProtoGenesys.WallHack.Entities", gPlayerEntities->Current.iValue);
			XML.set("ProtoGenesys.WallHack.CrossHair", gPlayerCrossHair->Current.iValue);
			XML.set("ProtoGenesys.WallHack.Compass", gPlayerCompass->Current.iValue);
			XML.set("ProtoGenesys.WallHack.Radar", gPlayerRadar->Current.iValue);
			XML.set("ProtoGenesys.WallHack.BulletTracers", gPlayerBulletTracers->Current.iValue);

			XML.set("ProtoGenesys.Miscellaneous.ThirdPerson", gThirdPerson->Current.iValue);
			XML.set("ProtoGenesys.Miscellaneous.OrbitalVsat", gOrbitalVsat->Current.iValue);
			XML.set("ProtoGenesys.Miscellaneous.HardcoreHud", gHardcoreHud->Current.iValue);
			XML.set("ProtoGenesys.Miscellaneous.DisableEmp", gDisableEmp->Current.iValue);
			XML.set("ProtoGenesys.Miscellaneous.IdStealer", gIdStealer->Current.iValue);
			XML.set("ProtoGenesys.Miscellaneous.AirStuckToggle", gAirStuckToggle->Current.iValue);
			XML.set("ProtoGenesys.Miscellaneous.ReloadCancel", gReloadCancel->Current.iValue);
			XML.set("ProtoGenesys.Miscellaneous.NoFlinch", gNoFlinch->Current.iValue);
			XML.set("ProtoGenesys.Miscellaneous.BoneScanPriorities", gBoneScanPriorities->Current.iValue);
			XML.set("ProtoGenesys.Miscellaneous.NamePrestigeSpam", gNamePrestigeSpam->Current.iValue);
			XML.set("ProtoGenesys.Miscellaneous.TrickShot", gTrickShot->Current.iValue);

			XML.set("ProtoGenesys.Tweaks.AimBone", gAimBone->Current.iValue);
			XML.set("ProtoGenesys.Tweaks.AimAngle", gAimAngle->Current.iValue);
			XML.set("ProtoGenesys.Tweaks.AimPower", gAimPower->Current.iValue);
			XML.set("ProtoGenesys.Tweaks.AutoAimTime", gAutoAimTime->Current.iValue);
			XML.set("ProtoGenesys.Tweaks.AutoAimDelay", gAutoAimDelay->Current.iValue);
			XML.set("ProtoGenesys.Tweaks.AutoZoomDelay", gAutoZoomDelay->Current.iValue);
			XML.set("ProtoGenesys.Tweaks.AutoFireDelay", gAutoFireDelay->Current.iValue);
			XML.set("ProtoGenesys.Tweaks.RecoilFactor", gRecoilFactor->Current.flValue);
			XML.set("ProtoGenesys.Tweaks.SpreadFactor", gSpreadFactor->Current.flValue);
			XML.set("ProtoGenesys.Tweaks.AntiAimCustomPitch", gAntiAimCustomPitch->Current.flValue);
			XML.set("ProtoGenesys.Tweaks.AntiAimCustomYaw", gAntiAimCustomYaw->Current.flValue);

			XML.set("ProtoGenesys.Colors.AxisVisible.Red", gColorAxisVisible->Current.cValue[0]);
			XML.set("ProtoGenesys.Colors.AxisVisible.Green", gColorAxisVisible->Current.cValue[1]);
			XML.set("ProtoGenesys.Colors.AxisVisible.Blue", gColorAxisVisible->Current.cValue[2]);
			XML.set("ProtoGenesys.Colors.AxisVisible.Alpha", gColorAxisVisible->Current.cValue[3]);

			XML.set("ProtoGenesys.Colors.AxisInvisible.Red", gColorAxisInvisible->Current.cValue[0]);
			XML.set("ProtoGenesys.Colors.AxisInvisible.Green", gColorAxisInvisible->Current.cValue[1]);
			XML.set("ProtoGenesys.Colors.AxisInvisible.Blue", gColorAxisInvisible->Current.cValue[2]);
			XML.set("ProtoGenesys.Colors.AxisInvisible.Alpha", gColorAxisInvisible->Current.cValue[3]);

			XML.set("ProtoGenesys.Colors.AlliesVisible.Red", gColorAlliesVisible->Current.cValue[0]);
			XML.set("ProtoGenesys.Colors.AlliesVisible.Green", gColorAlliesVisible->Current.cValue[1]);
			XML.set("ProtoGenesys.Colors.AlliesVisible.Blue", gColorAlliesVisible->Current.cValue[2]);
			XML.set("ProtoGenesys.Colors.AlliesVisible.Alpha", gColorAlliesVisible->Current.cValue[3]);

			XML.set("ProtoGenesys.Colors.AlliesInvisible.Red", gColorAlliesInvisible->Current.cValue[0]);
			XML.set("ProtoGenesys.Colors.AlliesInvisible.Green", gColorAlliesInvisible->Current.cValue[1]);
			XML.set("ProtoGenesys.Colors.AlliesInvisible.Blue", gColorAlliesInvisible->Current.cValue[2]);
			XML.set("ProtoGenesys.Colors.AlliesInvisible.Alpha", gColorAlliesInvisible->Current.cValue[3]);

			XML.set("ProtoGenesys.Colors.Accents.Red", gColorAccents->Current.cValue[0]);
			XML.set("ProtoGenesys.Colors.Accents.Green", gColorAccents->Current.cValue[1]);
			XML.set("ProtoGenesys.Colors.Accents.Blue", gColorAccents->Current.cValue[2]);
			XML.set("ProtoGenesys.Colors.Accents.Alpha", gColorAccents->Current.cValue[3]);

			XML.set("ProtoGenesys.Colors.CrossHair.Red", gColorCrossHair->Current.cValue[0]);
			XML.set("ProtoGenesys.Colors.CrossHair.Green", gColorCrossHair->Current.cValue[1]);
			XML.set("ProtoGenesys.Colors.CrossHair.Blue", gColorCrossHair->Current.cValue[2]);
			XML.set("ProtoGenesys.Colors.CrossHair.Alpha", gColorCrossHair->Current.cValue[3]);

			XML.set("ProtoGenesys.Colors.Text.Red", gColorText->Current.cValue[0]);
			XML.set("ProtoGenesys.Colors.Text.Green", gColorText->Current.cValue[1]);
			XML.set("ProtoGenesys.Colors.Text.Blue", gColorText->Current.cValue[2]);
			XML.set("ProtoGenesys.Colors.Text.Alpha", gColorText->Current.cValue[3]);

			XML.set("ProtoGenesys.Colors.Shadow.Red", gColorShadow->Current.cValue[0]);
			XML.set("ProtoGenesys.Colors.Shadow.Green", gColorShadow->Current.cValue[1]);
			XML.set("ProtoGenesys.Colors.Shadow.Blue", gColorShadow->Current.cValue[2]);
			XML.set("ProtoGenesys.Colors.Shadow.Alpha", gColorShadow->Current.cValue[3]);

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
				szFilePath = acut::AnsiToWstring(acut::GetExeDirectory() + DEFAULT_XML, CP_ACP);

			else
				szFilePath = acut::AnsiToWstring(path, CP_ACP);

			if (GetFileAttributes(acut::WstringToAnsi(szFilePath, CP_ACP).c_str()) == INVALID_FILE_ATTRIBUTES)
				return false;

			XML.read_from_file(szFilePath);

			XML.get_if_present("ProtoGenesys.AimBot.Mode", gAimBotMode->Current.iValue);
			XML.get_if_present("ProtoGenesys.AimBot.AutoZoom", gAutoZoom->Current.iValue);
			XML.get_if_present("ProtoGenesys.AimBot.AutoFire", gAutoFire->Current.iValue);
			XML.get_if_present("ProtoGenesys.AimBot.AutoWall", gAutoWall->Current.iValue);
			XML.get_if_present("ProtoGenesys.AimBot.ApplyPrediction", gApplyPrediction->Current.iValue);
			XML.get_if_present("ProtoGenesys.AimBot.AntiTeamKill", gAntiTeamKill->Current.iValue);
			XML.get_if_present("ProtoGenesys.AimBot.SilentAim", gSilentAim->Current.iValue);
			XML.get_if_present("ProtoGenesys.AimBot.AntiAimPitch", gAntiAimPitch->Current.iValue);
			XML.get_if_present("ProtoGenesys.AimBot.AntiAimYaw", gAntiAimYaw->Current.iValue);
			XML.get_if_present("ProtoGenesys.AimBot.BoneScan", gBoneScan->Current.iValue);
			XML.get_if_present("ProtoGenesys.AimBot.SortMethod", gSortMethod->Current.iValue);

			XML.get_if_present("ProtoGenesys.WallHack.Mode", gWallHackMode->Current.iValue);
			XML.get_if_present("ProtoGenesys.WallHack.Boxes", gPlayerBoxes->Current.iValue);
			XML.get_if_present("ProtoGenesys.WallHack.Bones", gPlayerBones->Current.iValue);
			XML.get_if_present("ProtoGenesys.WallHack.SnapLines", gPlayerSnapLines->Current.iValue);
			XML.get_if_present("ProtoGenesys.WallHack.Info", gPlayerInfo->Current.iValue);
			XML.get_if_present("ProtoGenesys.WallHack.Weapons", gPlayerWeapons->Current.iValue);
			XML.get_if_present("ProtoGenesys.WallHack.Entities", gPlayerEntities->Current.iValue);
			XML.get_if_present("ProtoGenesys.WallHack.CrossHair", gPlayerCrossHair->Current.iValue);
			XML.get_if_present("ProtoGenesys.WallHack.Compass", gPlayerCompass->Current.iValue);
			XML.get_if_present("ProtoGenesys.WallHack.Radar", gPlayerRadar->Current.iValue);
			XML.get_if_present("ProtoGenesys.WallHack.BulletTracers", gPlayerBulletTracers->Current.iValue);

			XML.get_if_present("ProtoGenesys.Miscellaneous.ThirdPerson", gThirdPerson->Current.iValue);
			XML.get_if_present("ProtoGenesys.Miscellaneous.OrbitalVsat", gOrbitalVsat->Current.iValue);
			XML.get_if_present("ProtoGenesys.Miscellaneous.HardcoreHud", gHardcoreHud->Current.iValue);
			XML.get_if_present("ProtoGenesys.Miscellaneous.DisableEmp", gDisableEmp->Current.iValue);
			XML.get_if_present("ProtoGenesys.Miscellaneous.IdStealer", gIdStealer->Current.iValue);
			XML.get_if_present("ProtoGenesys.Miscellaneous.AirStuckToggle", gAirStuckToggle->Current.iValue);
			XML.get_if_present("ProtoGenesys.Miscellaneous.ReloadCancel", gReloadCancel->Current.iValue);
			XML.get_if_present("ProtoGenesys.Miscellaneous.NoFlinch", gNoFlinch->Current.iValue);
			XML.get_if_present("ProtoGenesys.Miscellaneous.BoneScanPriorities", gBoneScanPriorities->Current.iValue);
			XML.get_if_present("ProtoGenesys.Miscellaneous.NamePrestigeSpam", gNamePrestigeSpam->Current.iValue);
			XML.get_if_present("ProtoGenesys.Miscellaneous.TrickShot", gTrickShot->Current.iValue);

			XML.get_if_present("ProtoGenesys.Tweaks.AimBone", gAimBone->Current.iValue);
			XML.get_if_present("ProtoGenesys.Tweaks.AimAngle", gAimAngle->Current.iValue);
			XML.get_if_present("ProtoGenesys.Tweaks.AimPower", gAimPower->Current.iValue);
			XML.get_if_present("ProtoGenesys.Tweaks.AutoAimTime", gAutoAimTime->Current.iValue);
			XML.get_if_present("ProtoGenesys.Tweaks.AutoAimDelay", gAutoAimDelay->Current.iValue);
			XML.get_if_present("ProtoGenesys.Tweaks.AutoZoomDelay", gAutoZoomDelay->Current.iValue);
			XML.get_if_present("ProtoGenesys.Tweaks.AutoFireDelay", gAutoFireDelay->Current.iValue);
			XML.get_if_present("ProtoGenesys.Tweaks.RecoilFactor", gRecoilFactor->Current.flValue);
			XML.get_if_present("ProtoGenesys.Tweaks.SpreadFactor", gSpreadFactor->Current.flValue);
			XML.get_if_present("ProtoGenesys.Tweaks.AntiAimCustomPitch", gAntiAimCustomPitch->Current.flValue);
			XML.get_if_present("ProtoGenesys.Tweaks.AntiAimCustomYaw", gAntiAimCustomYaw->Current.flValue);

			XML.get_if_present("ProtoGenesys.Colors.AxisVisible.Red", gColorAxisVisible->Current.cValue[0]);
			XML.get_if_present("ProtoGenesys.Colors.AxisVisible.Green", gColorAxisVisible->Current.cValue[1]);
			XML.get_if_present("ProtoGenesys.Colors.AxisVisible.Blue", gColorAxisVisible->Current.cValue[2]);
			XML.get_if_present("ProtoGenesys.Colors.AxisVisible.Alpha", gColorAxisVisible->Current.cValue[3]);

			XML.get_if_present("ProtoGenesys.Colors.AxisInvisible.Red", gColorAxisInvisible->Current.cValue[0]);
			XML.get_if_present("ProtoGenesys.Colors.AxisInvisible.Green", gColorAxisInvisible->Current.cValue[1]);
			XML.get_if_present("ProtoGenesys.Colors.AxisInvisible.Blue", gColorAxisInvisible->Current.cValue[2]);
			XML.get_if_present("ProtoGenesys.Colors.AxisInvisible.Alpha", gColorAxisInvisible->Current.cValue[3]);

			XML.get_if_present("ProtoGenesys.Colors.AlliesVisible.Red", gColorAlliesVisible->Current.cValue[0]);
			XML.get_if_present("ProtoGenesys.Colors.AlliesVisible.Green", gColorAlliesVisible->Current.cValue[1]);
			XML.get_if_present("ProtoGenesys.Colors.AlliesVisible.Blue", gColorAlliesVisible->Current.cValue[2]);
			XML.get_if_present("ProtoGenesys.Colors.AlliesVisible.Alpha", gColorAlliesVisible->Current.cValue[3]);

			XML.get_if_present("ProtoGenesys.Colors.AlliesInvisible.Red", gColorAlliesInvisible->Current.cValue[0]);
			XML.get_if_present("ProtoGenesys.Colors.AlliesInvisible.Green", gColorAlliesInvisible->Current.cValue[1]);
			XML.get_if_present("ProtoGenesys.Colors.AlliesInvisible.Blue", gColorAlliesInvisible->Current.cValue[2]);
			XML.get_if_present("ProtoGenesys.Colors.AlliesInvisible.Alpha", gColorAlliesInvisible->Current.cValue[3]);

			XML.get_if_present("ProtoGenesys.Colors.Accents.Red", gColorAccents->Current.cValue[0]);
			XML.get_if_present("ProtoGenesys.Colors.Accents.Green", gColorAccents->Current.cValue[1]);
			XML.get_if_present("ProtoGenesys.Colors.Accents.Blue", gColorAccents->Current.cValue[2]);
			XML.get_if_present("ProtoGenesys.Colors.Accents.Alpha", gColorAccents->Current.cValue[3]);

			XML.get_if_present("ProtoGenesys.Colors.CrossHair.Red", gColorCrossHair->Current.cValue[0]);
			XML.get_if_present("ProtoGenesys.Colors.CrossHair.Green", gColorCrossHair->Current.cValue[1]);
			XML.get_if_present("ProtoGenesys.Colors.CrossHair.Blue", gColorCrossHair->Current.cValue[2]);
			XML.get_if_present("ProtoGenesys.Colors.CrossHair.Alpha", gColorCrossHair->Current.cValue[3]);

			XML.get_if_present("ProtoGenesys.Colors.Text.Red", gColorText->Current.cValue[0]);
			XML.get_if_present("ProtoGenesys.Colors.Text.Green", gColorText->Current.cValue[1]);
			XML.get_if_present("ProtoGenesys.Colors.Text.Blue", gColorText->Current.cValue[2]);
			XML.get_if_present("ProtoGenesys.Colors.Text.Alpha", gColorText->Current.cValue[3]);

			XML.get_if_present("ProtoGenesys.Colors.Shadow.Red", gColorShadow->Current.cValue[0]);
			XML.get_if_present("ProtoGenesys.Colors.Shadow.Green", gColorShadow->Current.cValue[1]);
			XML.get_if_present("ProtoGenesys.Colors.Shadow.Blue", gColorShadow->Current.cValue[2]);
			XML.get_if_present("ProtoGenesys.Colors.Shadow.Alpha", gColorShadow->Current.cValue[3]);

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