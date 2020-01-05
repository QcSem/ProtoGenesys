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

		bool bXuidOverride = false;
		char szXuidOverride[0x11] = { NULL };
		QWORD dwXuidOverride = NULL;

		DWORD dwConnectPaths, dwMouseAccel, dwDrawBigFPS, dwMaxClients, dwNoDelta;
		LPVOID pVectoredExceptionHandler, pAddress;

		LONG VectoredExceptionHandler(_In_ LPEXCEPTION_POINTERS ExceptionInfo);
		typedef LONG(CALLBACK* tVectoredExceptionHandler)(_In_ LPEXCEPTION_POINTERS ExceptionInfo);

		BOOL PageGuardSVFK();
		BOOL PageGuardAddress(LPVOID address);
		BOOL PatchAntiCheat();

		void DrawBigFPS();
		void PredictPlayerState();
		void WritePacket();
		void KillSpam(DWORD attacker, DWORD victim);
		void ProcessText(LPSTR key, LPSTR value, SIZE_T length);

		cWin32Thunk<tVectoredExceptionHandler, cHooks> _thunkVectoredExceptionHandler;
	} extern _hooks;
}

//=====================================================================================