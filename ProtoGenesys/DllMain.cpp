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

DWORD_PTR FASTCALL hGetSteamID(DWORD_PTR** a1, LPVOID a2, INT a3);
typedef DWORD_PTR(FASTCALL* tGetSteamID)(DWORD_PTR** a1, LPVOID a2, INT a3);
tGetSteamID oGetSteamID;

HRESULT WINAPI hPresent(_In_ IDXGISwapChain* pSwapChain, _In_ UINT SyncInterval, _In_ UINT Flags);
typedef HRESULT(WINAPI* tPresent)(_In_ IDXGISwapChain* pSwapChain, _In_ UINT SyncInterval, _In_ UINT Flags);
tPresent oPresent = *(tPresent*)dwPresent;

INT USERCALL hProcessText(LPSTR key, LPSTR value, SIZE_T length);
typedef INT(USERCALL* tProcessText)(LPSTR key, LPSTR value, SIZE_T length);
tProcessText oProcessText = (tProcessText)dwProcessText;

//=====================================================================================

DWORD_PTR FASTCALL hGetSteamID(DWORD_PTR** a1, LPVOID a2, INT a3)
{
	DWORD_PTR dwSteamID = oGetSteamID(a1, a2, a3);

	*(QWORD*)dwSteamID = _hooks.dwXuidOverride;

	return dwSteamID;
}

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

VOID Initialize()
{
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

	if (_hooks.bXuidOverride)
		UnHook(oGetSteamID, hGetSteamID);

	_mainGui.pDevice->Release();
	_mainGui.pDeviceContext->Release();

	ImGui_ImplWin32_Shutdown();
	ImGui_ImplDX11_Shutdown();
	ImGui::DestroyContext();

	SetWindowLongPtr(_mainGui.hWindow, GWLP_WNDPROC, (LONG_PTR)_mainGui.oWindowProcess);
}

//=====================================================================================

VOID WINAPI SteamID(LPWSTR xuid)
{ 
#pragma DLLEXPORT

	_hooks.dwXuidOverride = _wtoi64(xuid);

	HMODULE hSteamAPI = GetModuleHandle("steam_api.dll");

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

	Hook(oGetSteamID, hGetSteamID);

	_hooks.bXuidOverride = true;
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