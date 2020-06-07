//=====================================================================================

#pragma once

#include <vector>
#include "Helpers.hpp"
#include "Thunk.hpp"

//=====================================================================================

#define PROGRAM_NAME "ProtoGenesys"
#define PROGRAM_CMD_PREFIX "proto"

#define MAX_CLIENTS 18
#define MAX_ENTITIES 1024
#define EF1_CROUCH 0x4
#define EF1_PRONE 0x8
#define EF1_FIRE 0x40
#define EF1_STREAK 0x4000
#define EF1_SPRINT 0x20000
#define EF1_DEAD 0x40000
#define EF1_ZOOM 0x80000
#define EF2_MANTLE 0x400
#define EF2_MOVE 0x100000
#define ID_ASSAULTSHIELD 89
#define M_METERS 55.0f
#define M_FEET (M_METERS*3.281f)
#define M_PI_DOUBLE ((float)M_PI*2.0f)

#define ByteToFloat(a) ((float)(a)/255.0f)
#define DwordFromBytes(a) (((BYTE)(a)[0])|((BYTE)(a)[1]<<8)|((BYTE)(a)[2]<<16)|((BYTE)(a)[3]<<24))
#define GetSign(a) ((a)?((*(int*)&(a)>>31)|1):0)
#define DegreesToRadians(a) ((a)*((float)M_PI/180.0f))
#define RadiansToDegrees(a) ((a)*(180.0f/(float)M_PI))
#define AngleToShort(a) ((int)((a)*(65536/360.0f))&65535)
#define ShortToAngle(a) ((float)((a)*(360.0f/65536)))
#define AngleNormalize360(a) (ShortToAngle(AngleToShort((a))))
#define AngleNormalize180(a) (((a)/360.0f-floorf((a)/360.0f+0.5f))*360.0f)

//=====================================================================================

namespace ProtoGenesys
{
	typedef unsigned __int64 QWORD;

	typedef enum
	{
		BONE_HELMET,
		BONE_HEAD,
		BONE_NECK,
		BONE_SPINE_4,
		BONE_UPPER_SPINE,
		BONE_LOWER_SPINE,
		BONE_MAIN_ROOT,
		BONE_LEFT_CLAVICLE,
		BONE_RIGHT_CLAVICLE,
		BONE_LEFT_SHOULDER,
		BONE_RIGHT_SHOULDER,
		BONE_LEFT_HIP,
		BONE_RIGHT_HIP,
		BONE_LEFT_ELBOW,
		BONE_RIGHT_ELBOW,
		BONE_LEFT_KNEE,
		BONE_RIGHT_KNEE,
		BONE_LEFT_WRIST_TWIST,
		BONE_RIGHT_WRIST_TWIST,
		BONE_LEFT_ANKLE,
		BONE_RIGHT_ANKLE,
		BONE_LEFT_WRIST,
		BONE_RIGHT_WRIST,
		BONE_LEFT_BALL,
		BONE_RIGHT_BALL,
		BONE_MAX
	}  eBone;
	/*
	//=====================================================================================
	*/
	typedef enum
	{
		ET_GENERAL,
		ET_PLAYER,
		ET_PLAYER_CORPSE,
		ET_ITEM,
		ET_MISSILE,
		ET_INVISIBLE,
		ET_SCRIPTMOVER,
		ET_SOUND_BLEND,
		ET_FX,
		ET_LOOP_FX,
		ET_PRIMARY_LIGHT,
		ET_TURRET,
		ET_HELICOPTER,
		ET_PLANE,
		ET_VEHICLE,
		ET_VEHICLE_CORPSE,
		ET_ACTOR,
		ET_ACTOR_SPAWNER,
		ET_ACTOR_CORPSE,
		ET_STREAMER_HINT,
		ET_ZBARRIER,
		ET_EVENTS,
		ET_MAX
	} eEntityType;
	/*
	//=====================================================================================
	*/
	typedef enum
	{
		TRACE_HITTYPE_NONE,
		TRACE_HITTYPE_ENTITY,
		TRACE_HITTYPE_DYNENT_MODEL,
		TRACE_HITTYPE_DYNENT_BRUSH,
		TRACE_HITTYPE_GLASS,
		TRACE_HITTYPE_MAX
	} eTraceHitType;
	/*
	//=====================================================================================
	*/
	typedef enum
	{
		PENETRATE_TYPE_NONE,
		PENETRATE_TYPE_SMALL,
		PENETRATE_TYPE_MEDIUM,
		PENETRATE_TYPE_LARGE,
		PENETRATE_TYPE_MAX
	} ePenetrateType;
	/*
	//=====================================================================================
	*/
	typedef enum
	{
		WEAPTYPE_BULLET,
		WEAPTYPE_GRENADE,
		WEAPTYPE_PROJECTILE,
		WEAPTYPE_BINOCULARS,
		WEAPTYPE_GAS,
		WEAPTYPE_BOMB,
		WEAPTYPE_MINE,
		WEAPTYPE_MELEE,
		WEAPTYPE_RIOTSHIELD,
		WEAPTYPE_MAX
	} eWeaponType;
	/*
	//=====================================================================================
	*/
	typedef enum
	{
		TEAM_FREE,
		TEAM_ALLIES,
		TEAM_AXIS,
		TEAM_THREE,
		TEAM_FOUR,
		TEAM_FIVE,
		TEAM_SIX,
		TEAM_SEVEN,
		TEAM_EIGHT,
		TEAM_SPECTATOR,
		TEAM_MAX
	} eTeam;
	/*
	//=====================================================================================
	*/
	typedef enum
	{
		ERR_FATAL,
		ERR_DROP,
		ERR_FROM_STARTUP,
		ERR_SERVERDISCONNECT,
		ERR_DISCONNECT,
		FERR_SCRIPT,
		ERR_SCRIPT_DROP,
		ERR_LOCALIZATION,
		ERR_MAX
	} eErrorParam;
	/*
	//=====================================================================================
	*/
	static std::vector<std::pair<std::string, std::string>> szBones =
	{
		std::make_pair("Helmet", "j_helmet"),
		std::make_pair("Head", "j_head"),
		std::make_pair("Neck", "j_neck"),
		std::make_pair("Spine 4", "j_spine4"),
		std::make_pair("Upper Spine", "j_spineupper"),
		std::make_pair("Lower Spine", "j_spinelower"),
		std::make_pair("Main Root", "j_mainroot"),
		std::make_pair("Left Clavicle", "j_clavicle_le"),
		std::make_pair("Right Clavicle", "j_clavicle_ri"),
		std::make_pair("Left Shoulder", "j_shoulder_le"),
		std::make_pair("Right Shoulder", "j_shoulder_ri"),
		std::make_pair("Left Hip", "j_hip_le"),
		std::make_pair("Right Hip", "j_hip_ri"),
		std::make_pair("Left Elbow", "j_elbow_le"),
		std::make_pair("Right Elbow", "j_elbow_ri"),
		std::make_pair("Left Knee", "j_knee_le"),
		std::make_pair("Right Knee", "j_knee_ri"),
		std::make_pair("Left Wrist Twist", "j_wristtwist_le"),
		std::make_pair("Right Wrist Twist", "j_wristtwist_ri"),
		std::make_pair("Left Ankle", "j_ankle_le"),
		std::make_pair("Right Ankle", "j_ankle_ri"),
		std::make_pair("Left Wrist", "j_wrist_le"),
		std::make_pair("Right Wrist", "j_wrist_ri"),
		std::make_pair("Left Ball", "j_ball_le"),
		std::make_pair("Right Ball", "j_ball_ri")
	};
	/*
	//=====================================================================================
	*/
	static std::vector<std::pair<eBone, std::pair<std::string, std::string>>> vBones =
	{
		std::make_pair(BONE_HELMET, szBones[BONE_HELMET]),
		std::make_pair(BONE_HEAD, szBones[BONE_HEAD]),
		std::make_pair(BONE_NECK, szBones[BONE_NECK]),
		std::make_pair(BONE_SPINE_4, szBones[BONE_SPINE_4]),
		std::make_pair(BONE_UPPER_SPINE, szBones[BONE_UPPER_SPINE]),
		std::make_pair(BONE_LOWER_SPINE, szBones[BONE_LOWER_SPINE]),
		std::make_pair(BONE_MAIN_ROOT, szBones[BONE_MAIN_ROOT]),
		std::make_pair(BONE_LEFT_CLAVICLE, szBones[BONE_LEFT_CLAVICLE]),
		std::make_pair(BONE_RIGHT_CLAVICLE, szBones[BONE_RIGHT_CLAVICLE]),
		std::make_pair(BONE_LEFT_SHOULDER, szBones[BONE_LEFT_SHOULDER]),
		std::make_pair(BONE_RIGHT_SHOULDER, szBones[BONE_RIGHT_SHOULDER]),
		std::make_pair(BONE_LEFT_HIP, szBones[BONE_LEFT_HIP]),
		std::make_pair(BONE_RIGHT_HIP, szBones[BONE_RIGHT_HIP]),
		std::make_pair(BONE_LEFT_ELBOW, szBones[BONE_LEFT_ELBOW]),
		std::make_pair(BONE_RIGHT_ELBOW, szBones[BONE_RIGHT_ELBOW]),
		std::make_pair(BONE_LEFT_KNEE, szBones[BONE_LEFT_KNEE]),
		std::make_pair(BONE_RIGHT_KNEE, szBones[BONE_RIGHT_KNEE]),
		std::make_pair(BONE_LEFT_WRIST_TWIST, szBones[BONE_LEFT_WRIST_TWIST]),
		std::make_pair(BONE_RIGHT_WRIST_TWIST, szBones[BONE_RIGHT_WRIST_TWIST]),
		std::make_pair(BONE_LEFT_ANKLE, szBones[BONE_LEFT_ANKLE]),
		std::make_pair(BONE_RIGHT_ANKLE, szBones[BONE_RIGHT_ANKLE]),
		std::make_pair(BONE_LEFT_WRIST, szBones[BONE_LEFT_WRIST]),
		std::make_pair(BONE_RIGHT_WRIST, szBones[BONE_RIGHT_WRIST]),
		std::make_pair(BONE_LEFT_BALL, szBones[BONE_LEFT_BALL]),
		std::make_pair(BONE_RIGHT_BALL, szBones[BONE_RIGHT_BALL])
	};
	/*
	//=====================================================================================
	*/
	typedef struct
	{
		LPSTR szName;
		char _0x4[0x14];

		union uDvarValue
		{
			bool bValue;
			int iValue;
			float flValue;
			DWORD dwValue;
			ImVec4 cValue;
			LPSTR szValue;
		} Current, Latched, Reset;

		union uDvarLimits
		{
			struct
			{
				DWORD dwMin;
				DWORD dwMax;
			};

			struct
			{
				QWORD qwMin;
				QWORD qwMax;
			};

			struct
			{
				FLOAT flMin;
				FLOAT flMax;
			};
		} Domain;

		char _0x58[0x8];
	} sDvar;
	/*
	//=====================================================================================
	*/
	typedef struct
	{
		int iServerTime;
		char _0x4[0x14];
		int iOtherFlags;
		char _0x1C[0xC];
		ImVec3 vOrigin;
		ImVec3 vVelocity;
		char _0x40[0xC];
		float flWeaponTime;
		int iWeaponDelay;
		char _0x54[0x30];
		int iThirdPerson;
		char _0x88[0x4];
		int iGravity;
		char _0x90[0x4];
		int iSpeed;
		ImVec3 vDeltaAngles;
		char _0xA4[0x12C];
		int iWeaponState[2];
		char _0x1D8[0x8];
		float flZoomProgress;
		char _0x1E4[0x4];
		float flSpreadOverride;
		int iSpreadOverrideState;
		char _0x1F0[0x8];
		ImVec3 vViewAngles;
		int iPlayerHeight;
		ImVec3 vPlayerHeight;
		char _0x214[0x18];
		int iDamageYaw;
		int iDamagePitch;
		int iDamageCount;
		char _0x238[0x8];
		int iHealth;
		char _0x244[0x4];
		int iMaxHealth;
		char _0x24C[0x74];
		int iSecondaryWeaponID;
		char _0x2C4[0x18];
		int iPrimaryWeaponID;
		char _0x2E0[0x124];
		int iSecondaryAmmoStock;
		int iPrimaryAmmoStock;
		char _0x40C[0x34];
		int iSecondaryAmmoClip;
		int iPrimaryAmmoClip;
		int iLethalAmmo;
		int iTacticalAmmo;
		char _0x450[0x5C];
		int iSatalliteTypeEnabled;
		char _0x4B0[0xA0];
		int iPerkFlags;
	}  sPlayerState;
	/*
	//=====================================================================================
	*/
	typedef struct
	{
		char _0x0[0x8];
		int iWidth;
		int iHeight;
		char _0x10[0x10];
		float flFovX;
		float flFovY;
		float flFovZ;
		char _0x2C[0x4];
		float flFov;
		ImVec3 vViewOrigin;
		char _0x40[0x4];
		ImVec3 vViewAxis[3];
		char _0x68[0x16D18];
		int iSplitScreenBlurEdges;
		int iPlayerTeleported;
		char _0x16D88[0x2D8];
	} sRefDef;
	/*
	//=====================================================================================
	*/
	typedef struct
	{
		char _0x0[0x1C];
		eWeaponType iWeaponType;
		char _0x20[0x4];
		ePenetrateType iPenetrateType;
		char _0x28[0x609];
		bool bRifleBullet;
		char _0x632[0x1AE];
		float flADSSpread;
	} sWeaponDef;
	/*
	//=====================================================================================
	*/
	typedef struct
	{
		int iSlot;
		sWeaponDef* WeaponDef;
		char _0x8[0x8];
	} sModelMask;
	/*
	//=====================================================================================
	*/
	typedef struct
	{
		int iInfoValid;
		int iNextValid;
		int iClientNum;
		char szName[32];
		eTeam iTeam1;
		eTeam iTeam2;
		int iFFATeam;
		char _0x38[0x28];
		int iRank;
		char _0x64[0x14];
		QWORD qwXuid;
		char szClan[8];
		char _0x88[0x7];
		bool bDead;
		char _0x90[0x4];
		int iScore;
		char _0x98[0x58];
		char szPlayerModel[32];
		char _0x110[0x3B4];
		ImVec3 vViewAngles;
		char _0x4D0[0xF4];
		int iWeaponID;
		char _0x5C8[0x1BC];
		sModelMask ModelMask[8];
		int iOffHandWeaponID;
	} sClientInfo;
	/*
	//=====================================================================================
	*/
	typedef struct
	{
		int iType;
		int iTime;
		int iDuration;
		ImVec3 vBase;
		ImVec3 vDelta;
	} sTrajectory;
	/*
	//=====================================================================================
	*/
	typedef struct
	{
		int eFlags1;
		int eFlags2;
		sTrajectory PositionTrajectory;
		sTrajectory AngleTrajectory;
		char _0x50[0xC];
		int iWeaponID1;
		int iWeaponID2;
		char _0x64[0x18];
	} sLerpEntityState;
	/*
	//=====================================================================================
	*/
	typedef struct
	{
		int iEntityNum;
		sLerpEntityState LerpEntityState;
		char _0x80[0x58];
		short wEntityType;
		short wGroundEntityNum;
		char _0xDA[0x2];
		short wOtherEntityNum;
		short wAttackerEntityNum;
		char _0xE2[0x2];
		int iWeaponID;
		char _0xE8[0x10];
	} sEntityState;
	/*
	//=====================================================================================
	*/
	typedef struct
	{
		char _0x0[0x2];
		short wValid;
		short wUsedForPlayerMesh;
		char _0x6[0x26];
		ImVec3 vOrigin;
		ImVec3 vViewAngles;
		char _0x44[0x11C];
		sLerpEntityState CurrentEntityState;
		sEntityState NextEntityState;
		char _0x2D4[0xA4];
		int iAlive;
		char _0x37C[0x4];
	} sCEntity;
	/*
	//=====================================================================================
	*/
	typedef struct
	{
		char _0x0[0x8];
		int iServerTime;
		char _0xC[0xC];
		int iPing;
	} sSnapShot;
	/*
	//=====================================================================================
	*/
	typedef struct
	{
		int iClientNum;
		char _0x4[0x20];
		sSnapShot* OldSnapShot;
		sSnapShot* NewSnapShot;
		char _0x2C[0x58];
		int iPing;
		int iServerTime;
		int iPhysicsTime;
		int iCommandTime;
		char _0x94[0x24];
		ImVec3 vOrigin;
		ImVec3 vVelocity;
		char _0xD0[0x10];
		int iWeaponDelay;
		char _0xE4[0x78];
		int iMoveType;
		char _0x160[0x2C];
		int iMoveFlag;
		char _0x190[0xB8];
		int iWeaponID;
		char _0x24C[0x14];
		int iWeaponState[2];
		char _0x268[0x8];
		float flZoomProgress;
		char _0x274[0x54];
		int iHealth;
		char _0x2CC[0x47DB8];
		float flFrameInterpolation;
		int iFrameTime;
		int iTime;
		int iOldTime;
		char _0x48094[0x8];
		int iThirdPerson;
		char _0x480A0[0x8];
		sPlayerState PlayerState;
		char _0x485FC[0x5294];
		sRefDef RefDef;
		ImVec3 vRefDefViewAngles;
		char _0x648FC[0x130];
		ImVec3 vWeaponAngles;
		char _0x64A38[0x3890];
		float flZoomSensitivity;
		char _0x682CC[0x68C];
		int iMatchUIVisibilityFlags;
		char _0x6895C[0xD18];
		int iWeaponSelect;
		char _0x69678[0x54];
		int iEquippedOffhand;
		char _0x696D0[0xF8];
		ImVec3 vKickAngles;
		ImVec3 vOffsetAngles;
		float flGunPitch;
		float flGunYaw;
		char _0x697E8[0xB4];
		float flShockSensitivity;
		char _0x698A0[0x1C0];
		sClientInfo ClientInfo[MAX_CLIENTS];
		char _0x72AF0[0xE04C];
		int iExtraButtonBits[2];
		char _0x80B44[0x8];
		int iPlayerTeleported;
		char _0x80B50[0xC];
		float flSpread;
		char _0x80B60[0x6BD0];
		char szGameType[4];
		char _0x87734[0x1D90C];
		sCEntity CEntity[MAX_ENTITIES];
	} sCG;
	/*
	//=====================================================================================
	*/
	typedef struct
	{
		int iServerTime;
		int iButtons[2];
		int iViewAngles[3];
		char _0x18[0xC];
		char szMove[3];
		char _0x27[0x15];
	} sUserCmd;
	/*
	//=====================================================================================
	*/
	typedef struct
	{
		char _0x0[0x68];
		int iPing;
		char _0x6C[0x42C3C];

		sUserCmd UserCmd[128];
		int iCurrentCmd;

		sUserCmd* GetUserCmd(int number)
		{
			return &UserCmd[number & 0x7F];
		}
	} sClientActive;
	/*
	//=====================================================================================
	*/
	typedef struct
	{
		char _0x0[0x18];
		LPSTR szDisplayName;
		char _0x1C[0x8];
	} sWeaponName;
	/*
	//=====================================================================================
	*/
	typedef struct
	{
		ImVec3 vAngles;
	} sViewAngles;
	/*
	//=====================================================================================
	*/
	typedef struct
	{
		ImVec3 vAngles;
	} sRecoilAngles;
	/*
	//=====================================================================================
	*/
	typedef struct
	{
		QWORD qwXuid;
		char szName[32];
		char _0x28[0x6E];
		char iIPAddress[4];
		char _0x9A[0xAE];
	} sServerSession;
	/*
	//=====================================================================================
	*/
	typedef struct
	{
		int iWidth;
		int iHeight;
	} sWindow;
	/*
	//=====================================================================================
	*/
	typedef struct
	{
		ImVec3 vNormal;
		char _0xC[0x4];
		float flFraction;
		int iSurfaceFlags;
		char _0x18[0x8];
		short wHitID;
		char _0x22[0x6];
		short wPartGroup;
		char bAllSolid;
		char bStartSolid;
		char _0x2C[0x14];
	} sTrace;
	/*
	//=====================================================================================
	*/
	typedef struct
	{
		sTrace Trace;
		char _0x40[0x4];
		ImVec3 vHitPos;
		char iIgnoreHitEnt;
		char _0x51[0x3];
		int iSurfaceType;
		char _0x58[0x8];
	} sBulletTraceResults;
	/*
	//=====================================================================================
	*/
	typedef struct
	{
		int iMaxEntNum;
		int iEntityNum;
		float flPower;
		int iBulletType;
		ImVec3 vViewOrigin;
		ImVec3 vStart;
		ImVec3 vEnd;
		ImVec3 vDir;
	} sBulletFireParams;
	/*
	//=====================================================================================
	*/
	typedef struct
	{
		ImVec3 vOrigin;
		ImVec3 vAxis[3];
	} sOrientation;
	/*
	//=====================================================================================
	*/
	typedef struct
	{
		char iIPAddress[4];
		short iPort;
		char _0x6[0x2];
		int iType;
		int iLocalNetID;
		int iServerID;
	} sNetAddr;
	/*
	//=====================================================================================
	*/
	typedef struct
	{
		ImVec2 vScaleVirtualToReal;
		ImVec2 vScaleVirtualToFull;
		ImVec2 vScaleRealToVirtual;
		ImVec2 vVirtualViewableMin;
		ImVec2 vVirtualViewableMax;
		ImVec2 vVirtualTweakableMin;
		ImVec2 vVirtualTweakableMax;
		ImVec2 vRealViewportBase;
		ImVec2 vRealViewportSize;
		ImVec2 vRealViewportMid;
		ImVec2 vRealViewableMin;
		ImVec2 vRealViewableMax;
		ImVec2 vRealTweakableMin;
		ImVec2 vRealTweakableMax;
		ImVec2 vSubScreen;
		float flHudSplitscreenScale;
	} sScreenPlacement;
	/*
	//=====================================================================================
	*/
	static MODULEINFO hT6mp = GetModuleInfo(NULL);
	static MODULEINFO hGameOverlayRenderer = GetModuleInfo("GameOverlayRenderer.dll");
	static HMODULE hSteamAPI = GetModuleHandle("steam_api.dll");

	static bool bGameOverlayRenderer = (hGameOverlayRenderer.lpBaseOfDll && hGameOverlayRenderer.SizeOfImage);
	static bool bIsSteamVersion = hT6mp.SizeOfImage == 0x400D000;

	static DWORD_PTR dwConnectPathsDvar = 0x2366AA4;
	static DWORD_PTR dwMouseAccelerationDvar = 0x11C7834;
	static DWORD_PTR dwDrawBigFPSDvar = 0x1140A08;
	static DWORD_PTR dwMaxClientsDvar = 0x25E456C;
	static DWORD_PTR dwNoDeltaDvar = 0x11C3634;

	static DWORD_PTR dwTransitionPlayerStateCall = bIsSteamVersion ? 0x55CF28 : 0x51DAE8;
	static DWORD_PTR dwGetWorldTagMatrixCall = bIsSteamVersion ? 0x7D53BA : 0x7D5CAA;
	static DWORD_PTR dwGameTypeSettingsCall = bIsSteamVersion ? 0x97CBC2 : 0x97CC42;
	static DWORD_PTR dwPenetrationMinFxDist = bIsSteamVersion ? *(DWORD_PTR*)0x324D65C : *(DWORD_PTR*)0x322C65C;
	static DWORD_PTR dwPenetrationMultiplier = bIsSteamVersion ? *(DWORD_PTR*)0x325FC04 : *(DWORD_PTR*)0x323EC04;
	static DWORD_PTR dwPenetrationCount = *(DWORD_PTR*)0x28FB870;
	static DWORD_PTR dwOrbitalVsat = 0x12A0CA4;

	static DWORD_PTR dwSysGetValueException = bIsSteamVersion ? 0x635F09 : 0x5EFBB9;
	static DWORD_PTR dwRegisterShaderException = bIsSteamVersion ? 0x6874E9 : 0x45A8C9;
	static DWORD_PTR dwConnectPathsException1 = bIsSteamVersion ? 0x43B6DB : 0x53367B;
	static DWORD_PTR dwConnectPathsException2 = bIsSteamVersion ? 0x4CF0C5 : 0x488335;
	static DWORD_PTR dwObituaryException = bIsSteamVersion ? 0x7A345C : 0x7A40EC;

	static DWORD_PTR dwComError = bIsSteamVersion ? 0x4ECFD0 : 0x58FC30;
	static DWORD_PTR dwSysGetValue = bIsSteamVersion ? 0x635EF0 : 0x5EFBA0;
	static DWORD_PTR dwSysMilliseconds = bIsSteamVersion ? 0x5BD480 : 0x57EC40;
	static DWORD_PTR dwRegisterShader = bIsSteamVersion ? 0x734000 : 0x734CC0;
	static DWORD_PTR dwPrepFireParams = bIsSteamVersion ? 0x6EE050 : 0x638120;
	static DWORD_PTR dwPredictPlayerState = bIsSteamVersion ? 0x5B3C40 : 0x5B4F40;
	static DWORD_PTR dwSetValueForKey = bIsSteamVersion ? 0x406830 : 0x551B00;
	static DWORD_PTR dwGetUsername = bIsSteamVersion ? 0x65FDE0 : 0x4B58D0;
	static DWORD_PTR dwGetClantag = bIsSteamVersion ? 0x5ACDA0 : 0x6D2900;
	static DWORD_PTR dwGetXuidstring = bIsSteamVersion ? 0x534D60 : 0x491870;
	static DWORD_PTR dwPopOverlayForSteamID = bIsSteamVersion ? 0x589740 : 0x556680;
	static DWORD_PTR dwGetIntPlayerStatInternal = bIsSteamVersion ? 0x67A430 : 0x52EE10;
	static DWORD_PTR dwInt64ToString = bIsSteamVersion ? 0x57E0F0 : 0x427820;
	static DWORD_PTR dwBulletHitEvent = bIsSteamVersion ? 0x57CED0 : 0x5F72B0;
	static DWORD_PTR dwCalcEntityLerpPositions = bIsSteamVersion ? 0x469870 : 0x6B98D0;
	static DWORD_PTR dwInterpolateEntityPosition = bIsSteamVersion ? 0x7A0000 : 0x7A0C90;
	static DWORD_PTR dwInterpolateEntityAngles = bIsSteamVersion ? 0x6B4100 : 0x4B1D40;
	static DWORD_PTR dwTransitionPlayerState = bIsSteamVersion ? 0x666DD0 : 0x6AB990;
	static DWORD_PTR dwGetWorldTagMatrix = bIsSteamVersion ? 0x47AC00 : 0x4FC740;
	static DWORD_PTR dwGameTypeSettings = bIsSteamVersion ? 0x6C43C0 : 0x4F5F70;
	static DWORD_PTR dwGetAddr = bIsSteamVersion ? 0x628E30 : 0x4D3A70;
	static DWORD_PTR dwGetPlayerStatus = bIsSteamVersion ? 0x8C5DD0 : 0x8C5F30;
	static DWORD_PTR dwSteamIDIsValid = bIsSteamVersion ? 0x531AC0 : 0x60EFC0;
	static DWORD_PTR dwAtoiCall1 = bIsSteamVersion ? 0x7D0DC8 : 0x7D16B8;
	static DWORD_PTR dwAtoiCall2 = bIsSteamVersion ? 0x7D0DD7 : 0x7D16C7;

	static DWORD_PTR dwCG = *(DWORD_PTR*)0x113F18C;
	static DWORD_PTR dwClientActive = 0x11C7D74;
	static DWORD_PTR dwViewAngles = dwCG + 0xFE64393C;
	static DWORD_PTR dwRecoilAngles = dwCG + 0xFE6438B0;
	static DWORD_PTR dwWeaponNames = *(DWORD_PTR*)0x10A7E28;
	static DWORD_PTR dwWindowHandle = 0x2B6ED88;
	static DWORD_PTR dwServerSession = 0x12A7240;
	static DWORD_PTR dwServerID = 0x11D0ACC;
	static DWORD_PTR dwWindow = 0x2956244;
	static DWORD_PTR dwConnectionState = 0x11C7848;

	static DWORD_PTR dwLocalClientIsInGame = bIsSteamVersion ? 0x5922F0 : 0x402F80;
	static DWORD_PTR dwGetDObj = bIsSteamVersion ? 0x5D2590 : 0x4DA190;
	static DWORD_PTR dwRegisterTag = bIsSteamVersion ? 0x479B40 : 0x5EE820;
	static DWORD_PTR dwGetTagPosition = bIsSteamVersion ? 0x664930 : 0x4A1210;
	static DWORD_PTR dwGetScreenPlacement = bIsSteamVersion ? 0x4C7EF0 : 0x6F6AF0;
	static DWORD_PTR dwWorldToScreen = bIsSteamVersion ? 0x595690 : 0x429A80;
	static DWORD_PTR dwEntityIsTeammate = bIsSteamVersion ? 0x50EC80 : 0x4309A0;
	static DWORD_PTR dwGetPlayerViewOrigin = bIsSteamVersion ? 0x580890 : 0x640E80;
	static DWORD_PTR dwEvaluateTrajectory = bIsSteamVersion ? 0x47F7A0 : 0x44CE20;
	static DWORD_PTR dwWeaponIsVehicle = bIsSteamVersion ? 0x5B8AD0 : 0x5AE8C0;
	static DWORD_PTR dwWeaponAmmoAvailable = bIsSteamVersion ? 0x557C40 : 0x6DF580;
	static DWORD_PTR dwSetZoomState = bIsSteamVersion ? 0x42BD00 : 0x583950;
	static DWORD_PTR dwWeaponIsAkimbo = bIsSteamVersion ? 0x6625B0 : 0x52E780;
	static DWORD_PTR dwGetWeaponDef = bIsSteamVersion ? 0x5846F0 : 0x5225F0;
	static DWORD_PTR dwGetPenetrateType = bIsSteamVersion ? 0x62E290 : 0x4AC220;
	static DWORD_PTR dwBulletTrace = bIsSteamVersion ? 0x7E0170 : 0x7E0A00;
	static DWORD_PTR dwGetWeaponDamageForRange = bIsSteamVersion ? 0x6D85B0 : 0x676160;
	static DWORD_PTR dwGetWeaponHitLocationMultiplier = bIsSteamVersion ? 0x472780 : 0x669830;
	static DWORD_PTR dwGetTraceHitType = bIsSteamVersion ? 0x4AC3D0 : 0x6A32F0;
	static DWORD_PTR dwEntityIsDeployedRiotshield = bIsSteamVersion ? 0x45D4D0 : 0x501160;
	static DWORD_PTR dwHasPerk = bIsSteamVersion ? 0x42BD20 : 0x530C10;
	static DWORD_PTR dwGetSurfacePenetrationDepth = bIsSteamVersion ? 0x572480 : 0x622CE0;
	static DWORD_PTR dwAdvanceTrace = bIsSteamVersion ? 0x5B0F60 : 0x703090;
	static DWORD_PTR dwLocationalTrace = bIsSteamVersion ? 0x6C6C00 : 0x50B870;
	static DWORD_PTR dwAddReliableCommand = bIsSteamVersion ? 0x5E58E0 : 0x6A1C40;
	static DWORD_PTR dwCbufAddText = bIsSteamVersion ? 0x5BDF70 : 0x5C6F10;
	static DWORD_PTR dwGetSpreadForWeapon = bIsSteamVersion ? 0x402A80 : 0x5B0BE0;
	static DWORD_PTR dwHashSeed = bIsSteamVersion ? 0x543CE0 : 0x6AACC0;
	static DWORD_PTR dwRandomFloat = bIsSteamVersion ? 0x6A3490 : 0x4947E0;
	static DWORD_PTR dwSeedRandom = bIsSteamVersion ? 0x4F38A0 : 0x556180;
	static DWORD_PTR dwSetFovSensitivityScale = bIsSteamVersion ? 0x40BB00 : 0x437730;
	static DWORD_PTR dwGetViewmodelWeaponIndex = bIsSteamVersion ? 0x625770 : 0x54A0D0;
	static DWORD_PTR dwWeaponHasPerk = bIsSteamVersion ? 0x449D90 : 0x47ED20;
	static DWORD_PTR dwUsingSniperScope = bIsSteamVersion ? 0x646700 : 0x6C5960;
	static DWORD_PTR dwSetUserCmdAimValues = bIsSteamVersion ? 0x6E7F80 : 0x67ACC0;
	static DWORD_PTR dwGetLastWeaponForAlt = bIsSteamVersion ? 0x4760B0 : 0x480290;
	static DWORD_PTR dwSetUserCmdWeapons = bIsSteamVersion ? 0x552A60 : 0x518D70;
	static DWORD_PTR dwSetExtraButtons = bIsSteamVersion ? 0x4840B0 : 0x6994D0;
	static DWORD_PTR dwGetWeaponName = bIsSteamVersion ? 0x458190 : 0x9502B0;
	static DWORD_PTR dwGetCurrentSession = bIsSteamVersion ? 0x4823F0 : 0x534520;
	static DWORD_PTR dwIsUserRegistered = bIsSteamVersion ? 0x6181A0 : 0x44D670;
	static DWORD_PTR dwGetPlayerAddr = bIsSteamVersion ? 0x701320 : 0x641180;
	static DWORD_PTR dwGetPlayerXuid = bIsSteamVersion ? 0x45C710 : 0x667A00;
	static DWORD_PTR dwJoinSessionFromXuid = bIsSteamVersion ? 0x6EB1E0 : 0x43D980;
	static DWORD_PTR dwCycleWeapon = bIsSteamVersion ? 0x69BAE0 : 0x57D850;
	static DWORD_PTR dwFindVar = bIsSteamVersion ? 0x42D410 : 0x563A70;
	static DWORD_PTR dwFindHash = bIsSteamVersion ? 0xA5E1C0 : 0xA5E1D0;
	static DWORD_PTR dwHashMemory = bIsSteamVersion ? 0xA61B00 : 0xA61B10;
	static DWORD_PTR dwRegisterPrng = bIsSteamVersion ? 0xA61F10 : 0xA61F20;
	static DWORD_PTR dwFindPrng = bIsSteamVersion ? 0xA61EA0 : 0xA61EB0;
	static DWORD_PTR dwSignHash = bIsSteamVersion ? 0xA61C00 : 0xA61C10;

	static DWORD_PTR dwTigerHash = bIsSteamVersion ? 0xC81AF8 : 0xC273A4;
	static DWORD_PTR dwPrng = bIsSteamVersion ? 0xCC8030 : 0xCC8090;
	static DWORD_PTR dwSprng = bIsSteamVersion ? 0xC52214 : 0xBD590C;
	static DWORD_PTR dwKey = bIsSteamVersion ? 0x301EB88 : 0x2FFDB88;

	static DWORD_PTR dwRankXP = bIsSteamVersion ? 0x2FB176C : 0x2F9076C;
	static DWORD_PTR dwPLevel = bIsSteamVersion ? 0x2FB1754 : 0x2F90754;

	static DWORD_PTR dwStatsBase = bIsSteamVersion ? 0x2F00000 : 0x2EDF000;
	static DWORD_PTR dwMemoryBase = bIsSteamVersion ? 0x24A0000 : 0x247F000;

	static DWORD_PTR dwSwapChain = bIsSteamVersion ? 0x3606F94 : 0x35E5F94;
	static DWORD_PTR dwMouseInput = 0x2B69969;

	static DWORD_PTR dwTacSSCheck = 0x2B68838;
	static DWORD_PTR dwTacSSPatch = 0x2B68824;
	static DWORD_PTR dwTacSSHandle = bIsSteamVersion ? 0x3A458F4 : 0x3A248F4;
	static DWORD_PTR dwXnAddr = bIsSteamVersion ? 0x34389B0 : 0x34179B0;

	static DWORD_PTR dwPresent = (bIsSteamVersion && bGameOverlayRenderer) ?
		ReadPointer(FindPattern((DWORD_PTR)hGameOverlayRenderer.lpBaseOfDll, (DWORD_PTR)hGameOverlayRenderer.SizeOfImage, "\xFF\x15\x00\x00\x00\x00\x5B\x5D\xC2\x0C\x00", "xx????xxxxx"), 0x2) :
		*(DWORD_PTR*)dwSwapChain;
	/*
	//=====================================================================================
	*/
#define CG ((sCG*)dwCG)
#define ClientActive (*(sClientActive**)dwClientActive)
#define WeaponNames ((sWeaponName*)dwWeaponNames)
#define ViewAngles ((sViewAngles*)dwViewAngles)
#define RecoilAngles ((sRecoilAngles*)dwRecoilAngles)
#define ServerSession ((sServerSession*)dwServerSession)
#define Window ((sWindow*)dwWindow)
	/*
	//=====================================================================================
	*/
	template <typename... Parameters>
	FORCEINLINE void Com_Error(eErrorParam code, LPCSTR format, Parameters... params)
	{
		return VariadicCall<void>(dwComError, code, format, params...);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE DWORD Sys_GetValue(int value)
	{
		return VariadicCall<DWORD>(dwSysGetValue, value);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE DWORD Sys_Milliseconds()
	{
		return VariadicCall<DWORD>(dwSysMilliseconds);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE DWORD RegisterShader(std::string name)
	{
		return VariadicCall<DWORD>(dwRegisterShader, name.c_str(), 7, 1, -1);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE bool PrepFireParams(sCEntity* entity, WORD tag, int weapon, int eventnum, bool player, sBulletFireParams* fireparams, ImVec3* tracerstart, int* shots, float* range, sOrientation* orientation, ImVec3* origin, float* spread, int* ignorenum)
	{
		return VariadicCall<bool>(dwPrepFireParams, 0, entity, tag, &CG->PlayerState, weapon, eventnum, player, fireparams, tracerstart, shots, range, orientation, origin, spread, ignorenum);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE LPSTR GetUsername()
	{
		return VariadicCall<LPSTR>(dwGetUsername, 0);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE LPSTR GetClantag()
	{
		return VariadicCall<LPSTR>(dwGetClantag, 0);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE LPSTR GetXuidstring()
	{
		return VariadicCall<LPSTR>(dwGetXuidstring, 0);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE void PopOverlayForSteamID(QWORD steamid)
	{
		return VariadicCall<void>(dwPopOverlayForSteamID, steamid);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE int GetIntPlayerStatInternal(int pathdepth, LPSTR* statpath, LPSTR buffer)
	{
		return VariadicCall<int>(dwGetIntPlayerStatInternal, pathdepth, statpath, buffer);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE int Int64ToString(QWORD int64, LPSTR buffer)
	{
		return VariadicCall<int>(dwInt64ToString, int64, buffer);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE bool LocalClientIsInGame()
	{
		return VariadicCall<bool>(dwLocalClientIsInGame, 0);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE LPVOID GetDObj(sCEntity* entity)
	{
		return VariadicCall<LPVOID>(dwGetDObj, entity->NextEntityState.iEntityNum, entity->wUsedForPlayerMesh);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE WORD RegisterTag(std::string name)
	{
		return VariadicCall<WORD>(dwRegisterTag, name.c_str());
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE LPVOID GetTagPosition(sCEntity* entity, WORD tag, LPVOID dobj, ImVec3* position)
	{
		return VariadicCall<LPVOID>(dwGetTagPosition, entity, dobj, tag, position);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE sScreenPlacement* GetScreenPlacement()
	{
		return VariadicCall<sScreenPlacement*>(dwGetScreenPlacement, 0);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE bool WorldToScreen(ImVec3* world, ImVec2* screen)
	{
		return VariadicCall<bool>(dwWorldToScreen, 0, world, screen);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE bool EntityIsTeammate(sCEntity* entity)
	{
		return VariadicCall<bool>(dwEntityIsTeammate, 0, entity);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE void GetPlayerViewOrigin(ImVec3* position)
	{
		return VariadicCall<void>(dwGetPlayerViewOrigin, 0, &CG->PlayerState, position);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE void EvaluateTrajectory(sTrajectory* trajectory, int time, ImVec3* result)
	{
		return VariadicCall<void>(dwEvaluateTrajectory, trajectory, time, result);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE bool WeaponIsVehicle()
	{
		return VariadicCall<bool>(dwWeaponIsVehicle, &CG->PlayerState);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE int WeaponAmmoAvailable()
	{
		return VariadicCall<int>(dwWeaponAmmoAvailable, &CG->PlayerState);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE int SetZoomState(bool enable)
	{
		return VariadicCall<int>(dwSetZoomState, 0, enable);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE bool WeaponIsAkimbo()
	{
		return VariadicCall<bool>(dwWeaponIsAkimbo, CG->iWeaponID);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE sWeaponDef* GetWeaponDef(int weapon)
	{
		return VariadicCall<sWeaponDef*>(dwGetWeaponDef, weapon);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE ePenetrateType GetPenetrateType(int weapon)
	{
		return VariadicCall<ePenetrateType>(dwGetPenetrateType, weapon);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE int GetWeaponDamageForRange(int weapon, ImVec3* startpos, ImVec3* hitpos)
	{
		return VariadicCall<int>(dwGetWeaponDamageForRange, weapon, startpos, hitpos);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE float GetWeaponHitLocationMultiplier(int hitloc, int weapon)
	{
		return VariadicCall<float>(dwGetWeaponHitLocationMultiplier, hitloc, weapon);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE WORD GetTraceHitType(sBulletTraceResults* traceresults)
	{
		return VariadicCall<WORD>(dwGetTraceHitType, traceresults);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE bool EntityIsDeployedRiotshield(sCEntity* entity)
	{
		return VariadicCall<bool>(dwEntityIsDeployedRiotshield, entity);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE bool HasPerk(int perk)
	{
		return VariadicCall<bool>(dwHasPerk, 0, CG->iClientNum, perk);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE float GetSurfacePenetrationDepth(ePenetrateType penetration, int surface)
	{
		return VariadicCall<float>(dwGetSurfacePenetrationDepth, penetration, surface);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE bool AdvanceTrace(sBulletFireParams* fireparams, sBulletTraceResults* traceresults, float distance)
	{
		return VariadicCall<bool>(dwAdvanceTrace, fireparams, traceresults, distance);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE int LocationalTrace(sTrace* trace, ImVec3* from, ImVec3* to, int skip, int mask)
	{
		return VariadicCall<int>(dwLocationalTrace, trace, from, to, skip, mask, 0, 0);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE void AddReliableCommand(std::string command)
	{
		return VariadicCall<void>(dwAddReliableCommand, 0, command.c_str());
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE void Cbuf_AddText(std::string command)
	{
		return VariadicCall<void>(dwCbufAddText, 0, command.c_str());
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE void GetSpreadForWeapon(int weapon, float* minimum, float* maximum)
	{
		return VariadicCall<void>(dwGetSpreadForWeapon, &CG->PlayerState, weapon, minimum, maximum);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE void HashSeed(int* servertime)
	{
		return VariadicCall<void>(dwHashSeed, servertime);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE float RandomFloat(int* seed)
	{
		return VariadicCall<float>(dwRandomFloat, seed);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE void SeedRandom(int* seed)
	{
		return VariadicCall<void>(dwSeedRandom, seed);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE void SetFovSensitivityScale(float scale)
	{
		return VariadicCall<void>(dwSetFovSensitivityScale, 0, scale);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE int GetViewmodelWeaponIndex()
	{
		return VariadicCall<int>(dwGetViewmodelWeaponIndex, &CG->PlayerState);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE bool WeaponHasPerk(int weapon, int perk)
	{
		return VariadicCall<bool>(dwWeaponHasPerk, weapon, perk);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE int UsingSniperScope()
	{
		return VariadicCall<int>(dwUsingSniperScope, &CG->PlayerState);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE void SetUserCmdAimValues(ImVec3* angles)
	{
		return VariadicCall<void>(dwSetUserCmdAimValues, 0, angles);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE int GetLastWeaponForAlt()
	{
		return VariadicCall<int>(dwGetLastWeaponForAlt, CG, &CG->PlayerState, CG->iWeaponSelect);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE void SetUserCmdWeapons(int weapon)
	{
		return VariadicCall<void>(dwSetUserCmdWeapons, 0, CG->iWeaponSelect, CG->iEquippedOffhand, weapon);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE void SetExtraButtons()
	{
		return VariadicCall<void>(dwSetExtraButtons, 0, &CG->iExtraButtonBits);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE LPSTR GetWeaponName(int weapon, LPSTR buffer, int length)
	{
		return VariadicCall<LPSTR>(dwGetWeaponName, weapon, buffer, length);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE LPVOID GetCurrentSession()
	{
		return VariadicCall<LPVOID>(dwGetCurrentSession);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE bool IsUserRegistered(LPVOID session, int clientnum)
	{
		return VariadicCall<bool>(dwIsUserRegistered, session, clientnum);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE sNetAddr* GetPlayerAddr(sNetAddr* netaddr, LPVOID session, int clientnum)
	{
		return VariadicCall<sNetAddr*>(dwGetPlayerAddr, netaddr, session, clientnum);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE QWORD GetPlayerXuid(LPVOID session, int clientnum)
	{
		return VariadicCall<QWORD>(dwGetPlayerXuid, session, clientnum);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE bool JoinSessionFromXuid(QWORD xuid)
	{
		return VariadicCall<bool>(dwJoinSessionFromXuid, 0, xuid);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE void CycleWeapon(int cycle)
	{
		return VariadicCall<void>(dwCycleWeapon, 0, cycle);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE sDvar* FindVar(std::string name)
	{
		return VariadicCall<sDvar*>(dwFindVar, name.c_str());
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE int FindHash(std::string name)
	{
		return VariadicCall<int>(dwFindHash, name.c_str());
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE int HashMemory(int hash, LPCSTR in, int inlen, LPSTR out, int* outlen)
	{
		return VariadicCall<int>(dwHashMemory, hash, in, inlen, out, outlen);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE int RegisterPrng(LPVOID prng)
	{
		return VariadicCall<int>(dwRegisterPrng, prng);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE int FindPrng(std::string name)
	{
		return VariadicCall<int>(dwFindPrng, name.c_str());
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE int SignHash(LPCSTR in, int inlen, LPSTR out, int* outlen, LPVOID prng, int wprng, LPVOID key)
	{
		return VariadicCall<int>(dwSignHash, in, inlen, out, outlen, prng, wprng, key);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE bool LocalPlayerIsValid()
	{
		return ((CG->CEntity[CG->iClientNum].iAlive & 2) && !(CG->CEntity[CG->iClientNum].NextEntityState.LerpEntityState.eFlags1 & EF1_DEAD));
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE bool EntityIsValid(sCEntity* entity)
	{
		return ((entity != &CG->CEntity[CG->iClientNum]) && (entity->iAlive & 2) && !(entity->NextEntityState.LerpEntityState.eFlags1 & EF1_DEAD));
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE bool ClientIsAlive(sClientInfo* client)
	{
		return (!client->bDead && strcmp(client->szPlayerModel, ""));
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE bool IsPlayerReloading()
	{
		return ((UINT)(CG->PlayerState.iWeaponState[0] - 11) < 8 || (UINT)(CG->PlayerState.iWeaponState[1] - 11) < 8);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE bool EntityHasRiotShield(sCEntity* entity)
	{
		return ((BYTE)entity->NextEntityState.iWeaponID == ID_ASSAULTSHIELD || (BYTE)entity->NextEntityState.LerpEntityState.iWeaponID2 == ID_ASSAULTSHIELD);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE bool IsThirdPerson()
	{
		return (CG->iThirdPerson || CG->PlayerState.iThirdPerson);
	}
}

//=====================================================================================