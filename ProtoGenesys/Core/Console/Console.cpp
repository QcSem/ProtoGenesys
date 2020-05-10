//=====================================================================================

#include "../../StdAfx.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	cConsole _console;

	cConsole::~cConsole()
	{
		ClearLog();

		for (int i = 0; i < vHistory.Size; i++)
			free(vHistory[i]);
	}
	/*
	//=====================================================================================
	*/
	void cConsole::Init()
	{
		static bool bFirstTime = true;

		if (bFirstTime)
		{
			ClearLog();
			ZeroMemory(szInput, sizeof(szInput));
			iHistoryPos = -1;

			vCommands.push_back("clear");
			vCommands.push_back("history");
			vCommands.push_back("quit");
			vCommands.push_back("proto_crashclient");
			vCommands.push_back("proto_crashserver");
			vCommands.push_back("proto_endround");
			vCommands.push_back("proto_name");
			vCommands.push_back("proto_clan");
			vCommands.push_back("proto_xuid");
			vCommands.push_back("proto_ip");
			vCommands.push_back("proto_killspam");
			vCommands.push_back("proto_resetstats");
			vCommands.push_back("proto_maxprestige");
			vCommands.push_back("proto_unlockall");
			vCommands.push_back("proto_disconnect");

			AddLog("Ready.");

			bFirstTime = false;
		}
	}
	/*
	//=====================================================================================
	*/
	void cConsole::ClearLog()
	{
		for (int i = 0; i < vItems.Size; i++)
			free(vItems[i]);

		vItems.clear();
		bScrollToBottom = true;
	}
	/*
	//=====================================================================================
	*/
	void cConsole::AddLog(LPCSTR format, ...) IM_FMTARGS(2)
	{
		char szBuffer[1024];
		va_list Args;
		va_start(Args, format);
		vsnprintf(szBuffer, IM_ARRAYSIZE(szBuffer), format, Args);
		szBuffer[IM_ARRAYSIZE(szBuffer) - 1] = 0;
		va_end(Args);
		vItems.push_back(Strdup(szBuffer));
		bScrollToBottom = true;
	}
	/*
	//=====================================================================================
	*/
	void cConsole::Draw()
	{
		if (_mainGui.bWriteLog)
		{
			ImGui::LogToFile();
			_mainGui.bWriteLog = false;
		}

		ImGui::TextWrapped("\t\t\tProtoGenesys");
		ImGui::Spacing();
		ImGui::TextWrapped("Press Help for details, press Tab to use text completion.");

		if (ImGui::Button("Help", ImVec2(50, 0)))
		{
			AddLog("1. proto_crashclient\n\t\tCrash clients in the lobby.");
			AddLog("2. proto_crashserver\n\t\tCrash the current server.");
			AddLog("3. proto_endround\n\t\tEnd the current round.");
			AddLog("4. proto_name <on|off> <name>\n\t\tChange your name.");
			AddLog("5. proto_clan <on|off> <clan>\n\t\tChange your clan.");
			AddLog("6. proto_xuid <on|off> <xuid>\n\t\tChange your xuid.");
			AddLog("7. proto_ip <on|off> <ip>\n\t\tChange your ip.");
			AddLog("8. proto_killspam <on|off> <message>\n\t\tSet killspam message.");
			AddLog("9. proto_resetstats\n\t\tReset your save data.");
			AddLog("10. proto_maxprestige\n\t\tSet your experience/prestige to max.");
			AddLog("11. proto_unlockall\n\t\tUnlock everything in the game.");
			AddLog("12. proto_disconnect\n\t\tDisconnect from the current server.");

			_mainGui.bWriteLog = true;
		} ImGui::SameLine();

		if (ImGui::Button("Clear", ImVec2(50, 0)))
		{
			ClearLog();
			_mainGui.bWriteLog = true;
		} ImGui::SameLine();

		bool bCopyToClipboard = ImGui::Button("Copy", ImVec2(50, 0));

		ImGui::Separator();
		ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetStyle().ItemSpacing.y), false, ImGuiWindowFlags_HorizontalScrollbar);

		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::Selectable("Clear"))
			{
				ClearLog();
				_mainGui.bWriteLog = true;
			}

			if (ImGui::Selectable("Copy"))
			{
				bCopyToClipboard = true;
				_mainGui.bWriteLog = true;
			}

			ImGui::EndPopup();
			_mainGui.bWriteLog = true;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

		if (bCopyToClipboard)
			ImGui::LogToClipboard();

		ImVec4 cDefaultText = ImGui::GetStyleColorVec4(ImGuiCol_Text);

		for (int i = 0; i < vItems.Size; i++)
		{
			LPCSTR szItem = vItems[i];
			ImVec4 cTemp = cDefaultText;

			if (strncmp(szItem, PREFIX_ERROR, strlen(PREFIX_ERROR)) == 0)
				cTemp = ImColor(1.0f, 0.4f, 0.4f, 1.0f);

			else if (strncmp(szItem, PREFIX_COMMAND, strlen(PREFIX_COMMAND)) == 0)
				cTemp = ImColor(1.0f, 0.5f, 0.3f, 1.0f);

			else if (strncmp(szItem, PREFIX_WARNING, strlen(PREFIX_WARNING)) == 0)
				cTemp = ImColor(1.0f, 1.0f, 0.3f, 1.0f);

			else if (strncmp(szItem, PREFIX_LOG, strlen(PREFIX_LOG)) == 0)
				cTemp = ImColor(0.3f, 0.5f, 1.0f, 1.0f);

			ImGui::PushStyleColor(ImGuiCol_Text, cTemp);
			ImGui::TextUnformatted(szItem);
			ImGui::PopStyleColor();
		}

		if (bCopyToClipboard)
			ImGui::LogFinish();

		if (bScrollToBottom)
			ImGui::SetScrollHere();

		bScrollToBottom = false;

		ImGui::PopStyleVar();
		ImGui::EndChild();
		ImGui::Separator();
	}
	/*
	//=====================================================================================
	*/
	void cConsole::SplitCommandLine(LPCSTR in, sCmdLine* out)
	{
		ZeroMemory(out, sizeof(sCmdLine));

		int iLength = (int)strlen(in) + 1;
		LPSTR szCmdBuffer = new char[iLength];

		strcpy_s(szCmdBuffer, iLength, in);

		LPSTR szToken = strtok(szCmdBuffer, " ");

		while (szToken != nullptr)
		{
			if (out->iArgNum == 0)
				strcpy_s(out->szCmdName, szToken);

			else
				strcpy_s(out->szCmdArgs[out->iArgNum - 1], szToken);

			szToken = strtok(nullptr, " ");

			if (szToken != nullptr)
				++out->iArgNum;
		}

		delete[] szCmdBuffer;
	}
	/*
	//=====================================================================================
	*/
	void cConsole::ExecCommand(LPCSTR command)
	{
		AddLog("# %s\n", command);

		sCmdLine CmdLine;
		SplitCommandLine(command, &CmdLine);

		iHistoryPos = -1;

		for (int i = vHistory.Size - 1; i >= 0; i--)
		{
			if (Stricmp(vHistory[i], command) == 0)
			{
				free(vHistory[i]);
				vHistory.erase(vHistory.begin() + i);
				break;
			}
		}

		vHistory.push_back(Strdup(command));

		if (!Stricmp(CmdLine.szCmdName, "clear"))
		{
			ClearLog();
		}

		else if (!Stricmp(CmdLine.szCmdName, "history"))
		{
			int iFirst = vHistory.Size - 10;

			for (int i = iFirst > 0 ? iFirst : 0; i < vHistory.Size; i++)
				AddLog("%3d: %s\n", i, vHistory[i]);
		}

		else if (!Stricmp(CmdLine.szCmdName, "quit"))
		{
			exit(EXIT_SUCCESS);
		}

		else if (!Stricmp(CmdLine.szCmdName, "proto_crashclient"))
		{
			AddLog("%s executing.", acut::ToLower(CmdLine.szCmdName).c_str());

			std::random_device rd;
			std::uniform_int_distribution<> dist(0x48, 0x49);

			std::string szNameOverride(_profiler.gNameOverRide->Current.szValue);
			std::string szXuidOverride(_profiler.gXuidOverRide->Current.szValue);

			AddReliableCommand(VariadicText("userinfo \"\\name\\%s\\clanAbbrev\\^%c%s\\xuid\\%s\"",
				szNameOverride.empty() ? GetUsername() : szNameOverride.c_str(),
				(char)dist(rd), acut::RandomANString(5).c_str(),
				szXuidOverride.empty() ? GetXuidstring() : szXuidOverride.c_str()));

			AddLog("%s executed.", acut::ToLower(CmdLine.szCmdName).c_str());
		}

		else if (!Stricmp(CmdLine.szCmdName, "proto_crashserver"))
		{
			AddLog("%s executing.", acut::ToLower(CmdLine.szCmdName).c_str());
			AddReliableCommand(VariadicText("sl %i %i", 1337, *(DWORD_PTR*)dwServerID));
			AddLog("%s executed.", acut::ToLower(CmdLine.szCmdName).c_str());
		}

		else if (!Stricmp(CmdLine.szCmdName, "proto_endround"))
		{
			AddLog("%s executing.", acut::ToLower(CmdLine.szCmdName).c_str());
			AddReliableCommand(VariadicText("mr %d -1 endround", *(DWORD_PTR*)dwServerID));
			AddLog("%s executed.", acut::ToLower(CmdLine.szCmdName).c_str());
		}

		else if (!Stricmp(CmdLine.szCmdName, "proto_name"))
		{
			if (CmdLine.iArgNum > 0)
			{
				if (!Stricmp(CmdLine.szCmdArgs[0], "on"))
				{
					char szArgBuff[512] = { NULL };

					for (int i = 1; i < CmdLine.iArgNum; i++)
						strcat_s(szArgBuff, VariadicText(i == CmdLine.iArgNum - 1 ? "%s" : "%s ", CmdLine.szCmdArgs[i]).c_str());

					LPSTR szNameOverride = strtok(szArgBuff, "\n");

					if (szNameOverride)
					{
						AddLog("%s executing.", acut::ToLower(CmdLine.szCmdName).c_str());

						_profiler.gNameOverRide->Current.szValue = Strdup(szNameOverride);

						std::string szNameOverride(_profiler.gNameOverRide->Current.szValue);
						std::string szClanOverride(_profiler.gClanOverRide->Current.szValue);
						std::string szXuidOverride(_profiler.gXuidOverRide->Current.szValue);

						AddReliableCommand(VariadicText("userinfo \"\\name\\%s\\clanAbbrev\\%s\\xuid\\%s\"",
							szNameOverride.empty() ? GetUsername() : szNameOverride.c_str(),
							szClanOverride.empty() ? GetClantag() : szClanOverride.c_str(),
							szXuidOverride.empty() ? GetXuidstring() : szXuidOverride.c_str()));

						AddLog("%s executed.", acut::ToLower(CmdLine.szCmdName).c_str());
					}

					else
					{
						AddLog("[ERROR] Null argument(s).");
					}
				}

				else if (!Stricmp(CmdLine.szCmdArgs[0], "off"))
				{
					AddLog("%s executing.", acut::ToLower(CmdLine.szCmdName).c_str());

					_profiler.gNameOverRide->Current.szValue = Strdup("");

					std::string szNameOverride(_profiler.gNameOverRide->Current.szValue);
					std::string szClanOverride(_profiler.gClanOverRide->Current.szValue);
					std::string szXuidOverride(_profiler.gXuidOverRide->Current.szValue);

					AddReliableCommand(VariadicText("userinfo \"\\name\\%s\\clanAbbrev\\%s\\xuid\\%s\"",
						szNameOverride.empty() ? GetUsername() : szNameOverride.c_str(),
						szClanOverride.empty() ? GetClantag() : szClanOverride.c_str(),
						szXuidOverride.empty() ? GetXuidstring() : szXuidOverride.c_str()));

					AddLog("%s executed.", acut::ToLower(CmdLine.szCmdName).c_str());
				}

				else
				{
					AddLog("[ERROR] Invalid argument(s).");
				}
			}

			else
			{
				AddLog("[ERROR] Missing argument(s).");
			}
		}

		else if (!Stricmp(CmdLine.szCmdName, "proto_clan"))
		{
			if (CmdLine.iArgNum > 0)
			{
				if (!Stricmp(CmdLine.szCmdArgs[0], "on"))
				{
					char szArgBuff[512] = { NULL };

					for (int i = 1; i < CmdLine.iArgNum; i++)
						strcat_s(szArgBuff, VariadicText(i == CmdLine.iArgNum - 1 ? "%s" : "%s ", CmdLine.szCmdArgs[i]).c_str());

					LPSTR szClanOverride = strtok(szArgBuff, "\n");

					if (szClanOverride)
					{
						AddLog("%s executing.", acut::ToLower(CmdLine.szCmdName).c_str());

						_profiler.gClanOverRide->Current.szValue = Strdup(szClanOverride);

						std::string szNameOverride(_profiler.gNameOverRide->Current.szValue);
						std::string szClanOverride(_profiler.gClanOverRide->Current.szValue);
						std::string szXuidOverride(_profiler.gXuidOverRide->Current.szValue);

						AddReliableCommand(VariadicText("userinfo \"\\name\\%s\\clanAbbrev\\%s\\xuid\\%s\"",
							szNameOverride.empty() ? GetUsername() : szNameOverride.c_str(),
							szClanOverride.empty() ? GetClantag() : szClanOverride.c_str(),
							szXuidOverride.empty() ? GetXuidstring() : szXuidOverride.c_str()));

						AddLog("%s executed.", acut::ToLower(CmdLine.szCmdName).c_str());
					}

					else
					{
						AddLog("[ERROR] Null argument(s).");
					}
				}

				else if (!Stricmp(CmdLine.szCmdArgs[0], "off"))
				{
					AddLog("%s executing.", acut::ToLower(CmdLine.szCmdName).c_str());

					_profiler.gClanOverRide->Current.szValue = Strdup("");

					std::string szNameOverride(_profiler.gNameOverRide->Current.szValue);
					std::string szClanOverride(_profiler.gClanOverRide->Current.szValue);
					std::string szXuidOverride(_profiler.gXuidOverRide->Current.szValue);

					AddReliableCommand(VariadicText("userinfo \"\\name\\%s\\clanAbbrev\\%s\\xuid\\%s\"",
						szNameOverride.empty() ? GetUsername() : szNameOverride.c_str(),
						szClanOverride.empty() ? GetClantag() : szClanOverride.c_str(),
						szXuidOverride.empty() ? GetXuidstring() : szXuidOverride.c_str()));

					AddLog("%s executed.", acut::ToLower(CmdLine.szCmdName).c_str());
				}

				else
				{
					AddLog("[ERROR] Invalid argument(s).");
				}
			}

			else
			{
				AddLog("[ERROR] Missing argument(s).");
			}
		}

		else if (!Stricmp(CmdLine.szCmdName, "proto_xuid"))
		{
			if (CmdLine.iArgNum > 0)
			{
				if (!Stricmp(CmdLine.szCmdArgs[0], "on"))
				{
					char szArgBuff[512] = { NULL };

					for (int i = 1; i < CmdLine.iArgNum; i++)
						strcat_s(szArgBuff, VariadicText(i == CmdLine.iArgNum - 1 ? "%s" : "%s ", CmdLine.szCmdArgs[i]).c_str());

					LPSTR szXuidOverride = strtok(szArgBuff, "\n");

					if (szXuidOverride)
					{
						AddLog("%s executing.", acut::ToLower(CmdLine.szCmdName).c_str());

						_profiler.gXuidOverRide->Current.szValue = Strdup(VariadicText("%llx", strtoll(szXuidOverride, NULL, 10)).c_str());

						std::string szNameOverride(_profiler.gNameOverRide->Current.szValue);
						std::string szClanOverride(_profiler.gClanOverRide->Current.szValue);
						std::string szXuidOverride(_profiler.gXuidOverRide->Current.szValue);

						AddReliableCommand(VariadicText("userinfo \"\\name\\%s\\clanAbbrev\\%s\\xuid\\%s\"",
							szNameOverride.empty() ? GetUsername() : szNameOverride.c_str(),
							szClanOverride.empty() ? GetClantag() : szClanOverride.c_str(),
							szXuidOverride.empty() ? GetXuidstring() : szXuidOverride.c_str()));

						AddLog("%s executed.", acut::ToLower(CmdLine.szCmdName).c_str());
					}

					else
					{
						AddLog("[ERROR] Null argument(s).");
					}
				}

				else if (!Stricmp(CmdLine.szCmdArgs[0], "off"))
				{
					AddLog("%s executing.", acut::ToLower(CmdLine.szCmdName).c_str());

					_profiler.gXuidOverRide->Current.szValue = Strdup("");

					std::string szNameOverride(_profiler.gNameOverRide->Current.szValue);
					std::string szClanOverride(_profiler.gClanOverRide->Current.szValue);
					std::string szXuidOverride(_profiler.gXuidOverRide->Current.szValue);

					AddReliableCommand(VariadicText("userinfo \"\\name\\%s\\clanAbbrev\\%s\\xuid\\%s\"",
						szNameOverride.empty() ? GetUsername() : szNameOverride.c_str(),
						szClanOverride.empty() ? GetClantag() : szClanOverride.c_str(),
						szXuidOverride.empty() ? GetXuidstring() : szXuidOverride.c_str()));

					AddLog("%s executed.", acut::ToLower(CmdLine.szCmdName).c_str());
				}

				else
				{
					AddLog("[ERROR] Invalid argument(s).");
				}
			}

			else
			{
				AddLog("[ERROR] Missing argument(s).");
			}
		}

		else if (!Stricmp(CmdLine.szCmdName, "proto_ip"))
		{
			if (CmdLine.iArgNum > 0)
			{
				if (!Stricmp(CmdLine.szCmdArgs[0], "on"))
				{
					if (!acut::SplitStringWithDelimiter(CmdLine.szCmdArgs[1], ".").empty())
					{
						if (acut::SplitStringWithDelimiter(CmdLine.szCmdArgs[1], ".").size() == 4)
						{
							AddLog("%s executing.", acut::ToLower(CmdLine.szCmdName).c_str());
							_profiler.gIpOverRide->Current.szValue = Strdup(CmdLine.szCmdArgs[1]);
							AddLog("%s executed.", acut::ToLower(CmdLine.szCmdName).c_str());
						}

						else
						{
							AddLog("[ERROR] Invalid argument(s).");
						}
					}

					else
					{
						AddLog("[ERROR] Null argument(s).");
					}
				}

				else if (!Stricmp(CmdLine.szCmdArgs[0], "off"))
				{
					AddLog("%s executing.", acut::ToLower(CmdLine.szCmdName).c_str());
					_profiler.gIpOverRide->Current.szValue = Strdup("");
					AddLog("%s executed.", acut::ToLower(CmdLine.szCmdName).c_str());
				}

				else
				{
					AddLog("[ERROR] Invalid argument(s).");
				}
			}

			else
			{
				AddLog("[ERROR] Missing argument(s).");
			}
		}

		else if (!Stricmp(CmdLine.szCmdName, "proto_killspam"))
		{
			if (CmdLine.iArgNum > 0)
			{
				if (!Stricmp(CmdLine.szCmdArgs[0], "on"))
				{
					char szArgBuff[512] = { NULL };

					for (int i = 1; i < CmdLine.iArgNum; i++)
						strcat_s(szArgBuff, VariadicText(i == CmdLine.iArgNum - 1 ? "%s" : "%s ", CmdLine.szCmdArgs[i]).c_str());

					LPSTR szKillspam = strtok(szArgBuff, "\n");

					if (szKillspam)
					{
						AddLog("%s executing.", acut::ToLower(CmdLine.szCmdName).c_str());
						_profiler.gKillSpam->Current.szValue = Strdup(szKillspam);
						AddLog("%s executed.", acut::ToLower(CmdLine.szCmdName).c_str());
					}

					else
					{
						AddLog("[ERROR] Null argument(s).");
					}
				}

				else if (!Stricmp(CmdLine.szCmdArgs[0], "off"))
				{
					AddLog("%s executing.", acut::ToLower(CmdLine.szCmdName).c_str());
					_profiler.gKillSpam->Current.szValue = Strdup("");
					AddLog("%s executed.", acut::ToLower(CmdLine.szCmdName).c_str());
				}

				else
				{
					AddLog("[ERROR] Invalid argument(s).");
				}
			}

			else
			{
				AddLog("[ERROR] Missing argument(s).");
			}
		}

		else if (!Stricmp(CmdLine.szCmdName, "proto_resetstats"))
		{
			AddLog("%s executing.", acut::ToLower(CmdLine.szCmdName).c_str());
			Cbuf_AddText("resetStats");
			AddLog("%s executed.", acut::ToLower(CmdLine.szCmdName).c_str());
		}

		else if (!Stricmp(CmdLine.szCmdName, "proto_maxprestige"))
		{
			AddLog("%s executing.", acut::ToLower(CmdLine.szCmdName).c_str());
			_stats.MaxPrestige();
			AddLog("%s executed.", acut::ToLower(CmdLine.szCmdName).c_str());
		}

		else if (!Stricmp(CmdLine.szCmdName, "proto_unlockall"))
		{
			AddLog("%s executing.", acut::ToLower(CmdLine.szCmdName).c_str());
			_stats.UnlockAll();
			AddLog("%s executed.", acut::ToLower(CmdLine.szCmdName).c_str());
		}

		else if (!Stricmp(CmdLine.szCmdName, "proto_disconnect"))
		{
			AddLog("%s executing.", acut::ToLower(CmdLine.szCmdName).c_str());
			Cbuf_AddText("disconnect");
			AddLog("%s executed.", acut::ToLower(CmdLine.szCmdName).c_str());
		}

		else
		{
			AddReliableCommand(acut::FindAndReplaceString(command, "%n", "\n"));
		}
	}
	/*
	//=====================================================================================
	*/
	int cConsole::TextEditCallbackStub(ImGuiTextEditCallbackData* data)
	{
		cConsole* Console = (cConsole*)data->UserData;
		return Console->TextEditCallback(data);
	}
	/*
	//=====================================================================================
	*/
	int cConsole::TextEditCallback(ImGuiTextEditCallbackData* data)
	{
		switch (data->EventFlag)
		{
		case ImGuiInputTextFlags_CallbackCompletion:
		{
			LPCSTR szEnd = data->Buf + data->CursorPos, szStart = szEnd;

			while (szStart > data->Buf)
			{
				const char c = szStart[-1];

				if (c == ' ' || c == '\t' || c == ',' || c == ';')
					break;

				szStart--;
			}

			ImVector<LPCSTR> vCandidates;

			for (int i = 0; i < vCommands.Size; i++)
				if (Strnicmp(vCommands[i], szStart, (int)(szEnd - szStart)) == 0)
					vCandidates.push_back(vCommands[i]);

			if (vCandidates.Size == 0)
				AddLog("No match for \"%.*s.\"\n", (int)(szEnd - szStart), szStart);

			else if (vCandidates.Size == 1)
			{
				data->DeleteChars((int)(szStart - data->Buf), (int)(szEnd - szStart));
				data->InsertChars(data->CursorPos, vCandidates[0]);
				data->InsertChars(data->CursorPos, " ");
			}

			else
			{
				int iMatchLength = (int)(szEnd - szStart);

				for (;;)
				{
					int c = 0;

					bool bAllCandidatesMatches = true;

					for (int i = 0; i < vCandidates.Size && bAllCandidatesMatches; i++)
						if (i == 0)
							c = tolower(vCandidates[i][iMatchLength]);

						else if (c == 0 || c != tolower(vCandidates[i][iMatchLength]))
							bAllCandidatesMatches = false;

					if (!bAllCandidatesMatches)
						break;

					iMatchLength++;
				}

				if (iMatchLength > 0)
				{
					data->DeleteChars((int)(szStart - data->Buf), (int)(szEnd - szStart));
					data->InsertChars(data->CursorPos, vCandidates[0], vCandidates[0] + iMatchLength);
				}

				AddLog("Possible matches:\n");

				for (int i = 0; i < vCandidates.Size; i++)
					AddLog("- %s\n", vCandidates[i]);
			}

			break;
		}
		case ImGuiInputTextFlags_CallbackHistory:
		{
			const int iPreviousHistoryPos = iHistoryPos;

			if (data->EventKey == ImGuiKey_UpArrow)
			{
				if (iHistoryPos == -1)
					iHistoryPos = vHistory.Size - 1;

				else if (iHistoryPos > 0)
					iHistoryPos--;
			}

			else if (data->EventKey == ImGuiKey_DownArrow)
			{
				if (iHistoryPos != -1)
					if (++iHistoryPos >= vHistory.Size)
						iHistoryPos = -1;
			}

			if (iPreviousHistoryPos != iHistoryPos)
			{
				data->CursorPos = data->SelectionStart = data->SelectionEnd = data->BufTextLen = (int)snprintf(data->Buf, (size_t)data->BufSize, "%s", (iHistoryPos >= 0) ? vHistory[iHistoryPos] : "");
				data->BufDirty = true;
			}
		}
		}
		return 0;
	}
}