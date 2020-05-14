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

				if (szKey.find("name") != std::string::npos)
				{
					std::string szNameOverride(_profiler.gNameOverRide->Current.szValue);

					*(LPCSTR*)(ExceptionInfo->ContextRecord->Esp + 0xC) = szNameOverride.empty() ? GetUsername() : szNameOverride.c_str();
					PageGuardAddress(dwGetClantag);
				}

				if (szKey.find("clanAbbrev") != std::string::npos)
				{
					std::string szClanOverride(_profiler.gClanOverRide->Current.szValue);

					*(LPCSTR*)(ExceptionInfo->ContextRecord->Esp + 0xC) = szClanOverride.empty() ? GetClantag() : szClanOverride.c_str();
					PageGuardAddress(dwGetXuidstring);
				}

				if (szKey.find("xuid") != std::string::npos)
				{
					std::string szXuidOverride(_profiler.gXuidOverRide->Current.szValue);

					*(LPCSTR*)(ExceptionInfo->ContextRecord->Esp + 0xC) = szXuidOverride.empty() ? GetXuidstring() : szXuidOverride.c_str();
					PageGuardAddress(dwGetIntPlayerStatInternal);
				}

				if (szKey.find("rank") != std::string::npos)
				{
					PageGuardAddress(dwInt64ToString);
				}

				if (szKey.find("steamid") != std::string::npos)
				{
					char szSteamID[0x11];

					Int64ToString(strtoll(_profiler.gXuidOverRide->Current.szValue, NULL, 0x10), szSteamID);
					*(LPCSTR*)(ExceptionInfo->ContextRecord->Esp + 0xC) = std::string(szSteamID).empty() ? GetXuidstring() : std::string(szSteamID).c_str();
				}

				std::string szValue(*(LPCSTR*)(ExceptionInfo->ContextRecord->Esp + 0xC));
				_console.AddLog("> changed: %s to: %s", szKey.c_str(), szValue.c_str());
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

				if (ExceptionInfo->ContextRecord->Eip != 0x9B389B && ExceptionInfo->ContextRecord->Eip != 0x9B389C)
				{
					_console.AddLog("] STATUS_ACCESS_VIOLATION @ 0x%X", ExceptionInfo->ExceptionRecord->ExceptionAddress);
					Com_Error(ERR_DROP, "STATUS_ACCESS_VIOLATION @ 0x%X", ExceptionInfo->ExceptionRecord->ExceptionAddress);

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
			_drawing.CalculateTracers();
			_aimBot.SetAimState();

			if (WeaponIsVehicle() || (!IsPlayerReloading() && WeaponAmmoAvailable()))
				_aimBot.StandardAim();

			_removals.RecoilCompensation();
		}

		else
		{
			_drawing.vTracers.clear();
			_mainGui.bIsAirStuck = false;
		}

		for (int i = 0; i < MAX_CLIENTS; i++)
			if (!CG->ClientInfo[i].iInfoValid)
				_targetList.bIsPriority[i] = false;
	}
	/*
	//=====================================================================================
	*/
	void cHooks::PredictPlayerState()
	{
		if (LocalClientIsInGame() && CG->PlayerState.iOtherFlags & 0x4)
		{
			static int iBackupAngles[3];

			sUserCmd* pOldCmd = ClientActive->GetUserCmd(ClientActive->iCurrentCmd - 1);
			sUserCmd* pCurrentCmd = ClientActive->GetUserCmd(ClientActive->iCurrentCmd);
			sUserCmd* pNewCmd = ClientActive->GetUserCmd(ClientActive->iCurrentCmd + 1);

			if (_mainGui.bIsAirStuck)
				CopyMemory(pCurrentCmd, pOldCmd, sizeof(sUserCmd));

			CopyMemory(pNewCmd, pCurrentCmd, sizeof(sUserCmd));
			++ClientActive->iCurrentCmd;

			VectorCopy(iBackupAngles, pOldCmd->iViewAngles);
			VectorCopy(pCurrentCmd->iViewAngles, iBackupAngles);

			++pOldCmd->iServerTime;
			--pCurrentCmd->iServerTime;

			if (!WeaponIsVehicle() && !IsPlayerReloading() && WeaponAmmoAvailable())
			{
				_aimBot.SilentAim(pOldCmd);
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
			sUserCmd* pUserCmd = ClientActive->GetUserCmd(ClientActive->iCurrentCmd);

			if (WeaponIsVehicle())
				_aimBot.AutoFire(pUserCmd);

			_antiAim.AntiAimPitch(pUserCmd);
			_antiAim.AntiAimYaw(pUserCmd);
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
					_profiler.gNameOverRide->Current.szValue = _strdup(CG->ClientInfo[victim].szName);
					_profiler.gClanOverRide->Current.szValue = _strdup(CG->ClientInfo[victim].szClan);
					_profiler.gXuidOverRide->Current.szValue = _strdup(VariadicText("%llx", CG->ClientInfo[victim].qwXuid).c_str());

					AddReliableCommand(VariadicText("userinfo \"\\name\\%s\\clanAbbrev\\%s\\xuid\\%llx\"",
						CG->ClientInfo[victim].szName,
						CG->ClientInfo[victim].szClan,
						CG->ClientInfo[victim].qwXuid));
				}

				if (_profiler.gTrickShot->Current.bValue)
				{
					AddReliableCommand(VariadicText("mr %d -1 endround", *(DWORD_PTR*)dwServerID));
					_profiler.gTrickShot->Current.bValue = false;
				}

				std::string szKillspam(_profiler.gKillSpam->Current.szValue);

				if (!szKillspam.empty())
				{
					szKillspam = acut::FindAndReplaceString(szKillspam, "%attacker", ServerSession[attacker].szName);
					szKillspam = acut::FindAndReplaceString(szKillspam, "%victim", ServerSession[victim].szName);
					szKillspam = acut::FindAndReplaceString(szKillspam, "%ip", VariadicText("%u.%u.%u.%u",
						(BYTE)ServerSession[victim].iIPAddress[0],
						(BYTE)ServerSession[victim].iIPAddress[1],
						(BYTE)ServerSession[victim].iIPAddress[2],
						(BYTE)ServerSession[victim].iIPAddress[3]));

					AddReliableCommand(VariadicText("say \"%s\"", acut::StripColorCodes(szKillspam).c_str()));
				}
			}

			else if (victim == CG->iClientNum)
			{
				_mainGui.bIsAirStuck = false;
			}
		}
	}
	/*
	//=====================================================================================
	*/
	void cHooks::BulletHitEvent(int localnum, int sourcenum, int targetnum, int weapon, Vector3 start, Vector3 position, Vector3 normal, Vector3 alphanormal, int surface, int _event, int param, int contents, char bone)
	{
		if (LocalClientIsInGame() && CG->PlayerState.iOtherFlags & 0x4)
		{
			if (_profiler.gPlayerBulletTracers->Current.bValue)
			{
				if (sourcenum == CG->iClientNum && !EntityIsTeammate(&CG->CEntity[targetnum]) && targetnum < MAX_CLIENTS && bone >= 0)
				{
					int iShots, iIgnoreNum;
					float flRange, flSpread;
					Vector3 vTracerStart, vOrigin;
					sOrientation Orientation;

					sBulletFireParams FireParams;
					ZeroMemory(&FireParams, sizeof(sBulletFireParams));

					if (PrepFireParams(&CG->CEntity[CG->iClientNum], RegisterTag("tag_flash"), CG->CEntity[CG->iClientNum].NextEntityState.iWeaponID, 32, true, &FireParams, vTracerStart, &iShots, &flRange, &Orientation, vOrigin, &flSpread, &iIgnoreNum))
					{
						cDrawing::sTracer Tracer;

						VectorCopy(vTracerStart, Tracer.vStartPos3D);
						VectorCopy(position, Tracer.vHitPos3D);

						Tracer.cColorShadow = _profiler.gColorShadow->Current.cValue;
						Tracer.cColorHitMarker = _profiler.gColorText->Current.cValue;
						Tracer.cColorTracer = _profiler.gColorAccents->Current.cValue;
						Tracer.iStartTime = CG->PlayerState.iServerTime;

						_drawing.vTracers.push_back(Tracer);
					}
				}
			}
		}
	}
	/*
	//=====================================================================================
	*/
	void cHooks::TransitionPlayerState(int localnum, sPlayerState* playerstate, LPVOID transplayerstate)
	{
		if (_profiler.gNoFlinch->Current.bValue)
		{
			playerstate->iDamageYaw = 0xFF;
			playerstate->iDamagePitch = 0xFF;
			playerstate->iDamageCount = 0x0;
		}
	}
	/*
	//=====================================================================================
	*/
	void cHooks::CalcEntityLerpPositions(int localnum, sCEntity* entity)
	{
		if (LocalClientIsInGame() && CG->PlayerState.iOtherFlags & 0x4)
		{
			if (_profiler.gThirdPersonAntiAim->Current.bValue && _antiAim.ReadyForAntiAim() && !_mainGui.bIsAirStuck)
			{
				if (entity->NextEntityState.iEntityNum == CG->iClientNum)
				{
					if (_profiler.gAntiAimPitch->Current.iValue > cProfiler::ANTIAIM_PITCH_OFF)
						CG->ClientInfo[entity->NextEntityState.iEntityNum].vViewAngles[0] = _antiAim.vAntiAimAngles[0] + CG->PlayerState.vDeltaAngles[0];

					if (_profiler.gAntiAimYaw->Current.iValue > cProfiler::ANTIAIM_YAW_OFF)
						entity->vViewAngles[1] = _antiAim.vAntiAimAngles[1] + CG->PlayerState.vDeltaAngles[1];
				}
			}

			if (_profiler.gApplyPrediction->Current.bValue)
			{
				_mathematics.ApplyPositionPrediction(entity);
				_mathematics.ApplyAnglePrediction(entity);
			}
		}
	}
	/*
	//=====================================================================================
	*/
	void cHooks::GetWorldTagMatrix(LPVOID pose, LPVOID dobj, WORD tag, Vector3 matrix[], Vector3 origin)
	{
		if (LocalClientIsInGame() && CG->PlayerState.iOtherFlags & 0x4)
		{
			if (_profiler.gThirdPersonAntiAim->Current.bValue && _antiAim.ReadyForAntiAim() && !_mainGui.bIsAirStuck)
			{
				GetPlayerViewOrigin(origin);
			}
		}
	}
	/*
	//=====================================================================================
	*/
	void cHooks::GetAddr(bool renew)
	{
		std::string szIpOverride(_profiler.gIpOverRide->Current.szValue);

		if (!szIpOverride.empty())
		{
			std::vector<std::string> vIpOverride = acut::SplitStringWithDelimiter(szIpOverride, ".");
			PBYTE pIP = (PBYTE)FindDmaAddy(dwXnAddr, std::vector<DWORD_PTR>({ 0xE0, 0x90, 0x38, 0x58, 0x14 }));

			pIP[0] = (BYTE)strtol(vIpOverride[0].c_str(), NULL, 10);
			pIP[1] = (BYTE)strtol(vIpOverride[1].c_str(), NULL, 10);
			pIP[2] = (BYTE)strtol(vIpOverride[2].c_str(), NULL, 10);
			pIP[3] = (BYTE)strtol(vIpOverride[3].c_str(), NULL, 10);
		}
	}
	/*
	//=====================================================================================
	*/
	int cHooks::GameTypeSettings(int settings)
	{
		return 20;
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
	bool cHooks::SteamIDIsValid(DWORD** _this)
	{
		return true;
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
		return PERSONA_STATE_ONLINE;
	}
	/*
	//=====================================================================================
	*/
	LPCSTR cHooks::GetFriendPersonaName(DWORD** _this, void* edx, sSteamID steamid)
	{
		auto Friend = std::find_if(vFriends.begin(), vFriends.end(), [&steamid](std::pair<QWORD, std::string>& _friend) { return steamid.SteamID.iAll64Bits == _friend.first; });

		if (Friend != vFriends.end())
			return Friend->second.c_str();

		return NULL;
	}
	/*
	//=====================================================================================
	*/
	int cHooks::GetFriendCount(DWORD** _this, void* edx, eFriendFlags friendflags)
	{
		int iFriendCount = 0;

		if (friendflags & FRIEND_FLAG_IMMEDIATE)
			iFriendCount = vFriends.size();

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
			qwSpoofID = vFriends[_friend - iFriendCount].first;
			*steamid = qwSpoofID;
			SteamID.SteamID.iAll64Bits = *steamid;
		}

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
				vFriends.push_back(make_pair(strtoll(szLine.substr(0, iPosition).c_str(), NULL, 10), szLine.substr(iPosition)));
			}
		}
	}
}

//=====================================================================================