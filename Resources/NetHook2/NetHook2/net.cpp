
#include "net.h"

#include "logger.h"
#include "csimplescan.h"

#if defined(NETHOOK2_OS_WINDOWS)
	#define BuildAndAsyncPatternSig  "\x55\x8B\xEC\x83\xEC\x70\xA1\x2A\x2A\x2A\x2A\x53"
	#define BuildAndAsyncPatternMask "xxxxxxx????x"

    #define RecvPktPatternSig  "\x55\x8B\xEC\x81\xEC\xB4\x04\x00\x00\xA1\x00\x00\x00\x00\x53\x56\x57"
	#define RecvPktPatternMask "xxxxxxxxxx????xxx"

#elif defined(NETHOOK2_OS_LINUX)
	#define BuildAndAsyncPatternSig  "\x55\x8B\xEC\x83\xEC\x70\xA1\x2A\x2A\x2A\x2A\x53"
	#define BuildAndAsyncPatternMask "xxxxxxx????x"

    #define RecvPktPatternSig "\x55\x57\x56\x53\xE8\x00\x00\x00\x00\x81\xC3\x5B\x18\xCD\x01\x81\xEC\x8C\x05\x00\x00"
    #define RecvPktPatternMask "xxxxx????xxxxxxxxxxxx"
    
#endif

namespace NetHook
{


BBuildAndAsyncSendFrameFn BBuildAndAsyncSendFrame_Orig = nullptr;
RecvPktFn RecvPkt_Orig = nullptr;

CNet::CNet() noexcept
	: m_RecvPktDetour(nullptr),
	  m_BuildDetour(nullptr)
{
	CSimpleScan steamClientScan(STEAMCLIENT);

	BBuildAndAsyncSendFrameFn pBuildFunc = nullptr;
	const bool bFoundBuildFunc = steamClientScan.FindFunction(
        BuildAndAsyncPatternSig,
        BuildAndAsyncPatternMask,
		(void**)&pBuildFunc
	);

	BBuildAndAsyncSendFrame_Orig = pBuildFunc;

	g_pLogger->LogConsole("CWebSocketConnection::BBuildAndAsyncSendFrame = 0x%x\n", BBuildAndAsyncSendFrame_Orig);

	RecvPktFn pRecvPktFunc = nullptr;
	const bool bFoundRecvPktFunc = steamClientScan.FindFunction(
        RecvPktPatternSig,
        RecvPktPatternMask,
		(void**)&pRecvPktFunc
	);

	RecvPkt_Orig = pRecvPktFunc;

	g_pLogger->LogConsole("CCMInterface::RecvPkt = 0x%x\n", RecvPkt_Orig);


	if (bFoundBuildFunc)
	{
		BBuildAndAsyncSendFrameFn thisBuildFunc = CNet::BBuildAndAsyncSendFrame;

		m_BuildDetour = new CSimpleDetour((void **)&BBuildAndAsyncSendFrame_Orig, (void *)thisBuildFunc);
		m_BuildDetour->Attach();

		g_pLogger->LogConsole("Detoured CWebSocketConnection::BBuildAndAsyncSendFrame!\n");
	}
	else
	{
		g_pLogger->LogConsole("Unable to hook CWebSocketConnection::BBuildAndAsyncSendFrame: func scan failed.\n");
	}

	if (bFoundRecvPktFunc)
	{
		RecvPktFn thisRecvPktFunc = CNet::RecvPkt;

		m_RecvPktDetour = new CSimpleDetour((void **)&RecvPkt_Orig, (void *)thisRecvPktFunc);
		m_RecvPktDetour->Attach();

		g_pLogger->LogConsole("Detoured CCMInterface::RecvPkt!\n");
	}
	else
	{
		g_pLogger->LogConsole("Unable to hook CCMInterface::RecvPkt: func scan failed.\n");
	}

}

CNet::~CNet()
{
	if (m_RecvPktDetour)
	{
		m_RecvPktDetour->Detach();
		delete m_RecvPktDetour;
	}

	if (m_BuildDetour)
	{
		m_BuildDetour->Detach();
		delete m_BuildDetour;
	}
}

#if defined(NETHOOK2_OS_WINDOWS)
bool CNet::BBuildAndAsyncSendFrame(void *webSocketConnection, void *unused, EWebSocketOpCode eWebSocketOpCode, const uint8 *pubData, uint32 cubData)
{
	if (eWebSocketOpCode == EWebSocketOpCode::k_eWebSocketOpCode_Binary)
	{
		g_pLogger->LogNetMessage(ENetDirection::k_eNetOutgoing, pubData, cubData);
	}
	else
	{
		g_pLogger->LogConsole("Sending websocket frame with opcode %d (%s), ignoring\n",
			eWebSocketOpCode, EWebSocketOpCodeToName(eWebSocketOpCode)
		);
	}

	return (*BBuildAndAsyncSendFrame_Orig)(webSocketConnection, unused, eWebSocketOpCode, pubData, cubData);
}

void CNet::RecvPkt(void *cmConnection, void *unused, CNetPacket *pPacket)
{
	g_pLogger->LogNetMessage(ENetDirection::k_eNetIncoming, pPacket->m_pubData, pPacket->m_cubData);

	(*RecvPkt_Orig)(cmConnection, unused, pPacket);
}
#elif defined(NETHOOK2_OS_LINUX)
bool CNet::BBuildAndAsyncSendFrame(void *webSocketConnection, EWebSocketOpCode eWebSocketOpCode, const uint8 *pubData, uint32 cubData)
{
	if (eWebSocketOpCode == EWebSocketOpCode::k_eWebSocketOpCode_Binary)
	{
		g_pLogger->LogNetMessage(ENetDirection::k_eNetOutgoing, pubData, cubData);
	}
	else
	{
		g_pLogger->LogConsole("Sending websocket frame with opcode %d (%s), ignoring\n",
			eWebSocketOpCode, EWebSocketOpCodeToName(eWebSocketOpCode)
		);
	}

	return (*BBuildAndAsyncSendFrame_Orig)(webSocketConnection, eWebSocketOpCode, pubData, cubData);
}

void CNet::RecvPkt(void *cmConnection, CNetPacket *pPacket)
{
	g_pLogger->LogNetMessage(ENetDirection::k_eNetIncoming, pPacket->m_pubData, pPacket->m_cubData);

	(*RecvPkt_Orig)(cmConnection, pPacket);
}
#endif

}
