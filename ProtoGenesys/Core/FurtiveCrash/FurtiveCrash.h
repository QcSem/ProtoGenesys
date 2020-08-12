//=====================================================================================

#pragma once
#include "../Engine/Engine.hpp"

//=====================================================================================

enum netsrc_t
{
	NS_NULL = 0xFFFFFFFF,
	NS_CLIENT1 = 0x0,
	NS_CLIENT2 = 0x1,
	NS_CLIENT3 = 0x2,
	NS_CLIENT4 = 0x3,
	NS_SERVER = 0x4,
	NS_MAXCLIENTS = 0x4,
	NS_PACKET = 0x5,
};

//=====================================================================================

struct msg_t
{
	int overflowed;
	int readOnly;
	char* data;
	char* splitData;
	int maxsize;
	int cursize;
	int splitSize;
	int readcount;
	int bit;
	int lastEntityRef;
	int flush;
	netsrc_t targetLocalNetID;
};

//=====================================================================================

enum netadrtype_t
{
	NA_BOT = 0x0,
	NA_BAD = 0x1,
	NA_LOOPBACK = 0x2,
	NA_BROADCAST = 0x3,
	NA_IP = 0x4,
};

//=====================================================================================

struct netadr_t
{
	union
	{
		unsigned char bytes[4];
		unsigned int inaddr;
	} ip;
	unsigned __int16 port;
	netadrtype_t type;
	netsrc_t localNetID;
	unsigned __int16 serverID;
};

//=====================================================================================

struct XNADDR
{
	char addrBuff[37];
};

//=====================================================================================

struct liveAddr
{
	XNADDR xnaddr;
};

//=====================================================================================

struct PartySceNpId
{
	char handle[20];
	char opt[8];
	char reserved[8];
};

//=====================================================================================

struct __declspec(align(4)) platformNetAdr
{
	netadr_t netAddr;
	liveAddr liveaddr;
};

//=====================================================================================

struct bdSecurityID
{
	char ab[8];
};

//=====================================================================================

struct PartyMemberTeam
{
	int team;
	int switchTeam;
	int switchTeamTime;
	int lastTeam;
};

//=====================================================================================

struct PartyMember
{
	char status;
	char unknown[8];
	int ackedMembers;
	int lastPacketTime;
	int lastHeartbeatTime;
	int lastPartyStateAck;
	int lastDemoHeartBeatTime;
	unsigned int challenge;
	int subpartyIndex;
	int reportedConnState;
	bool invited;
	bool headsetPresent;
	bool inLivePartyVoice;
	bool inLivePartyTalking;
	bool finishedLoadingDemo;
	bool isReady;
	bool isGuest;
	bool isSplitscreenClient;
	int natType;
	unsigned __int64 player;
	char gamertag[32];
	int localControllerIndex;
	int playerEmblem;
	PartySceNpId npid;
	char clanAbbrev[5];
	bool clanAbbrev_IsEliteValidated;
	int codPoints;
	unsigned int affinityBits;
	platformNetAdr platformAddr;
	int availableMapPackFlags;
	bdSecurityID privatePartyId;
	union
    {
    	int rank;
    	int rankPosition;
    };
	union
    {
    	int prestige;
    	int divisionID;
    	int daysLastPlayed;
    };
	int deaths;
	float skillRating;
	float skillVariance;
	PartyMemberTeam teamInfo;
	int score;
	int vetoedMap;
	int downloadPercent;
	bool readyForPlayback;
	int maximumPing;
	int specialFlags;
	int clanTagFeature;
	int voiceConnectivityBits;
	unsigned __int64 leagueTeamID;
	int leagueMemberCount;
	int searchStartUTC;
	int uploadBandwidth;
	char probation;
	char recentPlaylistEntries[3];
	unsigned int serverchallenge;
	int serverChallengeDeadline;
};

//=====================================================================================

struct __declspec(align(8)) ClientInfo
{
	bool registered;
	bool voiceRegistered;
	unsigned __int64 xuid;
	int natType;
	netadr_t addr;
	int voiceConnectivityBits;
	int lastConnectivityTestTime[1];
	bool friends;
	int flags;
	bool muted;
	int performanceValue;
	bool privateSlot;
};

//=====================================================================================

enum ControllerIndex_t
{
	INVALID_CONTROLLER_PORT = 0xFFFFFFFF,
	CONTROLLER_INDEX_FIRST = 0x0,
	CONTROLLER_INDEX_0 = 0x0,
	CONTROLLER_INDEX_COUNT = 0x1,
};

//=====================================================================================

struct RegisteredUser
{
	bool active;
	unsigned __int64 xuid;
	bool privateSlot;
	int performanceValue;
};

//=====================================================================================

struct bdSecurityKey
{
	char ab[16];
};

//=====================================================================================

struct XSESSION_INFO
{
	bdSecurityID sessionID;
	XNADDR hostAddress;
	bdSecurityKey keyExchangeKey;
};

//=====================================================================================

struct SessionDynamicData
{
	bool sessionHandle;
	XSESSION_INFO sessionInfo;
	bool keysGenerated;
	bool sessionStartCalled;
	unsigned __int64 sessionNonce;
	int privateSlots;
	int publicSlots;
	int flags;
	bool qosListenEnabled;
	ClientInfo users[19];
	ControllerIndex_t sessionCreateController;
	int sessionDeleteTime;
	int actualPublicSlots;
	int voiceConnectivityBits;
	RegisteredUser internalRegisteredUsers[19];
};

//=====================================================================================

struct __declspec(align(4)) SessionStaticData
{
	char* sessionName;
	bool registerUsersWithVoice;
	bool isDedicated;
};

//=====================================================================================

struct SessionData
{
	SessionStaticData staticData;
	SessionDynamicData dyn;
};

//=====================================================================================

struct PartyData_s
{
private:
	SessionData* session;

public:
	SessionData* get_session_data() const
	{
		return this->session;
	}

    bool is_running() const
	{
		return *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + 0x9590) == 1;
	}

    bool in_party() const
	{
		return is_running() && *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + 0x958C);
	}

	PartyMember* get_party_member(const std::size_t index) const
	{
		return reinterpret_cast<PartyMember*>(reinterpret_cast<std::uintptr_t>(this) + 0x208 + index * sizeof(PartyMember));
	}
};

//=====================================================================================

namespace t6
{
    namespace offsets
    {
		constexpr auto cls_state = 0x11C7848;

        constexpr auto g_partyData = 0x12B2308;
	    constexpr auto g_lobbyData = 0x12A7000;

		const auto Party_GetPartyData = ProtoGenesys::bIsSteamVersion ? 0x548CF0 : 0x615150;
		const auto atoi_CL_HandleRelayPacketCall = ProtoGenesys::bIsSteamVersion ? 0x7EB870 : 0x7EC480;

	    const auto MSG_Init = ProtoGenesys::bIsSteamVersion ? 0x69D900 : 0x465E60;
        const auto MSG_WriteString = ProtoGenesys::bIsSteamVersion ? 0x623EC0 : 0x630CF0;
	    const auto NET_OutOfBandData = ProtoGenesys::bIsSteamVersion ? 0x706EC0 : 0x45C230;

		const auto CL_SendPeerData = ProtoGenesys::bIsSteamVersion ? 0x6642E0 : 0x51FA60;
		const auto MSG_WriteByte = ProtoGenesys::bIsSteamVersion ? 0x4D0500 : 0x415940;
		const auto CL_CanWeConnectToClient = ProtoGenesys::bIsSteamVersion ? 0x697EF0 : 0x639CF0;
		const auto Party_FindMemberByXUID = ProtoGenesys::bIsSteamVersion ? 0x673F60 : 0x51AD60;
		const auto Live_GetXuid = ProtoGenesys::bIsSteamVersion ? 0x54FBD0 : 0x4EA8C0;
    }

	const static auto Party_GetPartyData = reinterpret_cast<PartyData_s*(*)()>(offsets::Party_GetPartyData);

    const static auto MSG_Init = reinterpret_cast<void(*)(msg_t * buf, char* data, int length)>(offsets::MSG_Init);
	const static auto MSG_WriteString = reinterpret_cast<void(*)(msg_t * sb, const char* s)>(offsets::MSG_WriteString);
	const static auto NET_OutOfBandData = reinterpret_cast<bool(*)(netsrc_t sock, netadr_t adr, const char* format, int len)>(offsets::NET_OutOfBandData);

	const static auto CL_SendPeerData = reinterpret_cast<bool(*)(void* sessionData, int localClientNum, netsrc_t sock, int clientNum, msg_t * msg, int dataType)>(offsets::CL_SendPeerData);
	const static auto MSG_WriteByte = reinterpret_cast<void(*)(msg_t * msg, int c)>(offsets::MSG_WriteByte);
	const static auto CL_CanWeConnectToClient = reinterpret_cast<int(*)(void* sessionData, int ourClientNum, int theirClientNum)>(offsets::CL_CanWeConnectToClient);
	const static auto Party_FindMemberByXUID = reinterpret_cast<int(*)(PartyData_s * party, std::uint64_t player)>(offsets::Party_FindMemberByXUID);
	const static auto Live_GetXuid = reinterpret_cast<std::uint64_t(*)(ControllerIndex_t controllerIndex)>(offsets::Live_GetXuid);

    static auto g_partyData = reinterpret_cast<PartyData_s*>(offsets::g_partyData);
	static auto g_lobbyData = reinterpret_cast<PartyData_s*>(offsets::g_lobbyData);

	static PartyData_s* get_party_data()
	{
		return (t6::g_partyData->in_party() ? t6::g_partyData : t6::g_lobbyData);
	}
}

//=====================================================================================

namespace furtive_crash
{
    int atoi_CL_HandleRelayPacket(const char* str);

    void init();
	void free();

	void send_crash(std::size_t client_num);
	bool send_connectivity_test(std::size_t client_num);
}

//=====================================================================================