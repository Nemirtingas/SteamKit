#include "os.h"
#include "crypto.h"

#include "logger.h"
#include "csimplescan.h"

#include <cstddef>


#if defined(NETHOOK2_OS_WINDOWS)
	#define EncryptPatternSig "\x55\x8B\xEC\x6A\x01\xFF\x75\x24"
	#define EncryptPatternMask "xxxxxxxx"

	#define MsgNameFromEMsgPatternSig "\x55\x8B\xEC\x51\x56\xE8\x00\x00\x00\x00\x8B\x00\x00\x00\x00\x00\x8B\xF0"
	#define MsgNameFromEMsgPatternMask "xxxxxx????x?????xx"

#elif defined(NETHOOK2_OS_LINUX)
	#define EncryptPatternSig "\x55\x57\x56\x83\xEC\x08\x8B\x4C"
	#define EncryptPatternMask "xxxxxxxx"

	#define MsgNameFromEMsgPatternSig "\x57\x56\x53\xE8\x00\x00\x00\x00\x81\xC3\x9C\x04\xCD\x00\x83\xEC\x20"
	#define MsgNameFromEMsgPatternMask "xxxx????xxxxxxxxx"

#endif
#include <assert.h>



SymmetricEncryptChosenIVFn Encrypt_Orig = nullptr;
PchMsgNameFromEMsgFn PchMsgNameFromEMsg = nullptr;

static_assert(sizeof(MsgInfo_t) == 20, "Wrong size of MsgInfo_t");
static_assert(offsetof(MsgInfo_t, eMsg) == 0, "Wrong offset of MsgInfo_t::eMsg");
static_assert(offsetof(MsgInfo_t, nFlags) == 4, "Wrong offset of MsgInfo_t::nFlags");
static_assert(offsetof(MsgInfo_t, k_EServerTarget) == 8, "Wrong offset of MsgInfo_t::k_EServerTarget");
static_assert(offsetof(MsgInfo_t, nUnk1) == 12, "Wrong offset of MsgInfo_t::uUnk1");
static_assert(offsetof(MsgInfo_t, pchMsgName) == 16, "Wrong offset of MsgInfo_t::pchMsgName");

typedef std::pair<EMsg, MsgInfo_t *> MsgPair;

CCrypto::CCrypto() noexcept
	: Encrypt_Detour( nullptr )
{
	CSimpleScan steamClientScan( STEAMCLIENT );


	SymmetricEncryptChosenIVFn pEncrypt = nullptr;
	const bool bEncrypt = steamClientScan.FindFunction(
		EncryptPatternSig,
		EncryptPatternMask,
		(void **)&pEncrypt
	);

	Encrypt_Orig = pEncrypt;

	g_pLogger->LogConsole( "CCrypto::SymmetricEncryptChosenIV = 0x%x\n", Encrypt_Orig );

	const bool bPchMsgNameFromEMsg = steamClientScan.FindFunction(
		MsgNameFromEMsgPatternSig,
		MsgNameFromEMsgPatternMask,
		(void**)&PchMsgNameFromEMsg
	);

	if (bPchMsgNameFromEMsg)
	{
		g_pLogger->LogConsole( "PchMsgNameFromEMsg = 0x%x\n", PchMsgNameFromEMsg);
	}
	else
	{
		g_pLogger->LogConsole( "Unable to find PchMsgNameFromEMsg.\n" );
	}

	SymmetricEncryptChosenIVFn encrypt = CCrypto::SymmetricEncryptChosenIV;

	if ( bEncrypt )
	{
		Encrypt_Detour = new CSimpleDetour((void **) &Encrypt_Orig, (void*) encrypt);
		Encrypt_Detour->Attach();

		g_pLogger->LogConsole( "Detoured SymmetricEncryptChosenIV!\n" );
	}
	else
	{
		g_pLogger->LogConsole( "Unable to hook SymmetricEncryptChosenIV: Func scan failed.\n" );
	}
}

CCrypto::~CCrypto()
{
	if ( Encrypt_Detour )
	{
		Encrypt_Detour->Detach();
		delete Encrypt_Detour;
	}
}



bool __cdecl CCrypto::SymmetricEncryptChosenIV( const uint8 *pubPlaintextData, uint32 cubPlaintextData, const uint8 *pIV, uint32 cubIV, uint8 *pubEncryptedData, uint32 *pcubEncryptedData, const uint8 *pubKey, uint32 cubKey )
{
	g_pLogger->LogNetMessage( ENetDirection::k_eNetOutgoing, pubPlaintextData, cubPlaintextData );

	return (*Encrypt_Orig)( pubPlaintextData, cubPlaintextData, pIV, cubIV, pubEncryptedData, pcubEncryptedData, pubKey, cubKey );
}



const char* CCrypto::GetMessage( EMsg eMsg, uint8 serverType )
{
	if(PchMsgNameFromEMsg != nullptr)
	{
		return PchMsgNameFromEMsg(eMsg);
	}

	return nullptr;
}
