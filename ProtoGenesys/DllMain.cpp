//=====================================================================================

#include "StdAfx.hpp"

using namespace ProtoGenesys;

//=====================================================================================

#define FASTCALL __fastcall
#define USERCALL __cdecl

#define DLLEXPORT comment(linker, "/EXPORT:" __FUNCTION__ "=" __FUNCDNAME__)

#define Hook(original, hook) (DetourTransactionBegin(), DetourUpdateThread(GetCurrentThread()), DetourAttach((LPVOID*)&original, (LPVOID)hook), DetourTransactionCommit())
#define UnHook(original, hook) (DetourTransactionBegin(), DetourUpdateThread(GetCurrentThread()), DetourDetach((LPVOID*)&original, (LPVOID)hook), DetourTransactionCommit())

//=====================================================================================

VOID WINAPI SteamFriends();
VOID WINAPI SteamUser(LPWSTR xuid);

//=====================================================================================

HRESULT WINAPI hPresent(_In_ IDXGISwapChain* pSwapChain, _In_ UINT SyncInterval, _In_ UINT Flags);
typedef HRESULT(WINAPI* tPresent)(_In_ IDXGISwapChain* pSwapChain, _In_ UINT SyncInterval, _In_ UINT Flags);
tPresent oPresent = *(tPresent*)dwPresent;

INT USERCALL hProcessText(LPSTR key, LPSTR value, SIZE_T length);
typedef INT(USERCALL* tProcessText)(LPSTR key, LPSTR value, SIZE_T length);
tProcessText oProcessText = (tProcessText)dwProcessText;

int USERCALL hGetPlayerStatus(int localnum, DWORD xuid1, DWORD xuid2);
typedef int(USERCALL* tGetPlayerStatus)(int localnum, DWORD xuid1, DWORD xuid2);
tGetPlayerStatus oGetPlayerStatus = (tGetPlayerStatus)dwGetPlayerStatus;

int FASTCALL hGetSteamID(DWORD** _this, void* edx, int a2);
typedef int (FASTCALL* tGetSteamID)(DWORD** _this, void* edx, int a2);
tGetSteamID oGetSteamID;

bool FASTCALL hGetFriendGamePlayed(CSteamID steamIDFriend, int unk1, int unk2, FriendGameInfo_t* gameInfo);
typedef bool(FASTCALL* tGetFriendGamePlayed)(CSteamID steamIDFriend, int unk1, int unk2, FriendGameInfo_t* gameInfo);
tGetFriendGamePlayed oGetFriendGamePlayed;

int FASTCALL hGetFriendPersonaState(DWORD** _this, void* edx, CSteamID steamIDFriend);
typedef int(FASTCALL* tGetFriendPersonaState)(DWORD** _this, void* edx, CSteamID steamIDFriend);
tGetFriendPersonaState oGetFriendPersonaState;

LPCSTR FASTCALL hGetFriendPersonaName(DWORD** _this, void* edx, CSteamID steamIDFriend);
typedef LPCSTR(FASTCALL* tGetFriendPersonaName)(DWORD** _this, void* edx, CSteamID steamIDFriend);
tGetFriendPersonaName oGetFriendPersonaName;

int FASTCALL hGetFriendCount(DWORD** _this, void* edx, int iFriendFlags);
typedef int(FASTCALL* tGetFriendCount)(DWORD** _this, void* edx, int iFriendFlags);
tGetFriendCount oGetFriendCount;

void FASTCALL hGetFriendByIndex(DWORD** _this, void* edx, int64_t* pSteamID, int iFriend, int iFriendFlags);
typedef void(FASTCALL* tGetFriendByIndex)(DWORD** _this, void* edx, int64_t* pSteamID, int iFriend, int iFriendFlags);
tGetFriendByIndex oGetFriendByIndex;

//=====================================================================================

HRESULT WINAPI hPresent(_In_ IDXGISwapChain* swapchain, _In_ UINT syncinterval, _In_ UINT flags)
{
	_mainGui.Present(swapchain, syncinterval, flags);

	return oPresent(swapchain, syncinterval, flags);
}

//=====================================================================================

INT USERCALL hProcessText(LPSTR key, LPSTR value, SIZE_T length)
{
	_hooks.ProcessText(key, value, length);

	return oProcessText(key, value, length);
}

//=====================================================================================

int USERCALL hGetPlayerStatus(int localnum, DWORD xuid1, DWORD xuid2)
{
	oGetPlayerStatus(localnum, xuid1, xuid2);

	return _hooks.GetPlayerStatus(localnum, xuid1, xuid2);
}

//=====================================================================================

int FASTCALL hGetSteamID(DWORD** _this, void* edx, int a2)
{
	return _hooks.GetSteamID(oGetSteamID(_this, edx, a2));
}

//=====================================================================================

bool FASTCALL hGetFriendGamePlayed(CSteamID steamIDFriend, int unk1, int unk2, FriendGameInfo_t* gameInfo)
{
	return _hooks.GetFriendGamePlayed(steamIDFriend, unk1, unk2, gameInfo);
}

//=====================================================================================

int FASTCALL hGetFriendPersonaState(DWORD** _this, void* edx, CSteamID steamIDFriend)
{
	return _hooks.GetFriendPersonaState(_this, edx, steamIDFriend);
}

//=====================================================================================

LPCSTR FASTCALL hGetFriendPersonaName(DWORD** _this, void* edx, CSteamID steamIDFriend)
{
	return _hooks.GetFriendPersonaName(_this, edx, steamIDFriend);
}

//=====================================================================================

int FASTCALL hGetFriendCount(DWORD** _this, void* edx, int iFriendFlags)
{
	return _hooks.GetFriendCount(_this, edx, iFriendFlags);
}

//=====================================================================================

void FASTCALL hGetFriendByIndex(DWORD** _this, void* edx, int64_t* pSteamID, int iFriend, int iFriendFlags)
{
	return _hooks.GetFriendByIndex(_this, edx, pSteamID, iFriend, iFriendFlags);
}

//=====================================================================================

VOID Initialize()
{
	_hooks.RefreshFriends();
	_hooks.PatchAntiCheat();

	SetUnhandledExceptionFilter(NULL);
	_hooks.pVectoredExceptionHandler = AddVectoredExceptionHandler(TRUE, _hooks._thunkVectoredExceptionHandler.GetThunk());

	_hooks.dwConnectPaths = *(DWORD_PTR*)dwConnectPathsDvar;
	*(DWORD_PTR*)dwConnectPathsDvar = cHooks::VEH_INDEX_CONNECTPATHS;

	_hooks.dwMouseAccel = *(DWORD_PTR*)dwMouseAccelerationDvar;
	*(DWORD_PTR*)dwMouseAccelerationDvar = cHooks::VEH_INDEX_MOUSEACCEL;

	_hooks.dwDrawBigFPS = *(DWORD_PTR*)dwDrawBigFPSDvar;
	*(DWORD_PTR*)dwDrawBigFPSDvar = cHooks::VEH_INDEX_DRAWBIGFPS;

	_hooks.dwMaxClients = *(DWORD_PTR*)dwMaxClientsDvar;
	*(DWORD_PTR*)dwMaxClientsDvar = cHooks::VEH_INDEX_MAXCLIENTS;

	_hooks.dwNoDelta = *(DWORD_PTR*)dwNoDeltaDvar;
	*(DWORD_PTR*)dwNoDeltaDvar = cHooks::VEH_INDEX_NODELTA;

	Hook(oPresent, hPresent);
	Hook(oProcessText, hProcessText);
	Hook(oGetPlayerStatus, hGetPlayerStatus);

	SteamFriends();
}

//=====================================================================================

VOID Deallocate()
{
	*(DWORD_PTR*)dwConnectPathsDvar = _hooks.dwConnectPaths;
	*(DWORD_PTR*)dwMouseAccelerationDvar = _hooks.dwMouseAccel;
	*(DWORD_PTR*)dwDrawBigFPSDvar = _hooks.dwDrawBigFPS;
	*(DWORD_PTR*)dwMaxClientsDvar = _hooks.dwMaxClients;
	*(DWORD_PTR*)dwNoDeltaDvar = _hooks.dwNoDelta;

	RemoveVectoredExceptionHandler(_hooks.pVectoredExceptionHandler);

	UnHook(oPresent, hPresent);
	UnHook(oProcessText, hProcessText);
	UnHook(oGetPlayerStatus, hGetPlayerStatus);

	if (_hooks.bUserHooked)
		UnHook(oGetSteamID, hGetSteamID);

	if (_hooks.bFriendsHooked)
	{
		UnHook(oGetFriendCount, hGetFriendCount);
		UnHook(oGetFriendByIndex, hGetFriendByIndex);
		UnHook(oGetFriendPersonaState, hGetFriendPersonaState);
		UnHook(oGetFriendPersonaName, hGetFriendPersonaName);
		UnHook(oGetFriendGamePlayed, hGetFriendGamePlayed);
	}

	_mainGui.pDevice->Release();
	_mainGui.pDeviceContext->Release();

	ImGui_ImplWin32_Shutdown();
	ImGui_ImplDX11_Shutdown();
	ImGui::DestroyContext();

	SetWindowLongPtr(_mainGui.hWindow, GWLP_WNDPROC, (LONG_PTR)_mainGui.oWindowProcess);
}

//=====================================================================================

VOID WINAPI SteamFriends()
{
	if (!hSteamAPI)
		return;

	DWORD_PTR dwSteamFriendsFunc = (DWORD_PTR)GetProcAddress(hSteamAPI, "SteamFriends");

	if (!dwSteamFriendsFunc)
		return;

	DWORD_PTR dwSteamFriendsVTable = VariadicCall<DWORD_PTR>(dwSteamFriendsFunc);

	if (!dwSteamFriendsVTable)
		return;

	oGetFriendCount = *(tGetSteamID*)(*(DWORD_PTR*)dwSteamFriendsVTable + sizeof(DWORD_PTR) * 0x3);
	oGetFriendByIndex = *(tGetFriendByIndex*)(*(DWORD_PTR*)dwSteamFriendsVTable + sizeof(DWORD_PTR) * 0x4);
	oGetFriendPersonaState = *(tGetFriendPersonaState*)(*(DWORD_PTR*)dwSteamFriendsVTable + sizeof(DWORD_PTR) * 0x6);
	oGetFriendPersonaName = *(tGetFriendPersonaName*)(*(DWORD_PTR*)dwSteamFriendsVTable + sizeof(DWORD_PTR) * 0x7);
	oGetFriendGamePlayed = *(tGetFriendGamePlayed*)(*(DWORD_PTR*)dwSteamFriendsVTable + sizeof(DWORD_PTR) * 0x8);

	if (!oGetFriendCount || !oGetFriendByIndex || !oGetFriendPersonaState || !oGetFriendPersonaName || !oGetFriendGamePlayed)
		return;

	Hook(oGetFriendCount, hGetFriendCount);
	Hook(oGetFriendByIndex, hGetFriendByIndex);
	Hook(oGetFriendPersonaState, hGetFriendPersonaState);
	Hook(oGetFriendPersonaName, hGetFriendPersonaName);
	Hook(oGetFriendGamePlayed, hGetFriendGamePlayed);

	_hooks.bFriendsHooked = true;
}

//=====================================================================================

VOID WINAPI SteamUser(LPWSTR xuid)
{ 
#pragma DLLEXPORT

	if (!hSteamAPI)
		return;

	DWORD_PTR dwSteamUserFunc = (DWORD_PTR)GetProcAddress(hSteamAPI, "SteamUser");

	if (!dwSteamUserFunc)
		return;

	DWORD_PTR dwSteamUserVTable = VariadicCall<DWORD_PTR>(dwSteamUserFunc);

	if (!dwSteamUserVTable)
		return;

	oGetSteamID = *(tGetSteamID*)(*(DWORD_PTR*)dwSteamUserVTable + sizeof(DWORD_PTR) * 0x2);

	if (!oGetSteamID)
		return;

	_hooks.dwXuidOverride = _wtoi64(xuid);

	Hook(oGetSteamID, hGetSteamID);

	_hooks.bUserHooked = true;
}

//=====================================================================================

BOOL APIENTRY DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		Initialize();
		return TRUE;

	case DLL_PROCESS_DETACH:
		Deallocate();
		return TRUE;

	default:
		return FALSE;
	}
}

//=====================================================================================