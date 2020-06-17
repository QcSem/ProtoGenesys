//=====================================================================================

#include "../StdAfx.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	cHooks _hooks;

	LONG cHooks::VectoredExceptionHandler(_In_ LPEXCEPTION_POINTERS ExceptionInfo)
	{
		*(DWORD_PTR*)dwTacSSHandle = 0x1;

		if (_profiler.gOrbitalVsat->Current.iValue && CG->PlayerState.iSatalliteTypeEnabled != 1)
			CG->PlayerState.iSatalliteTypeEnabled = 1;

		if (_profiler.gHardcoreHud->Current.iValue && CG->iMatchUIVisibilityFlags & 0x200)
			CG->iMatchUIVisibilityFlags &= ~0x200;

		if (_profiler.gDisableEmp->Current.iValue && CG->PlayerState.iOtherFlags & 0x40)
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

					*(LPCSTR*)(ExceptionInfo->ContextRecord->Esp + 0xC) = szNameOverride.empty() ? GetUsername() : _strdup(szNameOverride.c_str());
					PageGuardAddress(dwGetClantag);
				}

				if (szKey.find("clanAbbrev") != std::string::npos)
				{
					std::string szClanOverride(_profiler.gClanOverRide->Current.szValue);

					*(LPCSTR*)(ExceptionInfo->ContextRecord->Esp + 0xC) = szClanOverride.empty() ? GetClantag() : _strdup(szClanOverride.c_str());
					PageGuardAddress(dwGetXuidstring);
				}

				if (szKey.find("xuid") != std::string::npos)
				{
					std::string szXuidOverride(_profiler.gXuidOverRide->Current.szValue);

					*(LPCSTR*)(ExceptionInfo->ContextRecord->Esp + 0xC) = szXuidOverride.empty() ? GetXuidstring() : _strdup(szXuidOverride.c_str());
					PageGuardAddress(dwGetIntPlayerStatInternal);
				}

				if (szKey.find("rank") != std::string::npos)
				{
					PageGuardAddress(dwInt64ToString);
				}

				if (szKey.find("steamid") != std::string::npos)
				{
					char szSteamID[0x11] = { NULL };
					std::string szXuidOverride(_profiler.gXuidOverRide->Current.szValue);

					Int64ToString(strtoll(szXuidOverride.empty() ? GetXuidstring() : szXuidOverride.c_str(), NULL, 0x10), szSteamID);
					*(LPCSTR*)(ExceptionInfo->ContextRecord->Esp + 0xC) = std::string(szSteamID).c_str();
				}

				std::string szValue(*(LPCSTR*)(ExceptionInfo->ContextRecord->Esp + 0xC));
				_console.AddLog("%s changed: %s to: %s", PREFIX_LOG, szKey.c_str(), szValue.c_str());
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
					_console.AddLog("%s STATUS_ACCESS_VIOLATION @ 0x%X", PREFIX_WARNING, ExceptionInfo->ExceptionRecord->ExceptionAddress);
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
			SetThirdPerson();

			_targetList.GetInformation();
			_drawing.CalculateTracers();
			_aimBot.SetAimState();
			_aimBot.SetReloadState();

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
				_targetList.Priorities[i].bIsPrioritized = _targetList.Priorities[i].bIsIgnored = false;
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

			pOldCmd->iViewAngles[0] = iBackupAngles[0];
			pOldCmd->iViewAngles[1] = iBackupAngles[1];
			pOldCmd->iViewAngles[2] = iBackupAngles[2];

			iBackupAngles[0] = pCurrentCmd->iViewAngles[0];
			iBackupAngles[1] = pCurrentCmd->iViewAngles[1];
			iBackupAngles[2] = pCurrentCmd->iViewAngles[2];

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
			{
				_aimBot.SilentAim(pUserCmd);
				_aimBot.AutoFire(pUserCmd);
			}

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
				if (_profiler.gIdStealer->Current.iValue)
				{
					_profiler.gNameOverRide->Current.szValue = _strdup(CG->ClientInfo[victim].szName);
					_profiler.gClanOverRide->Current.szValue = _strdup(CG->ClientInfo[victim].szClan);
					_profiler.gXuidOverRide->Current.szValue = _strdup(VariadicText("%llx", CG->ClientInfo[victim].qwXuid).c_str());

					AddReliableCommand(VariadicText("userinfo \"\\name\\%s\\clanAbbrev\\%s\\xuid\\%llx\"",
						CG->ClientInfo[victim].szName,
						CG->ClientInfo[victim].szClan,
						CG->ClientInfo[victim].qwXuid));
				}

				if (_profiler.gTrickShot->Current.iValue)
				{
					AddReliableCommand(VariadicText("mr %d -1 endround", *(DWORD_PTR*)dwServerID));
					_profiler.gTrickShot->Current.iValue = false;
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
	void cHooks::BulletHitEvent(int localnum, int sourcenum, int targetnum, int weapon, ImVec3* start, ImVec3* position, ImVec3* normal, ImVec3* alphanormal, int surface, int eventnum, int eventparm, int contents, char bone)
	{
		if (LocalClientIsInGame() && CG->PlayerState.iOtherFlags & 0x4)
		{
			if (_profiler.gPlayerBulletTracers->Current.iValue)
			{
				if (sourcenum == CG->iClientNum && !EntityIsTeammate(&CG->CEntity[targetnum]) && targetnum < MAX_CLIENTS && bone >= 0)
				{
					int iShots, iIgnoreNum;
					float flRange, flSpread;
					ImVec3 vTracerStart, vOrigin;
					sOrientation Orientation;

					sBulletFireParams FireParams;
					ZeroMemory(&FireParams, sizeof(sBulletFireParams));

					if (PrepFireParams(&CG->CEntity[CG->iClientNum], RegisterTag("tag_flash"), CG->CEntity[CG->iClientNum].NextEntityState.iWeaponID, 32, true, &FireParams, &vTracerStart, &iShots, &flRange, &Orientation, &vOrigin, &flSpread, &iIgnoreNum))
					{
						cDrawing::sTracer Tracer;

						Tracer.iStartTime = Sys_Milliseconds();
						Tracer.vStartPos3D = vTracerStart;
						Tracer.vHitPos3D = *position;

						Tracer.cColorShadow = _profiler.gColorShadow->Current.cValue;
						Tracer.cColorHitMarker = _profiler.gColorText->Current.cValue;
						Tracer.cColorTracer = _profiler.gColorAccents->Current.cValue;

						Tracer.flAlphaShadow = _profiler.gColorShadow->Current.cValue.w;
						Tracer.flAlphaHitMarker = _profiler.gColorText->Current.cValue.w;
						Tracer.flAlphaTracer = _profiler.gColorAccents->Current.cValue.w;

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
		if (_profiler.gNoFlinch->Current.iValue)
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
			if (_profiler.gThirdPerson->Current.iValue && _antiAim.ReadyForAntiAim() && !_mainGui.bIsAirStuck)
			{
				if (entity->NextEntityState.iEntityNum == CG->iClientNum)
				{
					if (_profiler.gAntiAimPitch->Current.iValue > cProfiler::ANTIAIM_PITCH_OFF)
						CG->ClientInfo[entity->NextEntityState.iEntityNum].vViewAngles.x = _antiAim.vAntiAimAngles.x + CG->PlayerState.vDeltaAngles.x;

					if (_profiler.gAntiAimYaw->Current.iValue > cProfiler::ANTIAIM_YAW_OFF)
						entity->vViewAngles.y = _antiAim.vAntiAimAngles.y + CG->PlayerState.vDeltaAngles.y;
				}
			}

			if (_profiler.gApplyPrediction->Current.iValue)
			{
				_mathematics.ApplyPositionPrediction(entity);
				_mathematics.ApplyAnglePrediction(entity);
			}
		}
	}
	/*
	//=====================================================================================
	*/
	void cHooks::GetWorldTagMatrix(LPVOID pose, LPVOID dobj, WORD tag, ImVec3 matrix[], ImVec3* origin)
	{
		if (LocalClientIsInGame() && CG->PlayerState.iOtherFlags & 0x4)
		{
			if (_profiler.gThirdPerson->Current.iValue && _antiAim.ReadyForAntiAim() && !_mainGui.bIsAirStuck)
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
			LPBYTE pIP = (LPBYTE)FindDmaAddy(dwXnAddr, std::vector<DWORD_PTR>({ 0xE0, 0x90, 0x38, 0x58, 0x14 }));

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
	int cHooks::GetPlayerStatus(int localnum, QWORD xuid)
	{
		return 1;
	}
	/*
	//=====================================================================================
	*/
	bool cHooks::SteamIDIsValid(CSteamID* steamid)
	{
		return true;
	}
	/*
	//=====================================================================================
	*/
	CSteamID* cHooks::GetSteamID(CSteamID* steamid)
	{
		steamid->SetFromUint64(qwXuidOverride);

		return steamid;
	}
	/*
	//=====================================================================================
	*/
	LPCSTR cHooks::GetPersonaName(LPCSTR name)
	{
		std::string szNameOverride(_profiler.gNameOverRide->Current.szValue);

		if (_profiler.gNamePrestigeSpam->Current.iValue)
			return _strdup(Randomize(name).c_str());

		else if (szNameOverride.empty())
			return _strdup(GetUsername());

		else
			return _strdup(szNameOverride.c_str());
	}
	/*
	//=====================================================================================
	*/
	int cHooks::GetFriendCount(LPVOID ecx, LPVOID edx, EFriendFlags friendflags)
	{
		int iFriendCount = 0;

		if (friendflags & k_EFriendFlagImmediate)
			iFriendCount = vFriends.size();

		return iFriendCount;
	}
	/*
	//=====================================================================================
	*/
	void cHooks::GetFriendByIndex(LPVOID ecx, LPVOID edx, CSteamID* steamid, int index, EFriendFlags friendflags)
	{
		auto SteamID = k_steamIDNil;

		if (friendflags & k_EFriendFlagImmediate)
			SteamID.SetFromUint64(vFriends[index - iFriendCount].first);

		steamid->SetFromUint64(SteamID.ConvertToUint64());
	}
	/*
	//=====================================================================================
	*/
	EPersonaState cHooks::GetFriendPersonaState(LPVOID ecx, LPVOID edx, CSteamID steamid)
	{
		return k_EPersonaStateOnline;
	}
	/*
	//=====================================================================================
	*/
	LPCSTR cHooks::GetFriendPersonaName(LPVOID ecx, LPVOID edx, CSteamID steamid)
	{
		auto Friend = std::find_if(vFriends.begin(), vFriends.end(), [&steamid](std::pair<QWORD, std::string>& _friend) { return steamid.ConvertToUint64() == _friend.first; });

		if (Friend != vFriends.end())
			return Friend->second.c_str();

		return NULL;
	}
	/*
	//=====================================================================================
	*/
	bool cHooks::GetFriendGamePlayed(CSteamID steamid, int unk1, int unk2, FriendGameInfo_t* friendgameinfo)
	{
		friendgameinfo->m_gameID.Set(202990);
		return true;
	}
	/*
	//=====================================================================================
	*/
	int cHooks::Atoi1(int result)
	{
		if (result > 0xFA)
		{
			std::ofstream file(acut::GetExeDirectory() + acut::FindAndReplaceString(DEFAULT_DMP, " ", ""), std::ios_base::out | std::ios_base::app);
			file << std::hex << result << std::endl;

			_console.AddLog("%s RCE_ATTEMPT_BLOCKED @ 0x%X", PREFIX_WARNING, result);

			result = 0;
		}

		return result;
	}
	/*
	//=====================================================================================
	*/
	int cHooks::Atoi2(int result)
	{
		if (result > 0xA)
		{
			std::ofstream file(acut::GetExeDirectory() + acut::FindAndReplaceString(DEFAULT_DMP, " ", ""), std::ios_base::out | std::ios_base::app);
			file << std::hex << (0x2E448C80 + 0x4 * result + 0x6885C) << std::endl;

			_console.AddLog("%s RCE_ATTEMPT_BLOCKED @ 0x%X", PREFIX_WARNING, (0x2E448C80 + 0x4 * result + 0x6885C));

			result = 0;
		}

		return result;
	}
	/*
	//=====================================================================================
	*/
	std::string cHooks::Randomize(std::string name)
	{
		static std::string szNameOverride(name);

		if (!*(int*)dwConnectionState)
		{
			static int iMode = 1;
			static int iTimer = Sys_Milliseconds();

			if (Sys_Milliseconds() - iTimer > 500)
			{
				std::random_device Device;
				std::uniform_int_distribution<int> RandomPrestige(0, 15), RandomColor(0, 9);

				_stats.SetPLevel(RandomPrestige(Device));
				Cbuf_AddText(VariadicText("statWriteDDL clanTagStats clanName ^%i", RandomColor(Device)));

				switch (iMode)
				{
				case 1:
					szNameOverride = "8======mD~~~   ";
					iMode = 2;
					break;
				case 2:
					szNameOverride = "8=====m=D ~~~  ";
					iMode = 3;
					break;
				case 3:
					szNameOverride = "8====m==D  ~~~ ";
					iMode = 4;
					break;
				case 4:
					szNameOverride = "8===m===D   ~~~";
					iMode = 5;
					break;
				case 5:
					szNameOverride = "8==m====D~   ~~";
					iMode = 6;
					break;
				case 6:
					szNameOverride = "8=m=====D~~   ~";
					iMode = 7;
					break;
				case 7:
					szNameOverride = "8m======D~~~   ";
					iMode = 8;
					break;
				case 8:
					szNameOverride = "8=m=====D ~~~  ";
					iMode = 9;
					break;
				case 9:
					szNameOverride = "8==m====D  ~~~ ";
					iMode = 10;
					break;
				case 10:
					szNameOverride = "8===m===D   ~~~";
					iMode = 11;
					break;
				case 11:
					szNameOverride = "8====m==D~   ~~";
					iMode = 12;
					break;
				case 12:
					szNameOverride = "8=====m=D~~   ~";
					iMode = 1;
					break;
				}

				iTimer = Sys_Milliseconds();
			}
		}

		return szNameOverride;
	}
	/*
	//=====================================================================================
	*/
	void cHooks::RefreshFriends()
	{
		vFriends.clear();

		std::ifstream File(acut::GetExeDirectory() + acut::FindAndReplaceString(DEFAULT_TXT, " ", ""), std::ios_base::in);
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
	/*
	//=====================================================================================
	*/
	void cHooks::SetThirdPerson()
	{
		std::string szDvarName = IsThirdPerson() ? "cg_fov_default_thirdperson" : "cg_fov";
		FindVar(szDvarName)->Current.flValue = _profiler.gFieldOfView->Current.flValue;
		CG->iThirdPerson = _profiler.gThirdPerson->Current.iValue;
	}
}

//=====================================================================================