//=====================================================================================

#pragma once

#include "../Engine/Engine.hpp"
#include "RapidXML/rapidxml_wrap.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	class cProfiler
	{
	public:

		struct sCvar
		{
			std::string szName;
			std::vector<std::string> szItems;

			union uCvarValue
			{
				bool bValue;
				int iValue;
				float flValue;
				DWORD dwValue;
				ImVec4 cValue;
				LPSTR szValue;

				uCvarValue(bool value) : bValue(value) {}
				uCvarValue(int value) : iValue(value) {}
				uCvarValue(float value) : flValue(value) {}
				uCvarValue(DWORD value) : dwValue(value) {}
				uCvarValue(ImVec4 value) : cValue(value) {}
				uCvarValue(LPSTR value) : szValue(value) {}
			} Current, Reset;

			union uCvarLimits
			{
				struct
				{
					int iMin;
					int iMax;
				};

				struct
				{
					float flMin;
					float flMax;
				};

				struct
				{
					DWORD dwMin;
					DWORD dwMax;
				};

				uCvarLimits(int min, int max) : iMin(min), iMax(max) {}
				uCvarLimits(float min, float max) : flMin(min), flMax(max) {}
				uCvarLimits(DWORD min, DWORD max) : dwMin(min), dwMax(max) {}
			} Domain;

			sCvar(std::string name, std::vector<std::string> items, bool value) : szName(name), szItems(items), Current(value), Reset(value), Domain(NULL, NULL) {}
			sCvar(std::string name, std::vector<std::string> items, int value, int min, int max) : szName(name), szItems(items), Current(value), Reset(value), Domain(min, max) {}
			sCvar(std::string name, std::vector<std::string> items, float value, float min, float max) : szName(name), szItems(items), Current(value), Reset(value), Domain(min, max) {}
			sCvar(std::string name, std::vector<std::string> items, DWORD value, DWORD min, DWORD max) : szName(name), szItems(items), Current(value), Reset(value), Domain(min, max) {}
			sCvar(std::string name, std::vector<std::string> items, ImVec4 value) : szName(name), szItems(items), Current(value), Reset(value), Domain(NULL, NULL) {}
			sCvar(std::string name, std::vector<std::string> items, LPSTR value) : szName(name), szItems(items), Current(value), Reset(value), Domain(NULL, NULL) {}
		};

		enum eMenuTab
		{
			MENU_TAB_AIMBOT,
			MENU_TAB_WALLHACK,
			MENU_TAB_MISCELLANEOUS,
			MENU_TAB_TWEAKS,
			MENU_TAB_STYLES,
			MENU_TAB_PLAYERS,
			MENU_TAB_FRIENDS,
			MENU_TAB_CONSOLE,
			MENU_TAB_MAX
		};

		enum eAimBotMode
		{
			AIMBOT_MODE_OFF,
			AIMBOT_MODE_MANUAL,
			AIMBOT_MODE_AUTO,
			AIMBOT_MODE_MAX
		};

		enum eAntiAimPitch
		{
			ANTIAIM_PITCH_OFF,
			ANTIAIM_PITCH_ZERO,
			ANTIAIM_PITCH_UP,
			ANTIAIM_PITCH_DOWN,
			ANTIAIM_PITCH_JITTER,
			ANTIAIM_PITCH_RANDOM,
			ANTIAIM_PITCH_REVERSE,
			ANTIAIM_PITCH_MAX
		};

		enum eAntiAimYaw
		{
			ANTIAIM_YAW_OFF,
			ANTIAIM_YAW_ZERO,
			ANTIAIM_YAW_SPIN,
			ANTIAIM_YAW_JITTER,
			ANTIAIM_YAW_RANDOM,
			ANTIAIM_YAW_REVERSE,
			ANTIAIM_YAW_MAX
		};

		enum eSortMethod
		{
			SORT_METHOD_DISTANCE,
			SORT_METHOD_DAMAGE,
			SORT_METHOD_FOV,
			SORT_METHOD_MAX
		};

		enum eWallHackMode
		{
			WALLHACK_MODE_AXIS,
			WALLHACK_MODE_ALLIES,
			WALLHACK_MODE_ALL,
			WALLHACK_MODE_MAX
		};

		enum ePlayerBoxes
		{
			PLAYER_BOXES_OFF,
			PLAYER_BOXES_BORDER,
			PLAYER_BOXES_CORNER,
			PLAYER_BOXES_BORDER_FILLED,
			PLAYER_BOXES_CORNER_FILLED,
			PLAYER_BOXES_BORDER_3D,
			PLAYER_BOXES_CORNER_3D,
			PLAYER_BOXES_MAX
		};

		enum ePlayerBones
		{
			PLAYER_BONES_OFF,
			PLAYER_BONES_DOTS,
			PLAYER_BONES_LINES,
			PLAYER_BONES_MAX
		};

		enum ePlayerSnapLines
		{
			PLAYER_SNAPLINES_OFF,
			PLAYER_SNAPLINES_TOP,
			PLAYER_SNAPLINES_CENTER,
			PLAYER_SNAPLINES_BOTTOM,
			PLAYER_SNAPLINES_MAX
		};

		std::shared_ptr<sCvar> gMenuTabs = std::make_shared<sCvar>("Menu Tab", std::vector<std::string>({ "Aimbot", "ESP", "Misc.", "Tweaks", "Styles", "Players", "Friends", "Console" }), MENU_TAB_AIMBOT, MENU_TAB_AIMBOT, MENU_TAB_MAX);
		
		std::shared_ptr<sCvar> gAimBotMode = std::make_shared<sCvar>("Mode", std::vector<std::string>({ "Off", "Manual", "Auto" }), AIMBOT_MODE_OFF, AIMBOT_MODE_OFF, AIMBOT_MODE_MAX - 1);
		std::shared_ptr<sCvar> gAutoZoom = std::make_shared<sCvar>("Autozoom", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gAutoFire = std::make_shared<sCvar>("Autofire", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gAutoWall = std::make_shared<sCvar>("Autowall", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gApplyPrediction = std::make_shared<sCvar>("Apply Prediction", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gAntiKillTeamMates = std::make_shared<sCvar>("Anti-Kill Teammates", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gAntiKillIgnored = std::make_shared<sCvar>("Anti-Kill Ignored", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gSilentAim = std::make_shared<sCvar>("Silent-Aim", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gBoneScan = std::make_shared<sCvar>("Bonescan", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gTargetK9Unit = std::make_shared<sCvar>("Target K9 Unit", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gAntiAimPitch = std::make_shared<sCvar>("Anti-Aim Pitch", std::vector<std::string>({ "Off", "Zero", "Up", "Down", "Jitter", "Random", "Reverse" }), ANTIAIM_PITCH_OFF, ANTIAIM_PITCH_OFF, ANTIAIM_PITCH_MAX - 1);
		std::shared_ptr<sCvar> gAntiAimYaw = std::make_shared<sCvar>("Anti-Aim Yaw", std::vector<std::string>({ "Off", "Zero", "Spin", "Jitter", "Random", "Reverse" }), ANTIAIM_YAW_OFF, ANTIAIM_YAW_OFF, ANTIAIM_YAW_MAX - 1);
		std::shared_ptr<sCvar> gSortMethodTargets = std::make_shared<sCvar>("Sort Method Targets", std::vector<std::string>({ "Distance", "Damage", "Field of View" }), SORT_METHOD_DISTANCE, SORT_METHOD_DISTANCE, SORT_METHOD_MAX - 1);
		std::shared_ptr<sCvar> gSortMethodBones = std::make_shared<sCvar>("Sort Method Bones", std::vector<std::string>({ "Distance", "Damage", "Field of View" }), SORT_METHOD_DISTANCE, SORT_METHOD_DISTANCE, SORT_METHOD_MAX - 1);

		std::shared_ptr<sCvar> gWallHackMode = std::make_shared<sCvar>("Mode", std::vector<std::string>({ "Axis", "Allies", "All" }), WALLHACK_MODE_AXIS, WALLHACK_MODE_AXIS, WALLHACK_MODE_MAX - 1);
		std::shared_ptr<sCvar> gBoxes = std::make_shared<sCvar>("Boxes", std::vector<std::string>({ "Off", "Border", "Corner", "Border Filled", "Corner Filled", "Border 3D", "Corner 3D" }), PLAYER_BOXES_OFF, PLAYER_BOXES_OFF, PLAYER_BOXES_MAX - 1);
		std::shared_ptr<sCvar> gBones = std::make_shared<sCvar>("Bones", std::vector<std::string>({ "Off", "Dots", "Lines" }), PLAYER_BONES_OFF, PLAYER_BONES_OFF, PLAYER_BONES_MAX - 1);
		std::shared_ptr<sCvar> gSnapLines = std::make_shared<sCvar>("Snaplines", std::vector<std::string>({ "Off", "Top", "Center", "Bottom" }), PLAYER_SNAPLINES_OFF, PLAYER_SNAPLINES_OFF, PLAYER_SNAPLINES_MAX - 1);
		std::shared_ptr<sCvar> gDistances = std::make_shared<sCvar>("Distances", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gNames = std::make_shared<sCvar>("Names", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gWeapons = std::make_shared<sCvar>("Weapons", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gEntities = std::make_shared<sCvar>("Entities", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gCrossHair = std::make_shared<sCvar>("Crosshair", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gCompass = std::make_shared<sCvar>("Compass", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gRadar = std::make_shared<sCvar>("Radar", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gBulletTracers = std::make_shared<sCvar>("Bullet Tracers", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gThreatWarning = std::make_shared<sCvar>("Threat Warning", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);

		std::shared_ptr<sCvar> gThirdPerson = std::make_shared<sCvar>("Third Person", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gOrbitalVsat = std::make_shared<sCvar>("Orbital VSAT", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gHardcoreHud = std::make_shared<sCvar>("Hardcore HUD", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gDisableEmp = std::make_shared<sCvar>("Disable EMP", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gIdStealer = std::make_shared<sCvar>("ID Stealer", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gAirStuckToggle = std::make_shared<sCvar>("Airstuck Toggle", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gReloadCancel = std::make_shared<sCvar>("Reload Cancel", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gBoneScanPriorities = std::make_shared<sCvar>("Bonescan Priorities", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gBoneScanRiotShielders = std::make_shared<sCvar>("Bonescan Riotshielders", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gNameSpam = std::make_shared<sCvar>("Name Spam", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gTrickShot = std::make_shared<sCvar>("Trickshot", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gAnimatedWeaponizedCamo = std::make_shared<sCvar>("Animated Weaponized Camo", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gLagometer = std::make_shared<sCvar>("Lagometer", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);

		std::shared_ptr<sCvar> gAimBone = std::make_shared<sCvar>("Aimbone", std::vector<std::string>(), BONE_HELMET, BONE_HELMET, BONE_MAX - 1);
		std::shared_ptr<sCvar> gAimAngle = std::make_shared<sCvar>("Aimangle", std::vector<std::string>(), 180.0f, 1.0f, 180.0f);
		std::shared_ptr<sCvar> gShieldPitchShift = std::make_shared<sCvar>("Shield Pitch Shift", std::vector<std::string>(), 0.0f, -85.0f, 85.0f);
		std::shared_ptr<sCvar> gAntiAimPitchMinimum = std::make_shared<sCvar>("Anti-Aim Pitch Minimum", std::vector<std::string>(), -85.0f, -85.0f, 85.0f);
		std::shared_ptr<sCvar> gAntiAimPitchMaximum = std::make_shared<sCvar>("Anti-Aim Pitch Maximum", std::vector<std::string>(), 85.0f, -85.0f, 85.0f);
		std::shared_ptr<sCvar> gAimPower = std::make_shared<sCvar>("Aimpower", std::vector<std::string>(), 100.0f, 1.0f, 100.0f);
		std::shared_ptr<sCvar> gAutoAimTime = std::make_shared<sCvar>("Autoaim Time", std::vector<std::string>(), 0, 0, 1000);
		std::shared_ptr<sCvar> gAutoAimDelay = std::make_shared<sCvar>("Autoaim Delay", std::vector<std::string>(), 0, 0, 1000);
		std::shared_ptr<sCvar> gAutoZoomDelay = std::make_shared<sCvar>("Autozoom Delay", std::vector<std::string>(), 0, 0, 1000);
		std::shared_ptr<sCvar> gAutoFireDelay = std::make_shared<sCvar>("Autofire Delay", std::vector<std::string>(), 0, 0, 1000);
		std::shared_ptr<sCvar> gFakeLag = std::make_shared<sCvar>("Fake Lag", std::vector<std::string>(), 0, 0, 1000);
		std::shared_ptr<sCvar> gRecoilFactor = std::make_shared<sCvar>("Recoil Factor", std::vector<std::string>(), 1.0f, 0.0f, 1.0f);
		std::shared_ptr<sCvar> gSpreadFactor = std::make_shared<sCvar>("Spread Factor", std::vector<std::string>(), 1.0f, 0.0f, 1.0f);
		std::shared_ptr<sCvar> gFlinchFactor = std::make_shared<sCvar>("Flinch Factor", std::vector<std::string>(), 1.0f, 0.0f, 1.0f);
		std::shared_ptr<sCvar> gFieldOfView = std::make_shared<sCvar>("Field Of View", std::vector<std::string>(), 65.0f, 65.0f, 120.0f);
		std::shared_ptr<sCvar> gPointScale = std::make_shared<sCvar>("Point Scale", std::vector<std::string>(), 1, 1, 10);

		std::shared_ptr<sCvar> gColorAxisVisible = std::make_shared<sCvar>("Axis Visible", std::vector<std::string>(), ImVec4(ByteToFloat(0), ByteToFloat(255), ByteToFloat(0), ByteToFloat(255)));
		std::shared_ptr<sCvar> gColorAxisInvisible = std::make_shared<sCvar>("Axis Invisible", std::vector<std::string>(), ImVec4(ByteToFloat(0), ByteToFloat(255), ByteToFloat(0), ByteToFloat(255)));
		std::shared_ptr<sCvar> gColorAlliesVisible = std::make_shared<sCvar>("Allies Visible", std::vector<std::string>(), ImVec4(ByteToFloat(255), ByteToFloat(255), ByteToFloat(0), ByteToFloat(255)));
		std::shared_ptr<sCvar> gColorAlliesInvisible = std::make_shared<sCvar>("Allies Invisible", std::vector<std::string>(), ImVec4(ByteToFloat(255), ByteToFloat(255), ByteToFloat(0), ByteToFloat(255)));
		std::shared_ptr<sCvar> gColorAccents = std::make_shared<sCvar>("Accents", std::vector<std::string>(), ImVec4(ByteToFloat(255), ByteToFloat(0), ByteToFloat(0), ByteToFloat(255)));
		std::shared_ptr<sCvar> gColorCrossHair = std::make_shared<sCvar>("Crosshair", std::vector<std::string>(), ImVec4(ByteToFloat(255), ByteToFloat(0), ByteToFloat(255), ByteToFloat(255)));
		std::shared_ptr<sCvar> gColorText = std::make_shared<sCvar>("Text", std::vector<std::string>(), ImVec4(ByteToFloat(255), ByteToFloat(255), ByteToFloat(255), ByteToFloat(255)));
		std::shared_ptr<sCvar> gColorShadow = std::make_shared<sCvar>("Shadow", std::vector<std::string>(), ImVec4(ByteToFloat(0), ByteToFloat(0), ByteToFloat(0), ByteToFloat(255)));
		std::shared_ptr<sCvar> gColorThreatWarning = std::make_shared<sCvar>("Threat Warning", std::vector<std::string>(), ImVec4(ByteToFloat(0), ByteToFloat(127), ByteToFloat(255), ByteToFloat(31)));

		std::shared_ptr<sCvar> gNameOverRide = std::make_shared<sCvar>("Name Override", std::vector<std::string>(), _strdup(""));
		std::shared_ptr<sCvar> gClanOverRide = std::make_shared<sCvar>("Clan Override", std::vector<std::string>(), _strdup(""));
		std::shared_ptr<sCvar> gXuidOverRide = std::make_shared<sCvar>("XUID Override", std::vector<std::string>(), _strdup(""));
		std::shared_ptr<sCvar> gIpOverRide = std::make_shared<sCvar>("IP Override", std::vector<std::string>(), _strdup(""));
		std::shared_ptr<sCvar> gChatSpam = std::make_shared<sCvar>("Chatspam", std::vector<std::string>(), _strdup(""));
		std::shared_ptr<sCvar> gKillSpam = std::make_shared<sCvar>("Killspam", std::vector<std::string>(), _strdup(""));
		std::shared_ptr<sCvar> gVoteSpam = std::make_shared<sCvar>("Votespam", std::vector<std::string>(), _strdup(""));

		acut::XmlDoc<char> XML;

		bool SaveProfile(const std::string& path);
		bool LoadProfile(const std::string& path);
		void DisableAll();
	} extern _profiler;
}

//=====================================================================================
