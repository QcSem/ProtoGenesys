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

		typedef enum
		{
			MENU_TAB_AIMBOT,
			MENU_TAB_WALLHACK,
			MENU_TAB_MISCELLANEOUS,
			MENU_TAB_TWEAKS,
			MENU_TAB_STYLES,
			MENU_TAB_PLAYERS,
			MENU_TAB_CONSOLE,
			MENU_TAB_MAX
		} eMenuTab;

		typedef enum
		{
			MENU_COLOR_NEUTRAL,
			MENU_COLOR_NEUTRAL_NEON,
			MENU_COLOR_RED,
			MENU_COLOR_RED_NEON,
			MENU_COLOR_ORANGE,
			MENU_COLOR_ORANGE_NEON,
			MENU_COLOR_YELLOW,
			MENU_COLOR_YELLOW_NEON,
			MENU_COLOR_GREEN,
			MENU_COLOR_GREEN_NEON,
			MENU_COLOR_BLUE,
			MENU_COLOR_BLUE_NEON,
			MENU_COLOR_PURPLE,
			MENU_COLOR_PURPLE_NEON,
			MENU_COLOR_RAINBOW,
			MENU_COLOR_RAINBOW_NEON,
			MENU_COLOR_MAX
		} eMenuColor;

		typedef enum
		{
			MENU_CURSOR_BLACK,
			MENU_CURSOR_WHITE,
			MENU_CURSOR_MAX
		} eMenuCursor;

		typedef enum
		{
			MENU_FONT_LIGHT,
			MENU_FONT_MEDIUM,
			MENU_FONT_BOLD,
			MENU_FONT_MAX
		} eMenuFont;

		typedef enum
		{
			AIMBOT_MODE_OFF,
			AIMBOT_MODE_MANUAL,
			AIMBOT_MODE_AUTO,
			AIMBOT_MODE_MAX
		} eAimBotMode;

		typedef enum
		{
			ANTIAIM_PITCH_OFF,
			ANTIAIM_PITCH_UP,
			ANTIAIM_PITCH_DOWN,
			ANTIAIM_PITCH_JITTER,
			ANTIAIM_PITCH_RANDOM,
			ANTIAIM_PITCH_REVERSE,
			ANTIAIM_PITCH_CUSTOM,
			ANTIAIM_PITCH_MAX
		} eAntiAimPitch;

		typedef enum
		{
			ANTIAIM_YAW_OFF,
			ANTIAIM_YAW_SPIN,
			ANTIAIM_YAW_JITTER,
			ANTIAIM_YAW_RANDOM,
			ANTIAIM_YAW_REVERSE,
			ANTIAIM_YAW_CUSTOM,
			ANTIAIM_YAW_MAX
		} eAntiAimYaw;

		typedef enum
		{
			BONESCAN_OFF,
			BONESCAN_ONTIMER,
			BONESCAN_IMMEDIATE,
			BONESCAN_MAX
		} eBoneScan;

		typedef enum
		{
			SORT_METHOD_FOV,
			SORT_METHOD_DISTANCE,
			SORT_METHOD_MAX
		} eSortMethod;

		typedef enum
		{
			WALLHACK_MODE_AXIS,
			WALLHACK_MODE_ALLIES,
			WALLHACK_MODE_ALL,
			WALLHACK_MODE_MAX
		} eWallHackMode;

		typedef enum
		{
			PLAYER_BOXES_OFF,
			PLAYER_BOXES_BORDER,
			PLAYER_BOXES_CORNER,
			PLAYER_BOXES_BORDER_FILLED,
			PLAYER_BOXES_CORNER_FILLED,
			PLAYER_BOXES_BORDER_3D,
			PLAYER_BOXES_CORNER_3D,
			PLAYER_BOXES_MAX
		} ePlayerBoxes;

		typedef enum
		{
			PLAYER_BONES_OFF,
			PLAYER_BONES_DOTS,
			PLAYER_BONES_LINES,
			PLAYER_BONES_MAX
		} ePlayerBones;

		typedef enum
		{
			PLAYER_SNAPLINES_OFF,
			PLAYER_SNAPLINES_TOP,
			PLAYER_SNAPLINES_BOTTOM,
			PLAYER_SNAPLINES_CROSSHAIR,
			PLAYER_SNAPLINES_MAX
		} ePlayerSnapLines;

		std::shared_ptr<sCvar> gMenuTabs = std::make_shared<sCvar>("Menu Tab", std::vector<std::string>({ "Aimbot", "ESP", "Misc.", "Tweaks", "Styles", "Players", "Console" }), MENU_TAB_AIMBOT, MENU_TAB_AIMBOT, MENU_TAB_MAX);
		
		std::shared_ptr<sCvar> gAimBotMode = std::make_shared<sCvar>("Mode", std::vector<std::string>({ "Off", "Manual", "Auto" }), AIMBOT_MODE_OFF, AIMBOT_MODE_OFF, AIMBOT_MODE_MAX - 1);
		std::shared_ptr<sCvar> gAutoZoom = std::make_shared<sCvar>("Autozoom", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gAutoFire = std::make_shared<sCvar>("Autofire", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gAutoWall = std::make_shared<sCvar>("Autowall", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gApplyPrediction = std::make_shared<sCvar>("Apply Prediction", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gAntiTeamKill = std::make_shared<sCvar>("Anti-Teamkill", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gSilentAim = std::make_shared<sCvar>("Silent-Aim", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gAntiAimPitch = std::make_shared<sCvar>("Anti-Aim Pitch", std::vector<std::string>({ "Off", "Up", "Down", "Jitter", "Random", "Reverse", "Custom" }), ANTIAIM_PITCH_OFF, ANTIAIM_PITCH_OFF, ANTIAIM_PITCH_MAX - 1);
		std::shared_ptr<sCvar> gAntiAimYaw = std::make_shared<sCvar>("Anti-Aim Yaw", std::vector<std::string>({ "Off", "Spin", "Jitter", "Random", "Reverse", "Custom" }), ANTIAIM_YAW_OFF, ANTIAIM_YAW_OFF, ANTIAIM_YAW_MAX - 1);
		std::shared_ptr<sCvar> gBoneScan = std::make_shared<sCvar>("Bonescan", std::vector<std::string>({ "Off", "On Timer", "Immediate" }), BONESCAN_OFF, BONESCAN_OFF, BONESCAN_MAX - 1);
		std::shared_ptr<sCvar> gSortMethod = std::make_shared<sCvar>("Sort Method", std::vector<std::string>({ "Field of View", "Distance" }), SORT_METHOD_FOV, SORT_METHOD_FOV, SORT_METHOD_MAX - 1);

		std::shared_ptr<sCvar> gWallHackMode = std::make_shared<sCvar>("Mode", std::vector<std::string>({ "Axis", "Allies", "All" }), WALLHACK_MODE_AXIS, WALLHACK_MODE_AXIS, WALLHACK_MODE_MAX - 1);
		std::shared_ptr<sCvar> gPlayerBoxes = std::make_shared<sCvar>("Boxes", std::vector<std::string>({ "Off", "Border", "Corner", "Border Filled", "Corner Filled", "Border 3D", "Corner 3D" }), PLAYER_BOXES_OFF, PLAYER_BOXES_OFF, PLAYER_BOXES_MAX - 1);
		std::shared_ptr<sCvar> gPlayerBones = std::make_shared<sCvar>("Bones", std::vector<std::string>({ "Off", "Dots", "Lines" }), PLAYER_BONES_OFF, PLAYER_BONES_OFF, PLAYER_BONES_MAX - 1);
		std::shared_ptr<sCvar> gPlayerSnapLines = std::make_shared<sCvar>("Snaplines", std::vector<std::string>({ "Off", "Top", "Bottom", "Crosshair" }), PLAYER_SNAPLINES_OFF, PLAYER_SNAPLINES_OFF, PLAYER_SNAPLINES_MAX - 1);
		std::shared_ptr<sCvar> gPlayerInfo = std::make_shared<sCvar>("Info", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gPlayerWeapons = std::make_shared<sCvar>("Weapons", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gPlayerEntities = std::make_shared<sCvar>("Entities", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gPlayerCrossHair = std::make_shared<sCvar>("Crosshair", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gPlayerCompass = std::make_shared<sCvar>("Compass", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gPlayerRadar = std::make_shared<sCvar>("Radar", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gPlayerBulletTracers = std::make_shared<sCvar>("Bullet Tracers", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);

		std::shared_ptr<sCvar> gThirdPerson = std::make_shared<sCvar>("Third Person", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gOrbitalVsat = std::make_shared<sCvar>("Orbital VSAT", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gHardcoreHud = std::make_shared<sCvar>("Hardcore HUD", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gDisableEmp = std::make_shared<sCvar>("Disable EMP", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gIdStealer = std::make_shared<sCvar>("ID Stealer", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gAirStuckToggle = std::make_shared<sCvar>("Airstuck Toggle", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gReloadCancel = std::make_shared<sCvar>("Reload Cancel", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gNoFlinch = std::make_shared<sCvar>("No-Flinch", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gBoneScanPriorities = std::make_shared<sCvar>("Bonescan Priorities", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gNamePrestigeSpam = std::make_shared<sCvar>("Name/Prestige Spam", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);
		std::shared_ptr<sCvar> gTrickShot = std::make_shared<sCvar>("Trickshot", std::vector<std::string>({ "Off", "On" }), FALSE, FALSE, TRUE);

		std::shared_ptr<sCvar> gAimBone = std::make_shared<sCvar>("Aimbone", std::vector<std::string>(), BONE_HELMET, BONE_HELMET, BONE_MAX - 1);
		std::shared_ptr<sCvar> gAimAngle = std::make_shared<sCvar>("Aimangle", std::vector<std::string>(), 180, 1, 180);
		std::shared_ptr<sCvar> gAimPower = std::make_shared<sCvar>("Aimpower", std::vector<std::string>(), 100, 1, 100);
		std::shared_ptr<sCvar> gAutoAimTime = std::make_shared<sCvar>("Autoaim Time", std::vector<std::string>(), 0, 0, 1000);
		std::shared_ptr<sCvar> gAutoAimDelay = std::make_shared<sCvar>("Autoaim Delay", std::vector<std::string>(), 0, 0, 1000);
		std::shared_ptr<sCvar> gAutoZoomDelay = std::make_shared<sCvar>("Autozoom Delay", std::vector<std::string>(), 0, 0, 1000);
		std::shared_ptr<sCvar> gAutoFireDelay = std::make_shared<sCvar>("Autofire Delay", std::vector<std::string>(), 0, 0, 1000);
		std::shared_ptr<sCvar> gRecoilFactor = std::make_shared<sCvar>("Recoil Factor", std::vector<std::string>(), 1.0f, 0.0f, 1.0f);
		std::shared_ptr<sCvar> gSpreadFactor = std::make_shared<sCvar>("Spread Factor", std::vector<std::string>(), 1.0f, 0.0f, 1.0f);
		std::shared_ptr<sCvar> gAntiAimCustomPitch = std::make_shared<sCvar>("Anti-Aim Custom Pitch", std::vector<std::string>(), 0.0f, -85.0f, 85.0f);
		std::shared_ptr<sCvar> gAntiAimCustomYaw = std::make_shared<sCvar>("Anti-Aim Custom Yaw", std::vector<std::string>(), 0.0f, -180.0f, 180.0f);
		std::shared_ptr<sCvar> gFieldOfView = std::make_shared<sCvar>("Field Of View", std::vector<std::string>(), 65.0f, 65.0f, 120.0f);

		std::shared_ptr<sCvar> gColorAxisVisible = std::make_shared<sCvar>("Axis Visible", std::vector<std::string>(), ImVec4(ByteToFloat(0), ByteToFloat(255), ByteToFloat(0), ByteToFloat(255)));
		std::shared_ptr<sCvar> gColorAxisInvisible = std::make_shared<sCvar>("Axis Invisible", std::vector<std::string>(), ImVec4(ByteToFloat(0), ByteToFloat(255), ByteToFloat(0), ByteToFloat(255)));
		std::shared_ptr<sCvar> gColorAlliesVisible = std::make_shared<sCvar>("Allies Visible", std::vector<std::string>(), ImVec4(ByteToFloat(255), ByteToFloat(255), ByteToFloat(0), ByteToFloat(255)));
		std::shared_ptr<sCvar> gColorAlliesInvisible = std::make_shared<sCvar>("Allies Invisible", std::vector<std::string>(), ImVec4(ByteToFloat(255), ByteToFloat(255), ByteToFloat(0), ByteToFloat(255)));
		std::shared_ptr<sCvar> gColorAccents = std::make_shared<sCvar>("Accents", std::vector<std::string>(), ImVec4(ByteToFloat(255), ByteToFloat(0), ByteToFloat(0), ByteToFloat(255)));
		std::shared_ptr<sCvar> gColorCrossHair = std::make_shared<sCvar>("Crosshair", std::vector<std::string>(), ImVec4(ByteToFloat(255), ByteToFloat(0), ByteToFloat(255), ByteToFloat(255)));
		std::shared_ptr<sCvar> gColorText = std::make_shared<sCvar>("Text", std::vector<std::string>(), ImVec4(ByteToFloat(255), ByteToFloat(255), ByteToFloat(255), ByteToFloat(255)));
		std::shared_ptr<sCvar> gColorShadow = std::make_shared<sCvar>("Shadow", std::vector<std::string>(), ImVec4(ByteToFloat(0), ByteToFloat(0), ByteToFloat(0), ByteToFloat(255)));

		std::shared_ptr<sCvar> gNameOverRide = std::make_shared<sCvar>("Name Override", std::vector<std::string>(), _strdup(""));
		std::shared_ptr<sCvar> gClanOverRide = std::make_shared<sCvar>("Clan Override", std::vector<std::string>(), _strdup(""));
		std::shared_ptr<sCvar> gXuidOverRide = std::make_shared<sCvar>("XUID Override", std::vector<std::string>(), _strdup(""));
		std::shared_ptr<sCvar> gIpOverRide = std::make_shared<sCvar>("IP Override", std::vector<std::string>(), _strdup(""));
		std::shared_ptr<sCvar> gKillSpam = std::make_shared<sCvar>("Killspam", std::vector<std::string>(), _strdup(""));

		acut::XmlDoc<char> XML;

		bool SaveProfile(const std::string& path);
		bool LoadProfile(const std::string& path);
		void DisableAll();
	} extern _profiler;
}

//=====================================================================================
