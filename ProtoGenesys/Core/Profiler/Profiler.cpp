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

			XML.set("ProtoGenesys.AimBot.Mode", gAimBotMode->Custom.iValue);
			XML.set("ProtoGenesys.AimBot.AutoZoom", gAutoZoom->Custom.bValue);
			XML.set("ProtoGenesys.AimBot.AutoFire", gAutoFire->Custom.bValue);
			XML.set("ProtoGenesys.AimBot.AutoWall", gAutoWall->Custom.bValue);
			XML.set("ProtoGenesys.AimBot.ApplyPrediction", gApplyPrediction->Custom.bValue);
			XML.set("ProtoGenesys.AimBot.AntiTeamKill", gAntiTeamKill->Custom.bValue);
			XML.set("ProtoGenesys.AimBot.SilentAim", gSilentAim->Custom.bValue);
			XML.set("ProtoGenesys.AimBot.AntiAim", gAntiAim->Custom.iValue);
			XML.set("ProtoGenesys.AimBot.BoneScan", gBoneScan->Custom.iValue);
			XML.set("ProtoGenesys.AimBot.SortMethod", gSortMethod->Custom.iValue);

			XML.set("ProtoGenesys.WallHack.Mode", gWallHackMode->Custom.iValue);
			XML.set("ProtoGenesys.WallHack.Boxes", gPlayerBoxes->Custom.iValue);
			XML.set("ProtoGenesys.WallHack.Bones", gPlayerBones->Custom.iValue);
			XML.set("ProtoGenesys.WallHack.SnapLines", gPlayerSnapLines->Custom.iValue);
			XML.set("ProtoGenesys.WallHack.Info", gPlayerInfo->Custom.bValue);
			XML.set("ProtoGenesys.WallHack.Weapons", gPlayerWeapons->Custom.bValue);
			XML.set("ProtoGenesys.WallHack.Entities", gPlayerEntities->Custom.bValue);
			XML.set("ProtoGenesys.WallHack.CrossHair", gPlayerCrossHair->Custom.bValue);
			XML.set("ProtoGenesys.WallHack.Compass", gPlayerCompass->Custom.bValue);
			XML.set("ProtoGenesys.WallHack.Radar", gPlayerRadar->Custom.bValue);

			XML.set("ProtoGenesys.Tweaks.AimBone", gAimBone->Custom.iValue);
			XML.set("ProtoGenesys.Tweaks.AimAngle", gAimAngle->Custom.flValue);
			XML.set("ProtoGenesys.Tweaks.AimPower", gAimPower->Custom.iValue);
			XML.set("ProtoGenesys.Tweaks.AutoAimTime", gAutoAimTime->Custom.iValue);
			XML.set("ProtoGenesys.Tweaks.AutoAimDelay", gAutoAimDelay->Custom.iValue);
			XML.set("ProtoGenesys.Tweaks.AutoZoomDelay", gAutoZoomDelay->Custom.iValue);
			XML.set("ProtoGenesys.Tweaks.AutoFireDelay", gAutoFireDelay->Custom.iValue);
			XML.set("ProtoGenesys.Tweaks.RecoilFactor", gRecoilFactor->Custom.flValue);
			XML.set("ProtoGenesys.Tweaks.SpreadFactor", gSpreadFactor->Custom.flValue);

			XML.set("ProtoGenesys.Colors.Axis.Red", gColorAxis->Custom.cValue[0]);
			XML.set("ProtoGenesys.Colors.Axis.Green", gColorAxis->Custom.cValue[1]);
			XML.set("ProtoGenesys.Colors.Axis.Blue", gColorAxis->Custom.cValue[2]);

			XML.set("ProtoGenesys.Colors.Allies.Red", gColorAllies->Custom.cValue[0]);
			XML.set("ProtoGenesys.Colors.Allies.Green", gColorAllies->Custom.cValue[1]);
			XML.set("ProtoGenesys.Colors.Allies.Blue", gColorAllies->Custom.cValue[2]);

			XML.set("ProtoGenesys.Colors.RiotShield.Red", gColorRiotShield->Custom.cValue[0]);
			XML.set("ProtoGenesys.Colors.RiotShield.Green", gColorRiotShield->Custom.cValue[1]);
			XML.set("ProtoGenesys.Colors.RiotShield.Blue", gColorRiotShield->Custom.cValue[2]);

			XML.set("ProtoGenesys.Colors.CrossHair.Red", gColorCrossHair->Custom.cValue[0]);
			XML.set("ProtoGenesys.Colors.CrossHair.Green", gColorCrossHair->Custom.cValue[1]);
			XML.set("ProtoGenesys.Colors.CrossHair.Blue", gColorCrossHair->Custom.cValue[2]);

			XML.set("ProtoGenesys.Colors.Text.Red", gColorText->Custom.cValue[0]);
			XML.set("ProtoGenesys.Colors.Text.Green", gColorText->Custom.cValue[1]);
			XML.set("ProtoGenesys.Colors.Text.Blue", gColorText->Custom.cValue[2]);

			XML.set("ProtoGenesys.Colors.Shadow.Red", gColorShadow->Custom.cValue[0]);
			XML.set("ProtoGenesys.Colors.Shadow.Green", gColorShadow->Custom.cValue[1]);
			XML.set("ProtoGenesys.Colors.Shadow.Blue", gColorShadow->Custom.cValue[2]);

			XML.write_document(szFilePath);

			return true;
		}

		catch (const std::runtime_error & e)
		{
			MessageBox(_mainGui.hWindow, e.what(), "Error", MB_OK | MB_ICONERROR);

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

			XML.get_if_present("ProtoGenesys.AimBot.Mode", gAimBotMode->Custom.iValue);
			XML.get_if_present("ProtoGenesys.AimBot.AutoZoom", gAutoZoom->Custom.bValue);
			XML.get_if_present("ProtoGenesys.AimBot.AutoFire", gAutoFire->Custom.bValue);
			XML.get_if_present("ProtoGenesys.AimBot.AutoWall", gAutoWall->Custom.bValue);
			XML.get_if_present("ProtoGenesys.AimBot.ApplyPrediction", gApplyPrediction->Custom.bValue);
			XML.get_if_present("ProtoGenesys.AimBot.AntiTeamKill", gAntiTeamKill->Custom.bValue);
			XML.get_if_present("ProtoGenesys.AimBot.SilentAim", gSilentAim->Custom.bValue);
			XML.get_if_present("ProtoGenesys.AimBot.AntiAim", gAntiAim->Custom.iValue);
			XML.get_if_present("ProtoGenesys.AimBot.BoneScan", gBoneScan->Custom.iValue);
			XML.get_if_present("ProtoGenesys.AimBot.SortMethod", gSortMethod->Custom.iValue);

			XML.get_if_present("ProtoGenesys.WallHack.Mode", gWallHackMode->Custom.iValue);
			XML.get_if_present("ProtoGenesys.WallHack.Boxes", gPlayerBoxes->Custom.iValue);
			XML.get_if_present("ProtoGenesys.WallHack.Bones", gPlayerBones->Custom.iValue);
			XML.get_if_present("ProtoGenesys.WallHack.SnapLines", gPlayerSnapLines->Custom.iValue);
			XML.get_if_present("ProtoGenesys.WallHack.Info", gPlayerInfo->Custom.bValue);
			XML.get_if_present("ProtoGenesys.WallHack.Weapons", gPlayerWeapons->Custom.bValue);
			XML.get_if_present("ProtoGenesys.WallHack.Entities", gPlayerEntities->Custom.bValue);
			XML.get_if_present("ProtoGenesys.WallHack.CrossHair", gPlayerCrossHair->Custom.bValue);
			XML.get_if_present("ProtoGenesys.WallHack.Compass", gPlayerCompass->Custom.bValue);
			XML.get_if_present("ProtoGenesys.WallHack.Radar", gPlayerRadar->Custom.bValue);

			XML.get_if_present("ProtoGenesys.Tweaks.AimBone", gAimBone->Custom.iValue);
			XML.get_if_present("ProtoGenesys.Tweaks.AimAngle", gAimAngle->Custom.flValue);
			XML.get_if_present("ProtoGenesys.Tweaks.AimPower", gAimPower->Custom.iValue);
			XML.get_if_present("ProtoGenesys.Tweaks.AutoAimTime", gAutoAimTime->Custom.iValue);
			XML.get_if_present("ProtoGenesys.Tweaks.AutoAimDelay", gAutoAimDelay->Custom.iValue);
			XML.get_if_present("ProtoGenesys.Tweaks.AutoZoomDelay", gAutoZoomDelay->Custom.iValue);
			XML.get_if_present("ProtoGenesys.Tweaks.AutoFireDelay", gAutoFireDelay->Custom.iValue);
			XML.get_if_present("ProtoGenesys.Tweaks.RecoilFactor", gRecoilFactor->Custom.flValue);
			XML.get_if_present("ProtoGenesys.Tweaks.SpreadFactor", gSpreadFactor->Custom.flValue);

			XML.get_if_present("ProtoGenesys.Colors.Axis.Red", gColorAxis->Custom.cValue[0]);
			XML.get_if_present("ProtoGenesys.Colors.Axis.Green", gColorAxis->Custom.cValue[1]);
			XML.get_if_present("ProtoGenesys.Colors.Axis.Blue", gColorAxis->Custom.cValue[2]);

			XML.get_if_present("ProtoGenesys.Colors.Allies.Red", gColorAllies->Custom.cValue[0]);
			XML.get_if_present("ProtoGenesys.Colors.Allies.Green", gColorAllies->Custom.cValue[1]);
			XML.get_if_present("ProtoGenesys.Colors.Allies.Blue", gColorAllies->Custom.cValue[2]);

			XML.get_if_present("ProtoGenesys.Colors.RiotShield.Red", gColorRiotShield->Custom.cValue[0]);
			XML.get_if_present("ProtoGenesys.Colors.RiotShield.Green", gColorRiotShield->Custom.cValue[1]);
			XML.get_if_present("ProtoGenesys.Colors.RiotShield.Blue", gColorRiotShield->Custom.cValue[2]);

			XML.get_if_present("ProtoGenesys.Colors.CrossHair.Red", gColorCrossHair->Custom.cValue[0]);
			XML.get_if_present("ProtoGenesys.Colors.CrossHair.Green", gColorCrossHair->Custom.cValue[1]);
			XML.get_if_present("ProtoGenesys.Colors.CrossHair.Blue", gColorCrossHair->Custom.cValue[2]);

			XML.get_if_present("ProtoGenesys.Colors.Text.Red", gColorText->Custom.cValue[0]);
			XML.get_if_present("ProtoGenesys.Colors.Text.Green", gColorText->Custom.cValue[1]);
			XML.get_if_present("ProtoGenesys.Colors.Text.Blue", gColorText->Custom.cValue[2]);

			XML.get_if_present("ProtoGenesys.Colors.Shadow.Red", gColorShadow->Custom.cValue[0]);
			XML.get_if_present("ProtoGenesys.Colors.Shadow.Green", gColorShadow->Custom.cValue[1]);
			XML.get_if_present("ProtoGenesys.Colors.Shadow.Blue", gColorShadow->Custom.cValue[2]);

			return true;
		}

		catch (const std::runtime_error & e)
		{
			MessageBox(_mainGui.hWindow, e.what(), "Error", MB_OK | MB_ICONERROR);

			return false;
		}
	}
	/*
	//=====================================================================================
	*/
	void cProfiler::DisableAll()
	{
		gAimBotMode->Custom.iValue = gAimBotMode->Default.iValue;
		gAutoZoom->Custom.bValue = gAutoZoom->Default.bValue;
		gAutoFire->Custom.bValue = gAutoFire->Default.bValue;
		gAutoWall->Custom.bValue = gAutoWall->Default.bValue;
		gApplyPrediction->Custom.bValue = gApplyPrediction->Default.bValue;
		gAntiTeamKill->Custom.bValue = gAntiTeamKill->Default.bValue;
		gSilentAim->Custom.bValue = gSilentAim->Default.bValue;
		gAntiAim->Custom.iValue = gAntiAim->Default.iValue;
		gBoneScan->Custom.iValue = gBoneScan->Default.iValue;
		gSortMethod->Custom.iValue = gSortMethod->Default.iValue;

		gWallHackMode->Custom.iValue = gWallHackMode->Default.iValue;
		gPlayerBoxes->Custom.iValue = gPlayerBoxes->Default.iValue;
		gPlayerBones->Custom.iValue = gPlayerBones->Default.iValue;
		gPlayerSnapLines->Custom.iValue = gPlayerSnapLines->Default.iValue;
		gPlayerInfo->Custom.bValue = gPlayerInfo->Default.bValue;
		gPlayerWeapons->Custom.bValue = gPlayerWeapons->Default.bValue;
		gPlayerEntities->Custom.bValue = gPlayerEntities->Default.bValue;
		gPlayerCrossHair->Custom.bValue = gPlayerCrossHair->Default.bValue;
		gPlayerCompass->Custom.bValue = gPlayerCompass->Default.bValue;
		gPlayerRadar->Custom.bValue = gPlayerRadar->Default.bValue;

		gOrbitalVsat->Custom.bValue = gOrbitalVsat->Default.bValue;
		gThirdPerson->Custom.bValue = gThirdPerson->Default.bValue;
		gHardcoreHud->Custom.bValue = gHardcoreHud->Default.bValue;
		gDisableEmp->Custom.bValue = gDisableEmp->Default.bValue;
		gTrickShot->Custom.bValue = gTrickShot->Default.bValue;
	}
}

//=====================================================================================