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
#define FloatToByte(a) ((BYTE)(a)*255.0f)
#define DwordFromBytes(a) (((BYTE)(a)[0])|((BYTE)(a)[1]<<8)|((BYTE)(a)[2]<<16)|((BYTE)(a)[3]<<24))
#define GetSign(a) ((a)?((*(int*)&(a)>>31)|1):0)
#define DegreesToRadians(a) ((a)*((float)M_PI/180.0f))
#define RadiansToDegrees(a) ((a)*(180.0f/(float)M_PI))
#define AngleToShort(a) ((int)((a)*(65536/360.0f))&65535)
#define ShortToAngle(a) ((float)((a)*(360.0f/65536)))
#define AngleNormalize(a) (ShortToAngle(AngleToShort((a))))
#define Dereference(a) (*(decltype(a)*)(a))

//=====================================================================================

namespace ProtoGenesys
{
	enum eBone
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
	};
	/*
	//=====================================================================================
	*/
	enum eEntityType
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
	};
	/*
	//=====================================================================================
	*/
	enum eTraceHitType
	{
		TRACE_HITTYPE_NONE,
		TRACE_HITTYPE_ENTITY,
		TRACE_HITTYPE_DYNENT_MODEL,
		TRACE_HITTYPE_DYNENT_BRUSH,
		TRACE_HITTYPE_GLASS,
		TRACE_HITTYPE_MAX
	};
	/*
	//=====================================================================================
	*/
	enum ePenetrateType
	{
		PENETRATE_TYPE_NONE,
		PENETRATE_TYPE_SMALL,
		PENETRATE_TYPE_MEDIUM,
		PENETRATE_TYPE_LARGE,
		PENETRATE_TYPE_MAX
	};
	/*
	//=====================================================================================
	*/
	enum eWeaponType
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
	};
	/*
	//=====================================================================================
	*/
	enum eTeam
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
	};
	/*
	//=====================================================================================
	*/
	enum eErrorParam
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
	};
	/*
	//=====================================================================================
	*/
	enum eWeaponDobjInfoSlot
	{
		HELD,
		STOWED,
		MELEE,
	};
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
	struct sDvar
	{
	public:
		LPSTR szName;
	private:
		char _0x4[0x14];
	public:
		union uDvarValue
		{
			bool bValue;
			std::int32_t iValue;
			std::uint32_t dwValue;
			std::uint64_t qwValue;
			std::float_t flValue;
			ImVec4 cValue;
			LPSTR szValue;
		} Current, Latched, Reset;
		union uDvarLimits
		{
			struct
			{
				std::int32_t iMin;
				std::int32_t iMax;
			};
			struct
			{
				std::uint32_t dwMin;
				std::uint32_t dwMax;
			};
			struct
			{
				std::uint64_t qwMin;
				std::uint64_t qwMax;
			};
			struct
			{
				std::float_t flMin;
				std::float_t flMax;
			};
		} Domain;
	private:
		char _0x58[0x8];
	};
	/*
	//=====================================================================================
	*/
	struct sPlayerState
	{
	public:
		int iServerTime;
	private:
		char _0x4[0x14];
	public:
		int iOtherFlags;
	private:
		char _0x1C[0xC];
	public:
		ImVec3 vOrigin;
		ImVec3 vVelocity;
	private:
		char _0x40[0xC];
	public:
		float flWeaponTime;
		int iWeaponDelay;
	private:
		char _0x54[0x30];
	public:
		int iThirdPerson;
	private:
		char _0x88[0x4];
	public:
		int iGravity;
	private:
		char _0x90[0x4];
	public:
		int iSpeed;
		ImVec3 vDeltaAngles;
	private:
		char _0xA4[0x12C];
	public:
		int iWeaponState[2];
	private:
		char _0x1D8[0x8];
	public:
		float flZoomProgress;
	private:
		char _0x1E4[0x4];
	public:
		float flSpreadOverride;
		int iSpreadOverrideState;
	private:
		char _0x1F0[0x8];
	public:
		ImVec3 vViewAngles;
		int iPlayerHeight;
		ImVec3 vPlayerHeight;
	private:
		char _0x214[0x18];
	public:
		int iDamageYaw;
		int iDamagePitch;
		int iDamageCount;
	private:
		char _0x238[0x8];
	public:
		int iHealth;
	private:
		char _0x244[0x4];
	public:
		int iMaxHealth;
	private:
		char _0x24C[0x74];
	public:
		int iSecondaryWeaponID;
	private:
		char _0x2C4[0x18];
	public:
		int iPrimaryWeaponID;
	private:
		char _0x2E0[0x124];
	public:
		int iSecondaryAmmoStock;
		int iPrimaryAmmoStock;
	private:
		char _0x40C[0x34];
	public:
		int iSecondaryAmmoClip;
		int iPrimaryAmmoClip;
		int iLethalAmmo;
		int iTacticalAmmo;
	private:
		char _0x450[0x5C];
	public:
		int iSatalliteTypeEnabled;
	private:
		char _0x4B0[0xA0];
	public:
		int iPerkFlags;
	};
	/*
	//=====================================================================================
	*/
	struct sRefDef
	{
	public:
		int iX;
		int iY;
		int iWidth;
		int iHeight;
	private:
		char _0x10[0x10];
	public:
		float flFovX;
		float flFovY;
	private:
		char _0x28[0x8];
	public:
		float flFov;
		ImVec3 vViewOrigin;
	private:
		char _0x40[0x4];
	public:
		ImVec3 vViewAxis[3];
	private:
		char _0x68[0x16D18];
	public:
		int iSplitScreenBlurEdges;
		int iPlayerTeleported;
	private:
		char _0x16D88[0x2D8];
	};
	/*
	//=====================================================================================
	*/
	struct sWeaponDef
	{
	private:
		char _0x0[0x1C];
	public:
		eWeaponType iWeaponType;
	private:
		char _0x20[0x4];
	public:
		ePenetrateType iPenetrateType;
	private:
		char _0x28[0x609];
	public:
		bool bRifleBullet;
	private:
		char _0x632[0x1AE];
	public:
		float flADSSpread;
	};
	/*
	//=====================================================================================
	*/
	struct sModelMask
	{
	public:
		int iSlot;
		sWeaponDef* WeaponDef;
	private:
		char _0x8[0x8];
	};
	/*
	//=====================================================================================
	*/
	struct sClientInfo
	{
	public:
		int iInfoValid;
		int iNextValid;
		int iClientNum;
		char szName[32];
		eTeam iTeam1;
		eTeam iTeam2;
		int iFFATeam;
	private:
		char _0x38[0x28];
	public:
		int iRank;
	private:
		char _0x64[0x14];
	public:
		unsigned __int64 qwXuid;
		char szClan[8];
	private:
		char _0x88[0x7];
	public:
		bool bDead;
	private:
		char _0x90[0x4];
	public:
		int iScore;
	private:
		char _0x98[0x58];
	public:
		char szPlayerModel[32];
	private:
		char _0x110[0x3B4];
	public:
		ImVec3 vViewAngles;
	private:
		char _0x4D0[0xF4];
	public:
		int iWeaponID;
	private:
		char _0x5C8[0x1BC];
	public:
		sModelMask ModelMask[8];
		int iOffHandWeaponID;
	};
	/*
	//=====================================================================================
	*/
	struct sTrajectory
	{
	public:
		int iType;
		int iTime;
		int iDuration;
		ImVec3 vBase;
		ImVec3 vDelta;
	};
	/*
	//=====================================================================================
	*/
	struct sLerpEntityState
	{
	public:
		int eFlags1;
		int eFlags2;
		sTrajectory PositionTrajectory;
		sTrajectory AngleTrajectory;
	private:
		char _0x50[0xC];
	public:
		int iWeaponID1;
		int iWeaponID2;
	private:
		char _0x64[0x18];
	};
	/*
	//=====================================================================================
	*/
	struct sEntityState
	{
	public:
		int iEntityNum;
		sLerpEntityState LerpEntityState;
	private:
		char _0x80[0x58];
	public:
		short wEntityType;
		short wGroundEntityNum;
	private:
		char _0xDA[0x2];
	public:
		short wOtherEntityNum;
		short wAttackerEntityNum;
	private:
		char _0xE2[0x2];
	public:
		int iWeaponID;
	private:
		char _0xE8[0x10];
	};
	/*
	//=====================================================================================
	*/
	struct sCEntity
	{
	private:
		char _0x0[0x2];
	public:
		short wValid;
		short wUsedForPlayerMesh;
	private:
		char _0x6[0x26];
	public:
		ImVec3 vOrigin;
		ImVec3 vViewAngles;
	private:
		char _0x44[0x11C];
	public:
		sLerpEntityState CurrentEntityState;
		sEntityState NextEntityState;
	private:
		char _0x2D4[0xA4];
	public:
		int iAlive;
	private:
		char _0x37C[0x4];
	};
	/*
	//=====================================================================================
	*/
	struct sSnapShot
	{
	private:
		char _0x0[0x8];
	public:
		int iServerTime;
	private:
		char _0xC[0xC];
	public:
		int iPing;
	};
	/*
	//=====================================================================================
	*/
	struct sCG
	{
	public:
		int iClientNum;
	private:
		char _0x4[0x20];
	public:
		sSnapShot* OldSnapShot;
		sSnapShot* NewSnapShot;
	private:
		char _0x2C[0x58];
	public:
		int iPing;
		int iServerTime;
		int iPhysicsTime;
		int iCommandTime;
	private:
		char _0x94[0x24];
	public:
		ImVec3 vOrigin;
		ImVec3 vVelocity;
	private:
		char _0xD0[0x10];
	public:
		int iWeaponDelay;
	private:
		char _0xE4[0x78];
	public:
		int iMoveType;
	private:
		char _0x160[0x2C];
	public:
		int iMoveFlag;
	private:
		char _0x190[0xB8];
	public:
		int iWeaponID;
	private:
		char _0x24C[0x14];
	public:
		int iWeaponState[2];
	private:
		char _0x268[0x8];
	public:
		float flZoomProgress;
	private:
		char _0x274[0x54];
	public:
		int iHealth;
	private:
		char _0x2CC[0x47DB8];
	public:
		float flFrameInterpolation;
		int iFrameTime;
		int iTime;
		int iOldTime;
	private:
		char _0x48094[0x8];
	public:
		int iThirdPerson;
	private:
		char _0x480A0[0x8];
	public:
		sPlayerState PlayerState;
	private:
		char _0x485FC[0x5294];
	public:
		sRefDef RefDef;
		ImVec3 vRefDefViewAngles;
	private:
		char _0x648FC[0x130];
	public:
		ImVec3 vWeaponAngles;
	private:
		char _0x64A38[0x3890];
	public:
		float flZoomSensitivity;
	private:
		char _0x682CC[0x68C];
	public:
		int iMatchUIVisibilityFlags;
	private:
		char _0x6895C[0xD18];
	public:
		int iWeaponSelect;
	private:
		char _0x69678[0x54];
	public:
		int iEquippedOffhand;
	private:
		char _0x696D0[0xF8];
	public:
		ImVec3 vKickAngles;
		ImVec3 vOffsetAngles;
		float flGunPitch;
		float flGunYaw;
	private:
		char _0x697E8[0xB4];
	public:
		float flShockSensitivity;
	private:
		char _0x698A0[0x1C0];
	public:
		sClientInfo ClientInfo[MAX_CLIENTS];
	private:
		char _0x72AF0[0xE04C];
	public:
		int iExtraButtonBits[2];
	private:
		char _0x80B44[0x8];
	public:
		int iPlayerTeleported;
	private:
		char _0x80B50[0xC];
	public:
		float flSpread;
	private:
		char _0x80B60[0x6BD0];
	public:
		char szGameType[4];
	private:
		char _0x87734[0x1D90C];
	public:
		sCEntity CEntity[MAX_ENTITIES];
	};
	/*
	//=====================================================================================
	*/
	struct sUserCmd
	{
	public:
		int iServerTime;
		int iButtons[2];
		int iViewAngles[3];
	private:
		char _0x18[0xC];
	public:
		char szForwardMove;
		char szRightMove;
	private:
		char _0x26[0x16];
	};
	/*
	//=====================================================================================
	*/
	struct sClientActive
	{
	private:
		char _0x0[0x68];
	public:
		int iPing;
	private:
		char _0x6C[0x42C3C];

	public:
		sUserCmd UserCmd[128];
		int iCurrentCmd;

		sUserCmd* GetUserCmd(int number)
		{
			return &UserCmd[number & 0x7F];
		}
	};
	/*
	//=====================================================================================
	*/
	struct sWeaponName
	{
	private:
		char _0x0[0x18];
	public:
		LPSTR szDisplayName;
	private:
		char _0x1C[0x8];
	};
	/*
	//=====================================================================================
	*/
	struct sViewAngles
	{
	public:
		ImVec3 vAngles;
	};
	/*
	//=====================================================================================
	*/
	struct sRecoilAngles
	{
	public:
		ImVec3 vAngles;
	};
	/*
	//=====================================================================================
	*/
	struct sWindow
	{
	public:
		int iWidth;
		int iHeight;
	};
	/*
	//=====================================================================================
	*/
	struct sTrace
	{
	public:
		ImVec3 vNormal;
	private:
		char _0xC[0x4];
	public:
		float flFraction;
		int iSurfaceFlags;
	private:
		char _0x18[0x8];
	public:
		short wHitID;
	private:
		char _0x22[0x6];
	public:
		short wPartGroup;
		char bAllSolid;
		char bStartSolid;
	private:
		char _0x2C[0x14];
	};
	/*
	//=====================================================================================
	*/
	struct sBulletTraceResults
	{
	public:
		sTrace Trace;
	private:
		char _0x40[0x4];
	public:
		ImVec3 vHitPos;
		char iIgnoreHitEnt;
	private:
		char _0x51[0x3];
	public:
		int iSurfaceType;
	private:
		char _0x58[0x8];
	};
	/*
	//=====================================================================================
	*/
	struct sBulletFireParams
	{
	public:
		int iMaxEntNum;
		int iEntityNum;
		float flPower;
		int iBulletType;
		ImVec3 vViewOrigin;
		ImVec3 vStart;
		ImVec3 vEnd;
		ImVec3 vDir;
	};
	/*
	//=====================================================================================
	*/
	struct sOrientation
	{
	public:
		ImVec3 vOrigin;
		ImVec3 vAxis[3];
	};
	/*
	//=====================================================================================
	*/
	struct sNetAddr
	{
	public:
		char szIPAddress[4];
		short iPort;
		int iType;
		int iLocalNetID;
		short iServerID;
	};
	/*
	//=====================================================================================
	*/
	struct sScreenPlacement
	{
	public:
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
	};
	/*
	//=====================================================================================
	*/
	struct sPlayerHeldWeapon
	{
	public:
		int iWeapon;
		int iOptions;
		float flHeatPercent;
		int iFuelTankTime;
		int iAdsZoomSelect;
		bool bOverHeating;
		bool bNeedsRechamber;
		bool bHeldBefore;
		bool bQuickReload;
		bool bLockWeaponPickup;
		char szModel;
	private:
		char _0x1A[0x2];
	};
	/*
	//=====================================================================================
	*/
	struct sWeaponized
	{
	public:
		float flReflection;
	private:
		char _0x4[0x1C];
	public:
		float flLayer3Scroll;
	private:
		char _0x24[0x1C];
	public:
		float flEmissiveAmount;
	private:
		char _0x44[0xFC];
	public:
		float flLayer1Scroll;
	private:
		char _0x144[0x7C];
	public:
		float flLayer2Scroll;
	private:
		char _0x1C4[0x11C];
	public:
		float flGlossAmount;
	private:
		char _0x2E4[0xEC];
	};
	/*
	//=====================================================================================
	*/
	static MODULEINFO hSteamAPI;
	static MODULEINFO hGameOverlayRenderer;
	static MODULEINFO hT6mp = GetModuleInfo(NULL);

	static bool bIsSteamVersion = hT6mp.SizeOfImage == 0x400D000;

	static std::uintptr_t dwConnectPathsDvar = 0x2366AA4;
	static std::uintptr_t dwMouseAccelerationDvar = 0x11C7834;
	static std::uintptr_t dwDrawBigFPSDvar = 0x1140A08;
	static std::uintptr_t dwMaxClientsDvar = 0x25E456C;
	static std::uintptr_t dwNoDeltaDvar = 0x11C3634;

	static std::uintptr_t dwTransitionPlayerStateCall = bIsSteamVersion ? 0x55CF28 : 0x51DAE8;
	static std::uintptr_t dwGetWorldTagMatrixCall = bIsSteamVersion ? 0x7D53BA : 0x7D5CAA;
	static std::uintptr_t dwGameTypeSettingsCall = bIsSteamVersion ? 0x97CBC2 : 0x97CC42;
	static std::uintptr_t dwOrbitalVsat = 0x12A0CA4;

	static std::uintptr_t dwSysGetValueException = bIsSteamVersion ? 0x635F09 : 0x5EFBB9;
	static std::uintptr_t dwRegisterShaderException = bIsSteamVersion ? 0x6874E9 : 0x45A8C9;
	static std::uintptr_t dwConnectPathsException1 = bIsSteamVersion ? 0x43B6DB : 0x53367B;
	static std::uintptr_t dwConnectPathsException2 = bIsSteamVersion ? 0x4CF0C5 : 0x488335;
	static std::uintptr_t dwObituaryException = bIsSteamVersion ? 0x7A345C : 0x7A40EC;

	static std::uintptr_t dwComError = bIsSteamVersion ? 0x4ECFD0 : 0x58FC30;
	static std::uintptr_t dwSysGetValue = bIsSteamVersion ? 0x635EF0 : 0x5EFBA0;
	static std::uintptr_t dwSysMilliseconds = bIsSteamVersion ? 0x5BD480 : 0x57EC40;
	static std::uintptr_t dwRegisterShader = bIsSteamVersion ? 0x734000 : 0x734CC0;
	static std::uintptr_t dwPrepFireParams = bIsSteamVersion ? 0x6EE050 : 0x638120;
	static std::uintptr_t dwPredictPlayerState = bIsSteamVersion ? 0x5B3C40 : 0x5B4F40;
	static std::uintptr_t dwWritePacket = bIsSteamVersion ? 0x48E610 : 0x439380;
	static std::uintptr_t dwSetValueForKey = bIsSteamVersion ? 0x406830 : 0x551B00;
	static std::uintptr_t dwGetUsername = bIsSteamVersion ? 0x65FDE0 : 0x4B58D0;
	static std::uintptr_t dwGetClantag = bIsSteamVersion ? 0x5ACDA0 : 0x6D2900;
	static std::uintptr_t dwGetXuidstring = bIsSteamVersion ? 0x534D60 : 0x491870;
	static std::uintptr_t dwPopOverlayForSteamID = bIsSteamVersion ? 0x589740 : 0x556680;
	static std::uintptr_t dwGetIntPlayerStatInternal = bIsSteamVersion ? 0x67A430 : 0x52EE10;
	static std::uintptr_t dwInt64ToString = bIsSteamVersion ? 0x57E0F0 : 0x427820;
	static std::uintptr_t dwBulletHitEvent = bIsSteamVersion ? 0x57CED0 : 0x5F72B0;
	static std::uintptr_t dwCalcEntityLerpPositions = bIsSteamVersion ? 0x469870 : 0x6B98D0;
	static std::uintptr_t dwInterpolateEntityPosition = bIsSteamVersion ? 0x7A0000 : 0x7A0C90;
	static std::uintptr_t dwInterpolateEntityAngles = bIsSteamVersion ? 0x6B4100 : 0x4B1D40;
	static std::uintptr_t dwTransitionPlayerState = bIsSteamVersion ? 0x666DD0 : 0x6AB990;
	static std::uintptr_t dwSetupWeaponCamoRender = bIsSteamVersion ? 0x69C560 : 0x4B5B20;
	static std::uintptr_t dwGetWorldTagMatrix = bIsSteamVersion ? 0x47AC00 : 0x4FC740;
	static std::uintptr_t dwGameTypeSettings = bIsSteamVersion ? 0x6C43C0 : 0x4F5F70;
	static std::uintptr_t dwGetAddr = bIsSteamVersion ? 0x628E30 : 0x4D3A70;
	static std::uintptr_t dwGetPlayerStatus = bIsSteamVersion ? 0x8C5DD0 : 0x8C5F30;
	static std::uintptr_t dwIsValidSteamID = bIsSteamVersion ? 0x531AC0 : 0x60EFC0;
	static std::uintptr_t dwCreateScreenShot = bIsSteamVersion ? 0x75BF00 : 0x75CBB0;
	static std::uintptr_t dwAtoiCall1 = bIsSteamVersion ? 0x7D0DC8 : 0x7D16B8;
	static std::uintptr_t dwAtoiCall2 = bIsSteamVersion ? 0x7D0DD7 : 0x7D16C7;
	static std::uintptr_t dwMemcpyCall = bIsSteamVersion ? 0x687460 : 0x45A840;
	static std::uintptr_t dwFriendPersonaNameFromXuid = bIsSteamVersion ? 0x4FEA00 : 0x6286B0;
	static std::uintptr_t dwGetUserSteamIdAsXuid = bIsSteamVersion ? 0x44A8F0 : 0x5C2CD0;
	static std::uintptr_t dwGetUserSteamIdAsXuidCall = bIsSteamVersion ? 0x924621 : 0x924471;
	static std::uintptr_t dwFilterPersonaNameCall = bIsSteamVersion ? 0x4FEA4F : 0x6286FF;

	static std::uintptr_t dwCG = 0x113F18C;
	static std::uintptr_t dwClientActive = 0x11C7D74;
	static std::uintptr_t dwViewAngles = 0xFE64393C;
	static std::uintptr_t dwRecoilAngles = 0xFE6438B0;
	static std::uintptr_t dwWeaponNames = 0x10A7E28;
	static std::uintptr_t dwWindowHandle = 0x2B6ED88;
	static std::uintptr_t dwServerID = 0x11D0ACC;
	static std::uintptr_t dwWindow = 0x2956244;
	static std::uintptr_t dwConnectionState = 0x11C7848;

	static std::uintptr_t dwLocalClientIsInGame = bIsSteamVersion ? 0x5922F0 : 0x402F80;
	static std::uintptr_t dwGetDObj = bIsSteamVersion ? 0x5D2590 : 0x4DA190;
	static std::uintptr_t dwRegisterTag = bIsSteamVersion ? 0x479B40 : 0x5EE820;
	static std::uintptr_t dwGetTagPosition = bIsSteamVersion ? 0x664930 : 0x4A1210;
	static std::uintptr_t dwGetScreenPlacement = bIsSteamVersion ? 0x4C7EF0 : 0x6F6AF0;
	static std::uintptr_t dwEntityIsTeammate = bIsSteamVersion ? 0x50EC80 : 0x4309A0;
	static std::uintptr_t dwGetTeamColor = bIsSteamVersion ? 0x40AA70 : 0x5C02D0;
	static std::uintptr_t dwGetPlayerViewOrigin = bIsSteamVersion ? 0x580890 : 0x640E80;
	static std::uintptr_t dwEvaluateTrajectory = bIsSteamVersion ? 0x47F7A0 : 0x44CE20;
	static std::uintptr_t dwWeaponIsVehicle = bIsSteamVersion ? 0x5B8AD0 : 0x5AE8C0;
	static std::uintptr_t dwWeaponAmmoAvailable = bIsSteamVersion ? 0x557C40 : 0x6DF580;
	static std::uintptr_t dwGetAmmoNotInClip = bIsSteamVersion ? 0x63ADD0 : 0x471050;
	static std::uintptr_t dwGetClipSize = bIsSteamVersion ? 0x699C30 : 0x5B4410;
	static std::uintptr_t dwGetHeldWeapon = bIsSteamVersion ? 0x4420E0 : 0x4368E0;
	static std::uintptr_t dwSetZoomState = bIsSteamVersion ? 0x42BD00 : 0x583950;
	static std::uintptr_t dwWeaponIsAkimbo = bIsSteamVersion ? 0x6625B0 : 0x52E780;
	static std::uintptr_t dwGetWeaponDef = bIsSteamVersion ? 0x5846F0 : 0x5225F0;
	static std::uintptr_t dwGetPenetrateType = bIsSteamVersion ? 0x62E290 : 0x4AC220;
	static std::uintptr_t dwBulletTrace = bIsSteamVersion ? 0x7E0170 : 0x7E0A00;
	static std::uintptr_t dwGetWeaponDamageForRange = bIsSteamVersion ? 0x6D85B0 : 0x676160;
	static std::uintptr_t dwGetWeaponHitLocationMultiplier = bIsSteamVersion ? 0x472780 : 0x669830;
	static std::uintptr_t dwGetTraceHitType = bIsSteamVersion ? 0x4AC3D0 : 0x6A32F0;
	static std::uintptr_t dwEntityIsDeployedRiotshield = bIsSteamVersion ? 0x45D4D0 : 0x501160;
	static std::uintptr_t dwHasPerk = bIsSteamVersion ? 0x42BD20 : 0x530C10;
	static std::uintptr_t dwGetSurfacePenetrationDepth = bIsSteamVersion ? 0x572480 : 0x622CE0;
	static std::uintptr_t dwAdvanceTrace = bIsSteamVersion ? 0x5B0F60 : 0x703090;
	static std::uintptr_t dwPenetrationCheck = bIsSteamVersion ? 0x7E0370 : 0x7E0C00;
	static std::uintptr_t dwLocationalTrace = bIsSteamVersion ? 0x6C6C00 : 0x50B870;
	static std::uintptr_t dwAddReliableCommand = bIsSteamVersion ? 0x5E58E0 : 0x6A1C40;
	static std::uintptr_t dwCbufAddText = bIsSteamVersion ? 0x5BDF70 : 0x5C6F10;
	static std::uintptr_t dwGetSpreadForWeapon = bIsSteamVersion ? 0x402A80 : 0x5B0BE0;
	static std::uintptr_t dwHashSeed = bIsSteamVersion ? 0x543CE0 : 0x6AACC0;
	static std::uintptr_t dwRandomFloat = bIsSteamVersion ? 0x6A3490 : 0x4947E0;
	static std::uintptr_t dwSeedRandom = bIsSteamVersion ? 0x4F38A0 : 0x556180;
	static std::uintptr_t dwAngleNormalize180 = bIsSteamVersion ? 0x4DEF60 : 0x5FFDA0;
	static std::uintptr_t dwSetFovSensitivityScale = bIsSteamVersion ? 0x40BB00 : 0x437730;
	static std::uintptr_t dwGetViewmodelWeaponIndex = bIsSteamVersion ? 0x625770 : 0x54A0D0;
	static std::uintptr_t dwWeaponHasPerk = bIsSteamVersion ? 0x449D90 : 0x47ED20;
	static std::uintptr_t dwUsingSniperScope = bIsSteamVersion ? 0x646700 : 0x6C5960;
	static std::uintptr_t dwSetUserCmdAimValues = bIsSteamVersion ? 0x6E7F80 : 0x67ACC0;
	static std::uintptr_t dwClampUserCmdAimValues = bIsSteamVersion ? 0x942D10 : 0x942B50;
	static std::uintptr_t dwGetLastWeaponForAlt = bIsSteamVersion ? 0x4760B0 : 0x480290;
	static std::uintptr_t dwSetUserCmdWeapons = bIsSteamVersion ? 0x552A60 : 0x518D70;
	static std::uintptr_t dwSetExtraButtons = bIsSteamVersion ? 0x4840B0 : 0x6994D0;
	static std::uintptr_t dwGetWeaponName = bIsSteamVersion ? 0x458190 : 0x9502B0;
	static std::uintptr_t dwGetPartyData = bIsSteamVersion ? 0x548CF0 : 0x615150;
	static std::uintptr_t dwGetSessionData = bIsSteamVersion ? 0x4823F0 : 0x534520;
	static std::uintptr_t dwIsUserRegistered = bIsSteamVersion ? 0x6181A0 : 0x44D670;
	static std::uintptr_t dwGetPlayerAddr = bIsSteamVersion ? 0x701320 : 0x641180;
	static std::uintptr_t dwGetPlayerXuid = bIsSteamVersion ? 0x45C710 : 0x667A00;
	static std::uintptr_t dwGetOurClientNum = bIsSteamVersion ? 0x5F8040 : 0x629320;
	static std::uintptr_t dwJoinSessionFromXuid = bIsSteamVersion ? 0x6EB1E0 : 0x43D980;
	static std::uintptr_t dwCycleWeapon = bIsSteamVersion ? 0x69BAE0 : 0x57D850;
	static std::uintptr_t dwFindVar = bIsSteamVersion ? 0x42D410 : 0x563A70;
	static std::uintptr_t dwFindHash = bIsSteamVersion ? 0xA5E1C0 : 0xA5E1D0;
	static std::uintptr_t dwHashMemory = bIsSteamVersion ? 0xA61B00 : 0xA61B10;
	static std::uintptr_t dwRegisterPrng = bIsSteamVersion ? 0xA61F10 : 0xA61F20;
	static std::uintptr_t dwFindPrng = bIsSteamVersion ? 0xA61EA0 : 0xA61EB0;
	static std::uintptr_t dwSignHash = bIsSteamVersion ? 0xA61C00 : 0xA61C10;

	static std::uintptr_t dwTigerHash = bIsSteamVersion ? 0xC81AF8 : 0xC273A4;
	static std::uintptr_t dwPrng = bIsSteamVersion ? 0xCC8030 : 0xCC8090;
	static std::uintptr_t dwSprng = bIsSteamVersion ? 0xC52214 : 0xBD590C;
	static std::uintptr_t dwKey = bIsSteamVersion ? 0x301EB88 : 0x2FFDB88;

	static std::uintptr_t dwRankXP = bIsSteamVersion ? 0x2FB176C : 0x2F9076C;
	static std::uintptr_t dwPLevel = bIsSteamVersion ? 0x2FB1754 : 0x2F90754;

	static std::uintptr_t dwStatsBase = bIsSteamVersion ? 0x2F00000 : 0x2EDF000;
	static std::uintptr_t dwMemoryBase = bIsSteamVersion ? 0x24A0000 : 0x247F000;

	static std::uintptr_t dwSwapChain = bIsSteamVersion ? 0x3606F94 : 0x35E5F94;
	static std::uintptr_t dwDevice = bIsSteamVersion ? 0x35CF484 : 0x35AE484;
	static std::uintptr_t dwDeviceContext = bIsSteamVersion ? 0x35CF488 : 0x35AE488;

	static std::uintptr_t dwMouseInput = 0x2B69969;

	static std::uintptr_t dwTacSSCount = 0x28A1FB4;
	static std::uintptr_t dwTacSSCheck = 0x2B68838;
	static std::uintptr_t dwTacSSPatch = 0x2B68824;
	static std::uintptr_t dwTacSSHandle = bIsSteamVersion ? 0x3A458F4 : 0x3A248F4;
	static std::uintptr_t dwXnAddr = bIsSteamVersion ? 0x34389B0 : 0x34179B0;

	static std::vector<std::pair<std::uintptr_t, std::uintptr_t>> vWeaponized =
	{
		std::make_pair(0x164FAB8, 0x14E),
		std::make_pair(0x164FB28, 0xBC),
		std::make_pair(0x164FB98, 0xBC),
		std::make_pair(0x164FC08, 0xBC),
		std::make_pair(0x164FC78, 0xBC),
		std::make_pair(0x164FCE8, 0xCC),
		std::make_pair(0x164FD58, 0xCC),
		std::make_pair(0x164FDC8, 0xCC)
	};
	/*
	//=====================================================================================
	*/
#define CG ((sCG*)Dereference(dwCG))
#define ClientActive ((sClientActive*)Dereference(dwClientActive))
#define WeaponNames ((sWeaponName*)Dereference(dwWeaponNames))
#define ViewAngles ((sViewAngles*)(Dereference(dwCG)+dwViewAngles))
#define RecoilAngles ((sRecoilAngles*)(Dereference(dwCG)+dwRecoilAngles))
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
	FORCEINLINE int Sys_Milliseconds()
	{
		return VariadicCall<int>(dwSysMilliseconds);
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
	FORCEINLINE void PopOverlayForSteamID(std::uint64_t steamid)
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
	FORCEINLINE int Int64ToString(std::uint64_t int64, LPSTR buffer)
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
	FORCEINLINE bool EntityIsTeammate(sCEntity* entity)
	{
		return VariadicCall<bool>(dwEntityIsTeammate, 0, entity);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE void GetTeamColor(eTeam team, ImVec4* color)
	{
		return VariadicCall<void>(dwGetTeamColor, team, color);
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
	FORCEINLINE int GetAmmoNotInClip(int weapon)
	{
		return VariadicCall<int>(dwGetAmmoNotInClip, &CG->PlayerState, weapon);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE int GetClipSize(int weapon)
	{
		return VariadicCall<int>(dwGetClipSize, weapon);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE sPlayerHeldWeapon* GetHeldWeapon(int weapon)
	{
		return VariadicCall<sPlayerHeldWeapon*>(dwGetHeldWeapon, &CG->PlayerState, weapon);
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
	FORCEINLINE WORD GetTraceHitType(sTrace* trace)
	{
		return VariadicCall<WORD>(dwGetTraceHitType, trace);
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
	FORCEINLINE bool PenetrationCheck(sBulletFireParams* fireparams)
	{
		return VariadicCall<bool>(dwPenetrationCheck, 0, fireparams);
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
	FORCEINLINE float AngleNormalize180(float angle)
	{
		return VariadicCall<float>(dwAngleNormalize180, angle);
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
	FORCEINLINE void ClampUserCmdAimValues(sUserCmd* usercmd)
	{
		return VariadicCall<void>(dwClampUserCmdAimValues, &CG->PlayerState, usercmd);
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
	FORCEINLINE LPVOID GetSessionData()
	{
		return VariadicCall<LPVOID>(dwGetSessionData);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE LPVOID GetPartyData()
	{
		return VariadicCall<LPVOID>(dwGetPartyData);
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
	FORCEINLINE std::uint64_t GetPlayerXuid(LPVOID session, int clientnum)
	{
		return VariadicCall<std::uint64_t>(dwGetPlayerXuid, session, clientnum);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE int GetOurClientNum(LPVOID session)
	{
		return VariadicCall<int>(dwGetOurClientNum, 0, session);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE bool JoinSessionFromXuid(std::uint64_t xuid)
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
	/*
	//=====================================================================================
	*/
	struct sTimer
	{
	private:

		int iTick = 0, iWait = 0;

	public:

		bool Ready()
		{
			return ((Sys_Milliseconds() - iTick) >= iWait);
		};

		void Wait(int timeout)
		{
			iTick = Sys_Milliseconds();
			iWait = timeout;
		};
	};
}

//=====================================================================================