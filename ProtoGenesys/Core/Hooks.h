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

		bool bUserHooked, bFriendsHooked;
		char szXuidOverride[0x11];
		QWORD dwXuidOverride;

		std::vector<std::pair<QWORD, std::string>> vFriends;

		DWORD dwConnectPaths, dwMouseAccel, dwDrawBigFPS, dwMaxClients, dwNoDelta,
			dwSysValue, dwShader, dwAddress;

		DWORD_PTR dwSteamUserFunc, dwSteamUserVTable, dwSteamFriendsFunc, dwSteamFriendsVTable;

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
		void CalcEntityLerpPositions(int localnum, sEntity* entity);
		void OffsetThirdPersonView(int localnum1, int localnum2);

		int GetPlayerStatus(int localnum, DWORD xuid1, DWORD xuid2);
		sSteamID GetSteamID(sSteamID steamid);
		bool GetFriendGamePlayed(sSteamID steamid, int unk1, int unk2, sFriendGameInfo* friendgameinfo);
		ePersonaState GetFriendPersonaState(DWORD** _this, void* edx, sSteamID steamid);
		LPCSTR GetFriendPersonaName(DWORD** _this, void* edx, sSteamID steamid);
		int GetFriendCount(DWORD** _this, void* edx, eFriendFlags friendflags);
		sSteamID GetFriendByIndex(DWORD** _this, void* edx, QWORD* steamid, int _friend, eFriendFlags friendflags);

		void RefreshFriends();

		cWin32Thunk<tVectoredExceptionHandler, cHooks> _thunkVectoredExceptionHandler;
	} extern _hooks;
}

//=====================================================================================