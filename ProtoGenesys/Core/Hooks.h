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

		std::vector<std::pair<int64_t, std::string>> vFriends;

		DWORD dwSysValue, dwConnectPaths, dwMouseAccel, dwDrawBigFPS, dwMaxClients, dwNoDelta, dwAddress;
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
		void ProcessText(LPSTR key, LPSTR value, SIZE_T length);

		int GetPlayerStatus(int localnum, DWORD xuid1, DWORD xuid2);
		int GetSteamID(DWORD steamID);
		bool GetFriendGamePlayed(CSteamID steamIDFriend, int unk1, int unk2, FriendGameInfo_t* gameInfo);
		int GetFriendPersonaState(DWORD** _this, void* edx, CSteamID steamIDFriend);
		LPCSTR GetFriendPersonaName(DWORD** _this, void* edx, CSteamID steamIDFriend);
		int GetFriendCount(DWORD** _this, void* edx, int iFriendFlags);
		void GetFriendByIndex(DWORD** _this, void* edx, int64_t* pSteamID, int iFriend, int iFriendFlags);

		void RefreshFriends();

		cWin32Thunk<tVectoredExceptionHandler, cHooks> _thunkVectoredExceptionHandler;
	} extern _hooks;
}

//=====================================================================================