//=====================================================================================

#include "../StdAfx.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	cHooks _hooks;

	LONG cHooks::VectoredExceptionHandler(_In_ LPEXCEPTION_POINTERS ExceptionInfo)
	{
		*(DWORD_PTR*)dwTacSSHandle = 0x1;

		*(BYTE*)dwOrbitalVsat = _profiler.gOrbitalVsat->Custom.bValue;
		*(BYTE*)(dwCG + 0x4809C) = _profiler.gThirdPerson->Custom.bValue;

		if (_profiler.gHardcoreHud->Custom.bValue && *(DWORD*)(dwCG + 0x68958) & 0x200)
			*(DWORD*)(dwCG + 0x68958) &= ~0x200;

		if (_profiler.gDisableEmp->Custom.bValue && *(DWORD*)(dwCG + 0x480C0) & 0x40)
			*(DWORD*)(dwCG + 0x480C0) &= ~0x40;

		if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
		{
			if (ExceptionInfo->ContextRecord->Eip == dwSysGetValueException)
			{
				ExceptionInfo->ContextRecord->Eax = dwSysValue;
				ExceptionInfo->ContextRecord->Eip += 0x3;

				return EXCEPTION_CONTINUE_EXECUTION;
			}
		}

		if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_GUARD_PAGE)
		{
			if (ExceptionInfo->ContextRecord->Eip == dwPredictPlayerState)
			{
				PredictPlayerState();
			}

			if (ExceptionInfo->ContextRecord->Eip == dwAddress)
			{
				PageGuardSVFK();
			}

			if (ExceptionInfo->ContextRecord->Eip == dwSetValueForKey)
			{
				std::string szKey(*(LPCSTR*)(ExceptionInfo->ContextRecord->Esp + 0x8));
				std::string szValue(*(LPCSTR*)(ExceptionInfo->ContextRecord->Esp + 0xC));

				if (szKey.find("name") != std::string::npos)
				{
					std::string szNameOverride(_profiler.gNameOverride->Custom.szValue);

					*(LPCSTR*)(ExceptionInfo->ContextRecord->Esp + 0xC) = szNameOverride.empty() ? GetUsername() : szNameOverride.c_str();
					PageGuardAddress(dwGetClantag);
				}

				if (szKey.find("clanAbbrev") != std::string::npos)
				{
					std::string szClanOverride(_profiler.gClanOverride->Custom.szValue);

					*(LPCSTR*)(ExceptionInfo->ContextRecord->Esp + 0xC) = szClanOverride.empty() ? GetClantag() : szClanOverride.c_str();
					PageGuardAddress(dwGetXuidstring);
				}

				if (szKey.find("xuid") != std::string::npos)
				{
					PageGuardAddress(dwGetIntPlayerStatInternal);
				}

				if (szKey.find("rank") != std::string::npos)
				{
					PageGuardAddress(dwInt64ToString);
				}

				if (szKey.find("steamid") != std::string::npos)
				{

				}
			}

			return EXCEPTION_CONTINUE_EXECUTION;
		}

		else
		{
			DWORD dwProtection = PAGE_EXECUTE | PAGE_GUARD;

			switch (ExceptionInfo->ContextRecord->Eax)
			{
			case VEH_INDEX_CONNECTPATHS:
				ExceptionInfo->ContextRecord->Eax = dwConnectPaths;

				if (ExceptionInfo->ContextRecord->Eax && ExceptionInfo->ContextRecord->Esp)
				{
					if (*(DWORD_PTR*)(ExceptionInfo->ContextRecord->Esp) == dwConnectPathsException1 &&
						*(DWORD_PTR*)(ExceptionInfo->ContextRecord->Esp + 0x8) == dwConnectPathsException2)
					{
						PageGuardSVFK();
					}
				}

				return EXCEPTION_CONTINUE_EXECUTION;

			case VEH_INDEX_MOUSEACCEL:
				ExceptionInfo->ContextRecord->Eax = dwMouseAccel;

				VirtualProtect((LPVOID)dwPredictPlayerState, 0x1, dwProtection, &dwProtection);

				return EXCEPTION_CONTINUE_EXECUTION;

			case VEH_INDEX_DRAWBIGFPS:
				ExceptionInfo->ContextRecord->Eax = dwDrawBigFPS;

				dwSysValue = Sys_GetValue(3);

				DrawBigFPS();

				return EXCEPTION_CONTINUE_EXECUTION;

			case VEH_INDEX_MAXCLIENTS:
				ExceptionInfo->ContextRecord->Eax = dwMaxClients;

				if (*(DWORD_PTR*)ExceptionInfo->ContextRecord->Esp == dwObituaryException)
				{
					KillSpam(*(DWORD_PTR*)(ExceptionInfo->ContextRecord->Esp + 0x24),
						*(DWORD_PTR*)(ExceptionInfo->ContextRecord->Esp + 0x28));
				}

				return EXCEPTION_CONTINUE_EXECUTION;

			case VEH_INDEX_NODELTA:
				ExceptionInfo->ContextRecord->Eax = dwNoDelta;

				WritePacket();

				return EXCEPTION_CONTINUE_EXECUTION;

			default:
				if (ExceptionInfo->ContextRecord->Esi == VEH_INDEX_MAXCLIENTS)
				{
					ExceptionInfo->ContextRecord->Esi = dwMaxClients;

					return EXCEPTION_CONTINUE_EXECUTION;
				}
			}
		}

		return EXCEPTION_CONTINUE_SEARCH;
	}
	/*
	//=====================================================================================
	*/
	BOOL cHooks::PageGuardSVFK()
	{
		DWORD dwProtection = PAGE_EXECUTE | PAGE_GUARD;

		return VirtualProtect((LPVOID)dwSetValueForKey, 0x4, dwProtection, &dwProtection);
	}
	/*
	//=====================================================================================
	*/
	BOOL cHooks::PageGuardAddress(DWORD address)
	{
		dwAddress = address;
		DWORD dwProtection = PAGE_EXECUTE | PAGE_GUARD;

		return VirtualProtect((LPVOID)dwAddress, 0x4, dwProtection, &dwProtection);
	}
	/*
	//=====================================================================================
	*/
	BOOL cHooks::PatchAntiCheat()
	{
		BOOL bResult = NULL;
		DWORD dwProtection = PAGE_EXECUTE_READWRITE;

		char szPatchStub[7] = { -117, -1, 85, -117, -20, -117, -55 };

		if (*(DWORD_PTR*)dwTacSSCheck)
		{
			VirtualProtect((LPVOID)dwTacSSPatch, 0x8, dwProtection, &dwProtection);

			for (int i = 0; i < 7; ++i)
				*((BYTE*)dwTacSSPatch + 0x4 * i) = szPatchStub[i];

			bResult = VirtualProtect((LPVOID)dwTacSSPatch, 0x8, dwProtection, &dwProtection);
			*(DWORD_PTR*)dwTacSSCheck = NULL;
		}

		return bResult;
	}
	/*
	//=====================================================================================
	*/
	void cHooks::DrawBigFPS()
	{
		if (LocalClientIsInGame() && CG->PlayerState.iOtherFlags & 0x4)
		{
			_targetList.GetInformation();

			if (!IsPlayerReloading(&CG->PlayerState) && WeaponAmmoAvailable())
				_aimBot.StandardAim();

			_removals.RecoilCompensation();
		}
	}
	/*
	//=====================================================================================
	*/
	void cHooks::PredictPlayerState()
	{
		if (LocalClientIsInGame() && CG->PlayerState.iOtherFlags & 0x4)
		{
			static int iBackupAngles[3];

			ClientActive = (sClientActive*)(*(DWORD_PTR*)dwClientActive + 0x42CA8);

			sUserCmd* pOldCmd = ClientActive->GetUserCmd(ClientActive->iCurrentCmd - 1);
			sUserCmd* pCurrentCmd = ClientActive->GetUserCmd(ClientActive->iCurrentCmd);
			sUserCmd* pNewCmd = ClientActive->GetUserCmd(ClientActive->iCurrentCmd + 1);

			if (_mainGui.GetKeyPress(VK_DELETE, true))
				CopyMemory(pCurrentCmd, pOldCmd, sizeof(sUserCmd));

			CopyMemory(pNewCmd, pCurrentCmd, sizeof(sUserCmd));
			++ClientActive->iCurrentCmd;

			VectorCopy(iBackupAngles, pOldCmd->iViewAngles);
			VectorCopy(pCurrentCmd->iViewAngles, iBackupAngles);

			++pOldCmd->iServerTime;
			--pCurrentCmd->iServerTime;

			if (!IsPlayerReloading(&CG->PlayerState) && WeaponAmmoAvailable())
			{
				_aimBot.SilentAim(pOldCmd);

				if (!WeaponIsVehicle())
					_aimBot.AutoFire(pCurrentCmd);
			}

			_removals.SpreadCompensation(pOldCmd, pCurrentCmd->iServerTime);
		}
	}
	/*
	//=====================================================================================
	*/
	void cHooks::WritePacket()
	{
		if (LocalClientIsInGame() && CG->PlayerState.iOtherFlags & 0x4)
		{
			ClientActive = (sClientActive*)(*(DWORD_PTR*)dwClientActive + 0x42CA8);

			sUserCmd* pUserCmd = ClientActive->GetUserCmd(ClientActive->iCurrentCmd);

			if (WeaponIsVehicle())
				_aimBot.AutoFire(pUserCmd);

			_antiAim.AntiAim(pUserCmd);
		}
	}
	/*
	//=====================================================================================
	*/
	void cHooks::KillSpam(DWORD attacker, DWORD victim)
	{
		if (LocalClientIsInGame() && CG->PlayerState.iOtherFlags & 0x4)
		{
			if (attacker == CG->iClientNum && attacker != victim)
			{
				if (_profiler.gTrickShot->Custom.bValue)
				{
					AddReliableCommand(VariadicText("mr %d -1 endround", *(DWORD_PTR*)dwServerID));
					_profiler.gTrickShot->Custom.bValue = false;
				}

				std::string szKillspam(_profiler.gKillspam->Custom.szValue);

				if (!szKillspam.empty())
				{
					szKillspam = acut::FindAndReplaceString(szKillspam, "%attacker", CG->Client[attacker].szName);
					szKillspam = acut::FindAndReplaceString(szKillspam, "%victim", CG->Client[victim].szName);
					szKillspam = acut::FindAndReplaceString(szKillspam, "%ip", 
						VariadicText("%u.%u.%u.%u", 
							(BYTE)ServerSession[victim].szIP[0], 
							(BYTE)ServerSession[victim].szIP[1], 
							(BYTE)ServerSession[victim].szIP[2], 
							(BYTE)ServerSession[victim].szIP[3]));

					AddReliableCommand(VariadicText("say \"%s\"", acut::StripColorCodes(szKillspam).c_str()));
				}
			}
		}
	}
	/*
	//=====================================================================================
	*/
	void cHooks::ProcessText(LPSTR key, LPSTR value, SIZE_T length)
	{
		if (!strcmp(key, "clanTag"))
		{
			if (strstr(value, "^H") || strstr(value, "^I"))
				strcpy_s(value, strlen(value) + 1, VariadicText("[%s]", acut::RandomANString(strlen(value) - 2).c_str()).c_str());
		}
	}
}

//=====================================================================================