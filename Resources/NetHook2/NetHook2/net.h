
#ifndef NETHOOK_NET_H_
#define NETHOOK_NET_H_
#ifdef _WIN32
#pragma once
#endif

#include "os.h"

#include "steam/steamtypes.h"
#include "steam/net.h"

#include "csimpledetour.h"

namespace NetHook
{

class CNet
{

public:
	CNet() noexcept;
	~CNet();


public:
#if defined(NETHOOK2_OS_WINDOWS)
	// CWebSocketConnection::BBuildAndAsyncSendFrame(EWebSocketOpCode, uchar const*, int)
	static bool NETHOOK2_FASTCALL BBuildAndAsyncSendFrame(void *webSocketConnection, void *unused, EWebSocketOpCode eWebSocketOpCode, const uint8 *pubData, uint32 cubData);

	// CCMInterface::RecvPkt(CNetPacket *)
	static void NETHOOK2_FASTCALL RecvPkt(void *cmConnection, void *unused, CNetPacket *pPacket);
#elif defined(NETHOOK2_OS_LINUX)
	// CWebSocketConnection::BBuildAndAsyncSendFrame(EWebSocketOpCode, uchar const*, int)
	static bool NETHOOK2_CDECL BBuildAndAsyncSendFrame(void *webSocketConnection, EWebSocketOpCode eWebSocketOpCode, const uint8 *pubData, uint32 cubData);

	// CCMInterface::RecvPkt(CNetPacket *)
	static void NETHOOK2_CDECL RecvPkt(void *cmConnection, CNetPacket *pPacket);
#endif

private:
	CSimpleDetour *m_RecvPktDetour;
	CSimpleDetour *m_BuildDetour;

};

extern CNet *g_pNet;

typedef decltype(&CNet::BBuildAndAsyncSendFrame) BBuildAndAsyncSendFrameFn;
typedef decltype(&CNet::RecvPkt) RecvPktFn;


}

#endif // !NETHOOK_NET_H_
