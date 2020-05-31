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

		typedef enum
		{
			VEH_INDEX_CONNECTPATHS = 2104,
			VEH_INDEX_MOUSEACCEL = 22152,
			VEH_INDEX_DRAWBIGFPS = 33553,
			VEH_INDEX_MAXCLIENTS = 33558,
			VEH_INDEX_NODELTA = 33833
		} eVehIndex;

		QWORD qwXuidOverride;

		int iFriendCount;
		std::vector<std::pair<QWORD, std::string>> vFriends;

		DWORD dwConnectPaths, dwMouseAccel, dwDrawBigFPS, dwMaxClients, dwNoDelta, dwSysValue, dwShader, dwAddress;
		DWORD_PTR dwSteamUser, dwSteamFriends;

		ISteamUser* _steamUser;
		ISteamFriends* _steamFriends;

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
		void KillSpam(DWORD attacker, DWORD victim);
		void BulletHitEvent(int localnum, int sourcenum, int targetnum, int weapon, ImVec3* start, ImVec3* position, ImVec3* normal, ImVec3* alphanormal, int surface, int eventnum, int eventparm, int contents, char bone);
		void TransitionPlayerState(int localnum, sPlayerState* playerstate, LPVOID transplayerstate);
		void CalcEntityLerpPositions(int localnum, sCEntity* entity);
		void GetWorldTagMatrix(LPVOID pose, LPVOID dobj, WORD tag, ImVec3 matrix[], ImVec3* origin);
		void GetAddr(bool renew);
		
		int GameTypeSettings(int settings);
		int GetPlayerStatus(int localnum, QWORD xuid);
		bool SteamIDIsValid(CSteamID* steamid);

		CSteamID* GetSteamID(CSteamID* steamid);
		LPCSTR GetPersonaName(LPCSTR name);
		int GetFriendCount(LPVOID ecx, LPVOID edx, EFriendFlags friendflags);
		void GetFriendByIndex(LPVOID ecx, LPVOID edx, CSteamID* steamid, int index, EFriendFlags friendflags);
		EPersonaState GetFriendPersonaState(LPVOID ecx, LPVOID edx, CSteamID steamid);
		LPCSTR GetFriendPersonaName(LPVOID ecx, LPVOID edx, CSteamID steamid);
		bool GetFriendGamePlayed(CSteamID steamid, int unk1, int unk2, FriendGameInfo_t* friendgameinfo);

		int Atoi1(int result);
		int Atoi2(int result);

		std::string Randomize(std::string name);
		void RefreshFriends();
		void SetThirdPerson();

		StdThunk<tVectoredExceptionHandler, cHooks> _thunkVectoredExceptionHandler;
	} extern _hooks;
}

//=====================================================================================