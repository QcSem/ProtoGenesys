//=====================================================================================

#pragma once

#include "Engine/Engine.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	class cHooks
	{
	public:

		cHooks() : _thunkVectoredExceptionHandler(&cHooks::VectoredExceptionHandler, this) {}

		enum eVehIndex
		{
			VEH_INDEX_CONNECTPATHS = 2104,
			VEH_INDEX_MOUSEACCEL = 22152,
			VEH_INDEX_DRAWBIGFPS = 33553,
			VEH_INDEX_MAXCLIENTS = 33558,
			VEH_INDEX_NODELTA = 33833
		};

		enum eFakeLagState
		{
			FAKELAG_ON_READY,
			FAKELAG_OFF_WAIT,
			FAKELAG_OFF_READY,
			FAKELAG_ON_WAIT
		} iFakeLagState = FAKELAG_ON_READY;

		sTimer RandomizeTimer, FakeLagOnTimer, FakeLagOffTimer;

		bool bXuidOverride;
		std::uint64_t qwXuidOverride;

		int iScreenShotCount, iFriendCount;
		std::vector<std::pair<std::uint64_t, std::string>> vFriends;

		DWORD dwConnectPaths, dwMouseAccel, dwDrawBigFPS, dwMaxClients, dwNoDelta, dwSysValue, dwShader, dwAddress;

		ISteamUser* _steamUser;
		typedef decltype(SteamUser)* tSteamUser;
		tSteamUser GetSteamUser;

		ISteamFriends* _steamFriends;
		typedef decltype(SteamFriends)* tSteamFriends;
		tSteamFriends GetSteamFriends;

		LPTOP_LEVEL_EXCEPTION_FILTER pUnhandledExceptionFilter;
		LPVOID pVectoredExceptionHandler;

		LONG VectoredExceptionHandler(_In_ LPEXCEPTION_POINTERS ExceptionInfo);
		typedef LONG(CALLBACK* tVectoredExceptionHandler)(_In_ LPEXCEPTION_POINTERS ExceptionInfo);

		BOOL PageGuardSVFK();
		BOOL PageGuardAddress(DWORD address);
		BOOL PatchAntiCheat();

		void DrawBigFPS();
		void PredictPlayerState();
		void WritePacket();
		void Obituary(DWORD attacker, DWORD victim);
		void BulletHitEvent(int localnum, int sourcenum, int targetnum, int weapon, ImVec3* start, ImVec3* position, ImVec3* normal, ImVec3* alphanormal, int surface, int eventnum, int eventparm, int contents, char bone);
		void TransitionPlayerState(int localnum, sPlayerState* playerstate, LPVOID transplayerstate);
		void CalcEntityLerpPositions(int localnum, sCEntity* entity);
		void SetupWeaponCamoRender(int localnum, ImVec3* origin, eWeaponDobjInfoSlot weaponslot, int camo, LPVOID weapon, int* texture);
		void GetWorldTagMatrix(LPVOID pose, LPVOID dobj, WORD tag, ImVec3 matrix[], ImVec3* origin);
		void GetAddr(bool renew);
		
		int GameTypeSettings(int settings);
		int GetPlayerStatus(int localnum, std::uint64_t xuid);
		bool IsValidSteamID(CSteamID* steamid);
		void CreateScreenShot(int unk1, int unk2, int unk3, int unk4);

		CSteamID* GetSteamID(CSteamID* steamid);
		LPCSTR GetPersonaName(LPCSTR name);
		int GetFriendCount(LPVOID ecx, LPVOID edx, EFriendFlags friendflags);
		void GetFriendByIndex(LPVOID ecx, LPVOID edx, CSteamID* steamid, int index, EFriendFlags friendflags);
		EPersonaState GetFriendPersonaState(LPVOID ecx, LPVOID edx, CSteamID steamid);
		LPCSTR GetFriendPersonaName(LPVOID ecx, LPVOID edx, CSteamID steamid);
		bool GetFriendGamePlayed(CSteamID steamid, int unk1, int unk2, FriendGameInfo_t* friendgameinfo);

		int Atoi1(int result);
		int Atoi2(int result);
		LPVOID Memcpy(LPVOID dst, LPCVOID src, SIZE_T size);

		std::uint64_t GetUserSteamIdAsXuid();
		LPSTR FilterPersonaName(LPSTR name, bool ascii);

		std::string Randomize(std::string name);
		void RefreshFriends();
		void SetThirdPerson();
		void FakeLag();

		StdThunk<tVectoredExceptionHandler, cHooks> _thunkVectoredExceptionHandler;
	} extern _hooks;
}

//=====================================================================================