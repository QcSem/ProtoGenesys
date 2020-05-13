//=====================================================================================

#include "StdAfx.hpp"

using namespace ProtoGenesys;

//=====================================================================================

#define FASTCALL __fastcall
#define USERCALL __cdecl

#define DLLEXPORT comment(linker, "/EXPORT:" __FUNCTION__ "=" __FUNCDNAME__)

#define AttachHook(original, hook) (DetourTransactionBegin(), DetourUpdateThread(GetCurrentThread()), DetourAttach((LPVOID*)&original, (LPVOID)hook), DetourTransactionCommit())
#define DetachHook(original, hook) (DetourTransactionBegin(), DetourUpdateThread(GetCurrentThread()), DetourDetach((LPVOID*)&original, (LPVOID)hook), DetourTransactionCommit())

//=====================================================================================

HRESULT WINAPI hPresent(_In_ IDXGISwapChain* pSwapChain, _In_ UINT SyncInterval, _In_ UINT Flags);
typedef HRESULT(WINAPI* tPresent)(_In_ IDXGISwapChain* pSwapChain, _In_ UINT SyncInterval, _In_ UINT Flags);
tPresent oPresent;

void USERCALL hBulletHitEvent(int localnum, int sourcenum, int targetnum, int weapon, Vector3 start, Vector3 position, Vector3 normal, Vector3 alphanormal, int surface, int _event, int param, int contents, char bone);
typedef void(USERCALL* tBulletHitEvent)(int localnum, int sourcenum, int targetnum, int weapon, Vector3 start, Vector3 position, Vector3 normal, Vector3 alphanormal, int surface, int _event, int param, int contents, char bone);
tBulletHitEvent oBulletHitEvent = (tBulletHitEvent)dwBulletHitEvent;

void USERCALL hTransitionPlayerState(int localnum, sPlayerState* playerstate, LPVOID transplayerstate);
typedef void(USERCALL* tTransitionPlayerState)(int localnum, sPlayerState* playerstate, LPVOID transplayerstate);
tTransitionPlayerState oTransitionPlayerState = (tTransitionPlayerState)dwTransitionPlayerState;

void USERCALL hCalcEntityLerpPositions(int localnum, sCEntity* entity);
typedef void(USERCALL* tCalcEntityLerpPositions)(int localnum, sCEntity* entity);
tCalcEntityLerpPositions oCalcEntityLerpPositions = (tCalcEntityLerpPositions)dwCalcEntityLerpPositions;

int USERCALL hGetWorldTagMatrix(LPVOID pose, LPVOID dobj, WORD tag, Vector3 matrix[], Vector3 origin);
typedef int(USERCALL* tGetWorldTagMatrix)(LPVOID pose, LPVOID dobj, WORD tag, Vector3 matrix[], Vector3 origin);
tGetWorldTagMatrix oGetWorldTagMatrix = (tGetWorldTagMatrix)dwGetWorldTagMatrix;

LPVOID USERCALL hGetAddr(bool renew);
typedef LPVOID(USERCALL* tGetAddr)(bool renew);
tGetAddr oGetAddr = (tGetAddr)dwGetAddr;

int USERCALL hGameTypeSettings(int setting);
typedef int(USERCALL* tGameTypeSettings)(int setting);
tGameTypeSettings oGameTypeSettings = (tGameTypeSettings)dwGameTypeSettings;

int USERCALL hGetPlayerStatus(int localnum, DWORD xuid1, DWORD xuid2);
typedef int(USERCALL* tGetPlayerStatus)(int localnum, DWORD xuid1, DWORD xuid2);
tGetPlayerStatus oGetPlayerStatus = (tGetPlayerStatus)dwGetPlayerStatus;

bool FASTCALL hSteamIDIsValid(DWORD** _this);
typedef bool(FASTCALL* tSteamIDIsValid)(DWORD** _this);
tSteamIDIsValid oSteamIDIsValid = (tSteamIDIsValid)dwSteamIDIsValid;

sSteamID FASTCALL hGetSteamID(DWORD** _this, void* edx, int a2);
typedef sSteamID(FASTCALL* tGetSteamID)(DWORD** _this, void* edx, int a2);
tGetSteamID oGetSteamID;

bool FASTCALL hGetFriendGamePlayed(sSteamID steamid, int unk1, int unk2, sFriendGameInfo* friendgameinfo);
typedef bool(FASTCALL* tGetFriendGamePlayed)(sSteamID steamid, int unk1, int unk2, sFriendGameInfo* friendgameinfo);
tGetFriendGamePlayed oGetFriendGamePlayed;

ePersonaState FASTCALL hGetFriendPersonaState(DWORD** _this, void* edx, sSteamID steamid);
typedef ePersonaState(FASTCALL* tGetFriendPersonaState)(DWORD** _this, void* edx, sSteamID steamid);
tGetFriendPersonaState oGetFriendPersonaState;

LPCSTR FASTCALL hGetFriendPersonaName(DWORD** _this, void* edx, sSteamID steamid);
typedef LPCSTR(FASTCALL* tGetFriendPersonaName)(DWORD** _this, void* edx, sSteamID steamid);
tGetFriendPersonaName oGetFriendPersonaName;

int FASTCALL hGetFriendCount(DWORD** _this, void* edx, eFriendFlags friendflags);
typedef int(FASTCALL* tGetFriendCount)(DWORD** _this, void* edx, eFriendFlags friendflags);
tGetFriendCount oGetFriendCount;

sSteamID FASTCALL hGetFriendByIndex(DWORD** _this, void* edx, QWORD* steamid, int _friend, eFriendFlags friendflags);
typedef sSteamID(FASTCALL* tGetFriendByIndex)(DWORD** _this, void* edx, QWORD* steamid, int _friend, eFriendFlags friendflags);
tGetFriendByIndex oGetFriendByIndex;

//=====================================================================================

FurtiveHook fhTransitionPlayerStateCall{ x86Instruction::CALL, (LPVOID)dwTransitionPlayerStateCall, &hTransitionPlayerState };
FurtiveHook fhGetWorldTagMatrixCall{ x86Instruction::CALL, (LPVOID)dwGetWorldTagMatrixCall, &hGetWorldTagMatrix };
FurtiveHook fhGameTypeSettingsCall{ x86Instruction::CALL, (LPVOID)dwGameTypeSettingsCall, &hGameTypeSettings };

//=====================================================================================

HRESULT WINAPI hPresent(_In_ IDXGISwapChain* swapchain, _In_ UINT syncinterval, _In_ UINT flags)
{
	_mainGui.Present(swapchain, syncinterval, flags);

	return oPresent(swapchain, syncinterval, flags);
}

//=====================================================================================

void USERCALL hBulletHitEvent(int localnum, int sourcenum, int targetnum, int weapon, Vector3 start, Vector3 position, Vector3 normal, Vector3 alphanormal, int surface, int _event, int param, int contents, char bone)
{
	oBulletHitEvent(localnum, sourcenum, targetnum, weapon, start, position, normal, alphanormal, surface, _event, param, contents, bone);

	_hooks.BulletHitEvent(localnum, sourcenum, targetnum, weapon, start, position, normal, alphanormal, surface, _event, param, contents, bone);
}

//=====================================================================================

void USERCALL hTransitionPlayerState(int localnum, sPlayerState* playerstate, LPVOID transplayerstate)
{
	_hooks.TransitionPlayerState(localnum, playerstate, transplayerstate);

	oTransitionPlayerState(localnum, playerstate, transplayerstate);
}

//=====================================================================================

void USERCALL hCalcEntityLerpPositions(int localnum, sCEntity* entity)
{
	oCalcEntityLerpPositions(localnum, entity);

	return _hooks.CalcEntityLerpPositions(localnum, entity);
}

//=====================================================================================

int USERCALL hGetWorldTagMatrix(LPVOID pose, LPVOID dobj, WORD tag, Vector3 matrix[], Vector3 origin)
{
	int iReturn = oGetWorldTagMatrix(pose, dobj, tag, matrix, origin);

	_hooks.GetWorldTagMatrix(pose, dobj, tag, matrix, origin);

	return iReturn;
}

//=====================================================================================

LPVOID USERCALL hGetAddr(bool renew)
{
	_hooks.GetAddr(renew);

	return oGetAddr(renew);
}

//=====================================================================================

int USERCALL hGameTypeSettings(int setting)
{
	return _hooks.GameTypeSettings(setting);
}

//=====================================================================================

int USERCALL hGetPlayerStatus(int localnum, DWORD xuid1, DWORD xuid2)
{
	return _hooks.GetPlayerStatus(localnum, xuid1, xuid2);
}

//=====================================================================================

bool FASTCALL hSteamIDIsValid(DWORD** _this)
{
	return _hooks.SteamIDIsValid(_this);
}

//=====================================================================================

sSteamID FASTCALL hGetSteamID(DWORD** _this, void* edx, int a2)
{
	return _hooks.GetSteamID(oGetSteamID(_this, edx, a2));
}

//=====================================================================================

bool FASTCALL hGetFriendGamePlayed(sSteamID steamid, int unk1, int unk2, sFriendGameInfo* friendgameinfo)
{
	return _hooks.GetFriendGamePlayed(steamid, unk1, unk2, friendgameinfo);
}

//=====================================================================================

ePersonaState FASTCALL hGetFriendPersonaState(DWORD** _this, void* edx, sSteamID steamid)
{
	return _hooks.GetFriendPersonaState(_this, edx, steamid);
}

//=====================================================================================

LPCSTR FASTCALL hGetFriendPersonaName(DWORD** _this, void* edx, sSteamID steamid)
{
	LPCSTR szName = _hooks.GetFriendPersonaName(_this, edx, steamid);

	if (szName)
		return szName;

	return oGetFriendPersonaName(_this, edx, steamid);
}

//=====================================================================================

int FASTCALL hGetFriendCount(DWORD** _this, void* edx, eFriendFlags friendflags)
{
	_hooks.iFriendCount = oGetFriendCount(_this, edx, friendflags);

	return _hooks.GetFriendCount(_this, edx, friendflags) + _hooks.iFriendCount;
}

//=====================================================================================

sSteamID FASTCALL hGetFriendByIndex(DWORD** _this, void* edx, QWORD* steamid, int _friend, eFriendFlags friendflags)
{
	sSteamID SteamID;

	if (_friend >= 0 && _friend < _hooks.iFriendCount)
		SteamID = oGetFriendByIndex(_this, edx, steamid, _friend, friendflags);

	if (_friend >= _hooks.iFriendCount && _friend < _hooks.iFriendCount + (int)_hooks.vFriends.size())
		SteamID = _hooks.GetFriendByIndex(_this, edx, steamid, _friend, friendflags);

	return SteamID;
}

//=====================================================================================

void Initialize(HINSTANCE hinstDLL)
{
	_mainGui.hInstDll = hinstDLL;
	_hooks.PatchAntiCheat();

	_hooks.pUnhandledExceptionFilter = SetUnhandledExceptionFilter(NULL);
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

	oPresent = (tPresent)SwapVMT((bIsSteamVersion && bGameOverlayRenderer) ? (DWORD_PTR)&dwPresent : dwPresent, (DWORD_PTR)&hPresent, (bIsSteamVersion && bGameOverlayRenderer) ? 0 : 8);

	AttachHook(oBulletHitEvent, hBulletHitEvent);
	AttachHook(oCalcEntityLerpPositions, hCalcEntityLerpPositions);
	AttachHook(oGetAddr, hGetAddr);
	AttachHook(oGetPlayerStatus, hGetPlayerStatus);
	AttachHook(oSteamIDIsValid, hSteamIDIsValid);

	fhTransitionPlayerStateCall.SetHook();
	fhGetWorldTagMatrixCall.SetHook();
	fhGameTypeSettingsCall.SetHook();
}

//=====================================================================================

void Deallocate()
{
	*(DWORD_PTR*)dwConnectPathsDvar = _hooks.dwConnectPaths;
	*(DWORD_PTR*)dwMouseAccelerationDvar = _hooks.dwMouseAccel;
	*(DWORD_PTR*)dwDrawBigFPSDvar = _hooks.dwDrawBigFPS;
	*(DWORD_PTR*)dwMaxClientsDvar = _hooks.dwMaxClients;
	*(DWORD_PTR*)dwNoDeltaDvar = _hooks.dwNoDelta;

	RemoveVectoredExceptionHandler(_hooks.pVectoredExceptionHandler);
	SetUnhandledExceptionFilter(_hooks.pUnhandledExceptionFilter);

	SwapVMT((bIsSteamVersion && bGameOverlayRenderer) ? (DWORD_PTR)&dwPresent : dwPresent, (DWORD_PTR)oPresent, (bIsSteamVersion && bGameOverlayRenderer) ? 0 : 8);

	DetachHook(oBulletHitEvent, hBulletHitEvent);
	DetachHook(oCalcEntityLerpPositions, hCalcEntityLerpPositions);
	DetachHook(oGetAddr, hGetAddr);
	DetachHook(oGetPlayerStatus, hGetPlayerStatus);
	DetachHook(oSteamIDIsValid, hSteamIDIsValid);

	fhTransitionPlayerStateCall.UnHook();
	fhGetWorldTagMatrixCall.UnHook();
	fhGameTypeSettingsCall.UnHook();

	if (oGetSteamID)
		SwapVMT(_hooks.dwSteamUserVTable, (DWORD_PTR)oGetSteamID, 2);

	if (oGetFriendCount)
		SwapVMT(_hooks.dwSteamFriendsVTable, (DWORD_PTR)oGetFriendCount, 3);

	if (oGetFriendByIndex)
		SwapVMT(_hooks.dwSteamFriendsVTable, (DWORD_PTR)oGetFriendByIndex, 4);

	if (oGetFriendPersonaState)
		SwapVMT(_hooks.dwSteamFriendsVTable, (DWORD_PTR)oGetFriendPersonaState, 6);

	if (oGetFriendPersonaName)
		SwapVMT(_hooks.dwSteamFriendsVTable, (DWORD_PTR)oGetFriendPersonaName, 7);

	if (oGetFriendGamePlayed)
		SwapVMT(_hooks.dwSteamFriendsVTable, (DWORD_PTR)oGetFriendGamePlayed, 8);

	_mainGui.pDevice->Release();
	_mainGui.pDeviceContext->Release();

	ImGui_ImplWin32_Shutdown();
	ImGui_ImplDX11_Shutdown();
	ImGui::DestroyContext();

	SetWindowLongPtr(_mainGui.hWindow, GWLP_WNDPROC, (LONG_PTR)_mainGui.oWindowProcess);
}

//=====================================================================================

void SteamFriends()
{
	_hooks.RefreshFriends();

	if (!hSteamAPI)
		return;

	_hooks.dwSteamFriendsFunc = (DWORD_PTR)GetProcAddress(hSteamAPI, "SteamFriends");

	if (!_hooks.dwSteamFriendsFunc)
		return;

	_hooks.dwSteamFriendsVTable = VariadicCall<DWORD_PTR>(_hooks.dwSteamFriendsFunc);

	if (!_hooks.dwSteamFriendsVTable)
		return;

	oGetFriendCount = (tGetFriendCount)SwapVMT(_hooks.dwSteamFriendsVTable, (DWORD_PTR)&hGetFriendCount, 3);
	oGetFriendByIndex = (tGetFriendByIndex)SwapVMT(_hooks.dwSteamFriendsVTable, (DWORD_PTR)&hGetFriendByIndex, 4);
	oGetFriendPersonaState = (tGetFriendPersonaState)SwapVMT(_hooks.dwSteamFriendsVTable, (DWORD_PTR)&hGetFriendPersonaState, 6);
	oGetFriendPersonaName = (tGetFriendPersonaName)SwapVMT(_hooks.dwSteamFriendsVTable, (DWORD_PTR)&hGetFriendPersonaName, 7);
	oGetFriendGamePlayed = (tGetFriendGamePlayed)SwapVMT(_hooks.dwSteamFriendsVTable, (DWORD_PTR)&hGetFriendGamePlayed, 8);
}

//=====================================================================================

void WINAPI SteamUser(LPWSTR xuid)
{
#pragma DLLEXPORT

	_hooks.dwXuidOverride = wcstoll(xuid, NULL, 10);

	if (!hSteamAPI)
		return;

	_hooks.dwSteamUserFunc = (DWORD_PTR)GetProcAddress(hSteamAPI, "SteamUser");

	if (!_hooks.dwSteamUserFunc)
		return;

	_hooks.dwSteamUserVTable = VariadicCall<DWORD_PTR>(_hooks.dwSteamUserFunc);

	if (!_hooks.dwSteamUserVTable)
		return;

	oGetSteamID = (tGetSteamID)SwapVMT(_hooks.dwSteamUserVTable, (DWORD_PTR)&hGetSteamID, 2);

	SteamFriends();
}

//=====================================================================================

BOOL APIENTRY DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved)
{
	DisableThreadLibraryCalls(hinstDLL);

	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		Initialize(hinstDLL);
		return TRUE;

	case DLL_PROCESS_DETACH:
		Deallocate();
		return TRUE;

	default:
		return FALSE;
	}
}

//=====================================================================================