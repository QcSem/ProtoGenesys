//=====================================================================================

#include "../StdAfx.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	cHooks _hooks;

	LONG cHooks::VectoredExceptionHandler(_In_ LPEXCEPTION_POINTERS ExceptionInfo)
	{
		*(DWORD_PTR*)dwTacSSHandle = 0x1;

		CG->iThirdPerson = _profiler.gThirdPerson->Current.bValue;
		CG->PlayerState.iSatalliteTypeEnabled = _profiler.gOrbitalVsat->Current.bValue;

		if (_profiler.gHardcoreHud->Current.bValue && CG->iMatchUIVisibilityFlags & 0x200)
			CG->iMatchUIVisibilityFlags &= ~0x200;

		if (_profiler.gDisableEmp->Current.bValue && CG->PlayerState.iOtherFlags & 0x40)
			CG->PlayerState.iOtherFlags &= ~0x40;

		if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
		{
			if (ExceptionInfo->ContextRecord->Eip == dwSysGetValueException)
			{
				ExceptionInfo->ContextRecord->Eax = dwSysValue;
				ExceptionInfo->ContextRecord->Eip += 0x3;

				return EXCEPTION_CONTINUE_EXECUTION;
			}

			if (ExceptionInfo->ContextRecord->Eip == dwRegisterShaderException)
			{
				ExceptionInfo->ContextRecord->Edx = dwShader;
				ExceptionInfo->ContextRecord->Eip += 0x2;

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
					std::string szNameOverride(_profiler.gNameOverride->Current.szValue);

					*(LPCSTR*)(ExceptionInfo->ContextRecord->Esp + 0xC) = szNameOverride.empty() ? GetUsername() : szNameOverride.c_str();
					PageGuardAddress(dwGetClantag);
				}

				if (szKey.find("clanAbbrev") != std::string::npos)
				{
					std::string szClanOverride(_profiler.gClanOverride->Current.szValue);

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
				dwShader = RegisterShader("white");

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

			ClientActive = *(sClientActive**)dwClientActive;

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
			ClientActive = *(sClientActive**)dwClientActive;

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
				if (_profiler.gIdStealer->Current.bValue)
				{
					AddReliableCommand(VariadicText("userinfo \"\\name\\%s\\clanAbbrev\\%s\\xuid\\%s\"",
						CG->Client[victim].szName,
						CG->Client[victim].szClan,
						_ui64toa(CG->Client[victim].qwXuid, szXuidOverride, 0x10)));
				}

				if (_profiler.gTrickShot->Current.bValue)
				{
					AddReliableCommand(VariadicText("mr %d -1 endround", *(DWORD_PTR*)dwServerID));
					_profiler.gTrickShot->Current.bValue = false;
				}

				std::string szKillspam(_profiler.gKillspam->Current.szValue);

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
	void cHooks::CalcEntityLerpPositions(int localnum, sEntity* entity)
	{
		if (LocalClientIsInGame() && CG->PlayerState.iOtherFlags & 0x4)
		{
			if (_antiAim.IsAntiAiming() && !_mainGui.GetKeyPress(VK_DELETE, true))
			{
				if (entity->NextEntityState.iEntityNum == CG->iClientNum)
				{
					CG->Client[entity->NextEntityState.iEntityNum].vViewAngles[0] = _antiAim.vAntiAimAngles[0] + CG->PlayerState.vDeltaAngles[0];
					entity->vViewAngles[1] = _antiAim.vAntiAimAngles[1] + CG->PlayerState.vDeltaAngles[1];
				}
			}
		}
	}
	/*
	//=====================================================================================
	*/
	void cHooks::GetWorldTagMatrix(LPVOID pose, LPVOID dobj, WORD tag, Vector3 matrix[], Vector3 origin)
	{
		if (_ReturnAddress() == (LPVOID)dwGetWorldTagMatrixReturnAddress)
		{
			if (LocalClientIsInGame() && CG->PlayerState.iOtherFlags & 0x4)
			{
				if (_antiAim.IsAntiAiming() && !_mainGui.GetKeyPress(VK_DELETE, true) && _targetList.IsLocalPlayerValid())
				{
					GetPlayerViewOrigin(origin);
				}
			}
		}
	}
	/*
	//=====================================================================================
	*/
	void cHooks::GetAddr(bool renew)
	{
		std::string szIpOverride(_profiler.gIpOverride->Current.szValue);

		if (!szIpOverride.empty())
		{
			std::vector<std::string> vIpOverride = acut::SplitStringWithDelimiter(szIpOverride, ".");
			PBYTE pIP = (PBYTE)FindDmaAddy(dwXnAddr, std::vector<DWORD_PTR>({ 0xE0, 0x90, 0x38, 0x58, 0x14 }));

			pIP[0] = (BYTE)atoi(vIpOverride[0].c_str());
			pIP[1] = (BYTE)atoi(vIpOverride[1].c_str());
			pIP[2] = (BYTE)atoi(vIpOverride[2].c_str());
			pIP[3] = (BYTE)atoi(vIpOverride[3].c_str());
		}
	}
	/*
	//=====================================================================================
	*/
	int cHooks::GetItemEquipCount(LPVOID root, int _class)
	{
		return -1;
	}
	/*
	//=====================================================================================
	*/
	int cHooks::GetPlayerStatus(int localnum, DWORD xuid1, DWORD xuid2)
	{
		return 1;
	}
	/*
	//=====================================================================================
	*/
	sSteamID cHooks::GetSteamID(sSteamID steamid)
	{
		*(QWORD*)steamid.SteamID.iAll64Bits = dwXuidOverride;

		return steamid;
	}
	/*
	//=====================================================================================
	*/
	bool cHooks::GetFriendGamePlayed(sSteamID steamid, int unk1, int unk2, sFriendGameInfo* friendgameinfo)
	{
		friendgameinfo->GameID.iGameID = 202990;
		return true;
	}
	/*
	//=====================================================================================
	*/
	ePersonaState cHooks::GetFriendPersonaState(DWORD** _this, void* edx, sSteamID steamid)
	{
		return PERSONA_STATE_SNOOZE;
	}
	/*
	//=====================================================================================
	*/
	LPCSTR cHooks::GetFriendPersonaName(DWORD** _this, void* edx, sSteamID steamid)
	{
		auto Friend = std::find_if(vFriends.begin(), vFriends.end(), [&steamid](std::pair<QWORD, std::string>& _friend) { return steamid.SteamID.iAll64Bits == _friend.first; });

		return Friend->second.c_str();
	}
	/*
	//=====================================================================================
	*/
	int cHooks::GetFriendCount(DWORD** _this, void* edx, eFriendFlags friendflags)
	{
		int iFriendCount = 0;

		if (friendflags & FRIEND_FLAG_IMMEDIATE)
		{
			iFriendCount = vFriends.size();
		}

		return iFriendCount;
	}
	/*
	//=====================================================================================
	*/
	sSteamID cHooks::GetFriendByIndex(DWORD** _this, void* edx, QWORD* steamid, int _friend, eFriendFlags friendflags)
	{
		sSteamID SteamID;
		QWORD qwSpoofID = 0;

		if (friendflags & FRIEND_FLAG_IMMEDIATE)
		{
			if (_friend >= 0 && _friend < (int)vFriends.size())
			{
				qwSpoofID = vFriends[_friend].first;
			}
		}

		*steamid = qwSpoofID;
		return SteamID;
	}
	/*
	//=====================================================================================
	*/
	void cHooks::RefreshFriends()
	{
		vFriends.clear();

		std::ifstream File(acut::GetExeDirectory() + DEFAULT_TXT, std::ios_base::in);
		std::string szLine;

		while (std::getline(File, szLine))
		{
			size_t iPosition;

			if ((iPosition = szLine.find(" ")) != std::string::npos)
			{
				vFriends.push_back(make_pair(_atoi64(szLine.substr(0, iPosition).c_str()), szLine.substr(iPosition)));
			}
		}
	}
}

//=====================================================================================