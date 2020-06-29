//=====================================================================================

#include "../StdAfx.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	cHooks _hooks;

	LONG cHooks::VectoredExceptionHandler(_In_ LPEXCEPTION_POINTERS ExceptionInfo)
	{
		Dereference(dwTacSSHandle) = 0x1;

		if (CG)
		{
			if (_profiler.gOrbitalVsatAlwaysOn->Current.iValue && CG->PlayerState.iSatalliteTypeEnabled != 1)
				CG->PlayerState.iSatalliteTypeEnabled = 1;

			if (_profiler.gHardcoreHudOverlay->Current.iValue && CG->iMatchUIVisibilityFlags & 0x200)
				CG->iMatchUIVisibilityFlags &= ~0x200;

			if (_profiler.gDisableEmpOverlay->Current.iValue && CG->PlayerState.iOtherFlags & 0x40)
				CG->PlayerState.iOtherFlags &= ~0x40;
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
				std::string szKey(Dereference((LPSTR)(ExceptionInfo->ContextRecord->Esp + 0x8)));

				if (szKey.find("name") != std::string::npos)
				{
					std::string szNameOverride(_profiler.gNameOverRide->Current.szValue);

					Dereference((LPSTR)(ExceptionInfo->ContextRecord->Esp + 0xC)) = szNameOverride.empty() ? GetUsername() : VariadicText(szNameOverride);
					PageGuardAddress(dwGetClantag);
				}

				if (szKey.find("clanAbbrev") != std::string::npos)
				{
					std::string szClanOverride(_profiler.gClanOverRide->Current.szValue);

					Dereference((LPSTR)(ExceptionInfo->ContextRecord->Esp + 0xC)) = szClanOverride.empty() ? GetClantag() : VariadicText(szClanOverride);
					PageGuardAddress(dwGetXuidstring);
				}

				if (szKey.find("xuid") != std::string::npos)
				{
					std::string szXuidOverride(_profiler.gXuidOverRide->Current.szValue);

					Dereference((LPSTR)(ExceptionInfo->ContextRecord->Esp + 0xC)) = szXuidOverride.empty() ? GetXuidstring() : VariadicText(szXuidOverride);
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
					Dereference((LPSTR)(ExceptionInfo->ContextRecord->Esp + 0xC)) = VariadicText(szSteamID);
				}

				std::string szValue(Dereference((LPSTR)(ExceptionInfo->ContextRecord->Esp + 0xC)));
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
					if (Dereference(ExceptionInfo->ContextRecord->Esp) == dwConnectPathsException1 &&
						Dereference(ExceptionInfo->ContextRecord->Esp + 0x8) == dwConnectPathsException2)
					{
						PageGuardSVFK();
					}
				}

				return EXCEPTION_CONTINUE_EXECUTION;

			case VEH_INDEX_MOUSEACCEL:
				ExceptionInfo->ContextRecord->Eax = dwMouseAccel;

				VirtualProtect((LPVOID)dwPredictPlayerState, sizeof(BYTE), dwProtection, &dwProtection);

				return EXCEPTION_CONTINUE_EXECUTION;

			case VEH_INDEX_DRAWBIGFPS:
				ExceptionInfo->ContextRecord->Eax = dwDrawBigFPS;

				DrawBigFPS();

				return EXCEPTION_CONTINUE_EXECUTION;

			case VEH_INDEX_MAXCLIENTS:
				ExceptionInfo->ContextRecord->Eax = dwMaxClients;

				if (Dereference(ExceptionInfo->ContextRecord->Esp) == dwObituaryException)
				{
					Obituary(Dereference(ExceptionInfo->ContextRecord->Esp + 0x24),
						Dereference(ExceptionInfo->ContextRecord->Esp + 0x28));
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

		return VirtualProtect((LPVOID)dwSetValueForKey, sizeof(DWORD), dwProtection, &dwProtection);
	}
	/*
	//=====================================================================================
	*/
	BOOL cHooks::PageGuardAddress(DWORD address)
	{
		dwAddress = address;
		DWORD dwProtection = PAGE_EXECUTE | PAGE_GUARD;

		return VirtualProtect((LPVOID)dwAddress, sizeof(DWORD), dwProtection, &dwProtection);
	}
	/*
	//=====================================================================================
	*/
	BOOL cHooks::PatchAntiCheat()
	{
		BOOL bResult = NULL;
		DWORD dwProtection = PAGE_EXECUTE_READWRITE;

		char szPatchStub[7] = { -117, -1, 85, -117, -20, -117, -55 };

		if (Dereference(dwTacSSCheck))
		{
			VirtualProtect((LPVOID)dwTacSSPatch, sizeof(QWORD), dwProtection, &dwProtection);

			for (int i = 0; i < sizeof(szPatchStub); ++i)
				Dereference(dwTacSSPatch + sizeof(DWORD) * i) = szPatchStub[i];

			bResult = VirtualProtect((LPVOID)dwTacSSPatch, sizeof(QWORD), dwProtection, &dwProtection);
			Dereference(dwTacSSCheck) = NULL;
		}

		return bResult;
	}
	/*
	//=====================================================================================
	*/
	void cHooks::DrawBigFPS()
	{
		dwSysValue = Sys_GetValue(3);
		dwShader = RegisterShader("white");

		if (CG)
		{
			SetThirdPerson();

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
					_targetList.Priorities[i].bIsPrioritized = _targetList.Priorities[i].bIsIgnored = false;
		}
	}
	/*
	//=====================================================================================
	*/
	void cHooks::PredictPlayerState()
	{
		if (CG)
		{
			if (LocalClientIsInGame() && CG->PlayerState.iOtherFlags & 0x4)
			{
				_aimBot.SetReloadState();

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
	}
	/*
	//=====================================================================================
	*/
	void cHooks::WritePacket()
	{
		if (CG)
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
	}
	/*
	//=====================================================================================
	*/
	void cHooks::Obituary(DWORD attacker, DWORD victim)
	{
		if (CG)
		{
			if (LocalClientIsInGame() && CG->PlayerState.iOtherFlags & 0x4)
			{
				if (attacker == CG->iClientNum && attacker != victim)
				{
					if (_profiler.gNameClanXuidStealer->Current.iValue)
					{
						_profiler.gNameOverRide->Current.szValue = VariadicText(CG->ClientInfo[victim].szName);
						_profiler.gClanOverRide->Current.szValue = VariadicText(CG->ClientInfo[victim].szClan);
						_profiler.gXuidOverRide->Current.szValue = VariadicText("%llx", CG->ClientInfo[victim].qwXuid);

						AddReliableCommand(VariadicText("userinfo \"\\name\\%s\\clanAbbrev\\%s\\xuid\\%llx\"",
							CG->ClientInfo[victim].szName,
							CG->ClientInfo[victim].szClan,
							CG->ClientInfo[victim].qwXuid));
					}

					if (_profiler.gEndRoundOnNextKill->Current.iValue)
					{
						AddReliableCommand(VariadicText("mr %d -1 endround", Dereference(dwServerID)));
						_profiler.gEndRoundOnNextKill->Current.iValue = false;
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
	}
	/*
	//=====================================================================================
	*/
	void cHooks::BulletHitEvent(int localnum, int sourcenum, int targetnum, int weapon, ImVec3* start, ImVec3* position, ImVec3* normal, ImVec3* alphanormal, int surface, int eventnum, int eventparm, int contents, char bone)
	{
		if (CG)
		{
			if (LocalClientIsInGame() && CG->PlayerState.iOtherFlags & 0x4)
			{
				if (_profiler.gBulletTracers->Current.iValue)
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
		if (CG)
		{
			if (LocalClientIsInGame() && CG->PlayerState.iOtherFlags & 0x4)
			{
				if (_profiler.gThirdPersonCamera->Current.iValue && _antiAim.ReadyForAntiAim() && !_mainGui.bIsAirStuck)
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
	}
	/*
	//=====================================================================================
	*/
	void cHooks::GetWorldTagMatrix(LPVOID pose, LPVOID dobj, WORD tag, ImVec3 matrix[], ImVec3* origin)
	{
		if (CG)
		{
			if (LocalClientIsInGame() && CG->PlayerState.iOtherFlags & 0x4)
			{
				if (_profiler.gThirdPersonCamera->Current.iValue && _antiAim.ReadyForAntiAim() && !_mainGui.bIsAirStuck)
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
		std::string szIpOverride(_profiler.gIpOverRide->Current.szValue);

		if (!szIpOverride.empty())
		{
			std::vector<std::string> vIpOverride = acut::SplitStringWithDelimiter(szIpOverride, ".");
			LPBYTE lpIP = (LPBYTE)FindDmaAddy(dwXnAddr, std::vector<DWORD_PTR>({ 0xE0, 0x90, 0x38, 0x58, 0x14 }));

			lpIP[0] = (BYTE)strtol(vIpOverride[0].c_str(), NULL, 10);
			lpIP[1] = (BYTE)strtol(vIpOverride[1].c_str(), NULL, 10);
			lpIP[2] = (BYTE)strtol(vIpOverride[2].c_str(), NULL, 10);
			lpIP[3] = (BYTE)strtol(vIpOverride[3].c_str(), NULL, 10);
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
	bool cHooks::IsValidSteamID(CSteamID* steamid)
	{
		return true;
	}
	/*
	//=====================================================================================
	*/
	LPCSTR cHooks::GetPersonaName(LPCSTR name)
	{
		std::string szNameOverride(_profiler.gNameOverRide->Current.szValue);

		if (_profiler.gNameExperiencePrestigeSpam->Current.iValue)
			return VariadicText(Randomize(name));

		else if (szNameOverride.empty())
			return VariadicText(GetUsername());

		else
			return VariadicText(szNameOverride);
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
	QWORD cHooks::GetUserSteamIDAsXUID()
	{
		return qwXuidOverride;
	}
	/*
	//=====================================================================================
	*/
	std::string cHooks::Randomize(std::string name)
	{
		static std::string szNameOverride(name);

		if (!Dereference(dwConnectionState))
		{
			static int iTimer = Sys_Milliseconds();

			if (Sys_Milliseconds() - iTimer > 500)
			{
				std::random_device Device;
				std::uniform_int_distribution<int> RandomExperience(0, 1249100), RandomPrestige(0, 15), RandomColor(0, 9);

				_stats.SetRankXP(RandomExperience(Device));
				_stats.SetPLevel(RandomPrestige(Device));
				Cbuf_AddText(VariadicText("statWriteDDL clanTagStats clanName ^%i", RandomColor(Device)));
				szNameOverride = acut::RandomANString(0);

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
		CG->iThirdPerson = _profiler.gThirdPersonCamera->Current.iValue;
	}
}

//=====================================================================================