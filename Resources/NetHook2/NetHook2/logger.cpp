
#include "logger.h"

#include <cstdio>
#include <ctime>
#include <sstream>
#include <fstream>
#include <cstdarg>

#include "crypto.h"
#include "zip.h"
#include "binaryreader.h"

#include "steammessages_base.pb.h"

#if defined(NETHOOK2_OS_WINDOWS)
#elif defined(NETHOOK2_OS_LINUX)

#define vsprintf_s(s, s_size, format, args) vsprintf(s, format, args)
#define sprintf_s(s, s_size, format, ...) sprintf(s, format, __VA_ARGS__)

int _vscprintf (const char * format, va_list pargs)
{
	int retval;
	va_list argcopy;
	va_copy(argcopy, pargs);
	retval = vsnprintf(NULL, 0, format, argcopy);
	va_end(argcopy);
	return retval;
}

#endif

#ifndef MAX_PATH
#define MAX_PATH 4096
#endif

CLogger::CLogger() noexcept
{
	m_uiMsgNum = 0;

	m_RootDir = get_exec_path();
	m_RootDir = m_RootDir.substr( 0, m_RootDir.find_last_of( PATH_SEPARATOR ) );
	m_RootDir += PATH_SEPARATOR;
	m_RootDir += "nethook";
	m_RootDir += PATH_SEPARATOR;

	// create root nethook log directory if it doesn't exist
	create_directory( m_RootDir );

	time_t currentTime;
	time( &currentTime );

	std::ostringstream ss;
	ss << m_RootDir << currentTime << PATH_SEPARATOR;
	m_LogDir = ss.str();

	// create the session log directory
	create_directory( m_LogDir );
}


void CLogger::LogConsole( const char *szFmt, ... )
{
	va_list args;
	va_start( args, szFmt );

	int buffSize = _vscprintf( szFmt, args ) + 1;

	if ( buffSize <= 1 )
		return;

	char *szBuff = new char[ buffSize ];
	memset( szBuff, 0, buffSize );

	const int len = vsprintf_s( szBuff, buffSize, szFmt, args );

	szBuff[ buffSize - 1 ] = 0;

#if defined(NETHOOK2_OS_WINDOWS)
	HANDLE hOutput = GetStdHandle( STD_OUTPUT_HANDLE );

	WriteFile( hOutput, szBuff, len, &numWritten, nullptr );

#elif defined(NETHOOK2_OS_LINUX)
    std::string t(szBuff);
    t = "NetHook2: " + t;
	fputs(t.c_str(), stderr);
#endif

	delete [] szBuff;
}

void CLogger::DeleteFile( const char *szFileName, bool bSession )
{
	std::string outputFile = ( bSession ? m_LogDir : m_RootDir );
	outputFile += szFileName;

	delete_file( outputFile );
}

void CLogger::LogNetMessage( ENetDirection eDirection, const uint8 *pData, uint32 cubData )
{
	EMsg eMsg = (EMsg)*(uint16*)pData;
	eMsg = (EMsg)((int)eMsg & (~0x80000000));

	if ( eMsg == EMsg::k_EMsgMulti )
	{
		this->MultiplexMulti( eDirection, pData, cubData );
		return;
	}

	this->LogSessionData( eDirection, pData, cubData );
}

void CLogger::LogSessionData( ENetDirection eDirection, const uint8 *pData, uint32 cubData )
{
	std::string fullFile = m_LogDir;

	const char *outFile = GetFileNameBase( eDirection, (EMsg)*(uint16*)pData );
	fullFile += outFile;

	std::string fullFileTmp = fullFile + ".tmp";
	std::string fullFileFinal = fullFile + ".bin";
	std::ofstream file( fullFileTmp, std::ios::binary | std::ios::out | std::ios::trunc );

	file.write( (const char*)pData, cubData );

	file.close();

	move_file( fullFileTmp, fullFileFinal );

	this->LogConsole( "Wrote %d bytes to %s\n", cubData, outFile );
}

void* CLogger::OpenFile( const char *szFileName, bool bSession )
{
	std::string outputFile = ( bSession ? m_LogDir : m_RootDir );
	outputFile += szFileName;

	return new std::fstream( outputFile, std::ios::binary | std::ios::trunc | std::ios::out );
}

void CLogger::CloseFile( void* hFile ) noexcept
{
	delete reinterpret_cast<std::fstream*>(hFile);
}

void CLogger::LogOpenFile( void* hFile, const char *szFmt, ... )
{
	std::fstream& file = *reinterpret_cast<std::fstream*>(hFile);
	va_list args;
	va_start( args, szFmt );

	int buffSize = _vscprintf( szFmt, args ) + 1;

	if ( buffSize <= 1 )
		return;

	char *szBuff = new char[ buffSize ];
	memset( szBuff, 0, buffSize );

	const int len = vsprintf_s( szBuff, buffSize, szFmt, args );

	szBuff[ buffSize - 1 ] = 0;

	file.seekp(0, std::ios::end);

	file.write( szBuff, len );

	delete [] szBuff;
}

const char *CLogger::GetFileNameBase( ENetDirection eDirection, EMsg eMsg, uint8 serverType )
{
	static char szFileName[MAX_PATH];

	sprintf_s(
		szFileName, sizeof( szFileName ),
		"%03u_%s_%d_%s",
		++m_uiMsgNum,
		( eDirection == ENetDirection::k_eNetIncoming ? "in" : "out" ),
		static_cast<int>( eMsg ),
		g_pCrypto->GetMessage( eMsg, serverType )
	);

	return szFileName;
}

void CLogger::MultiplexMulti( ENetDirection eDirection, const uint8 *pData, uint32 cubData )
{
	struct ProtoHdr 
	{
		EMsg msg;
		int headerLength;
	};


	const ProtoHdr *pProtoHdr = (ProtoHdr*) pData;

	g_pLogger->LogConsole("Multi: msg %d length %d\n", (static_cast<int>(pProtoHdr->msg) & (~0x80000000)), pProtoHdr->headerLength );

	CMsgProtoBufHeader protoheader;
	protoheader.ParseFromArray( pData + 8, pProtoHdr->headerLength );

	g_pLogger->LogConsole("MultiProto\n");

	CMsgMulti multi;
	multi.ParseFromArray( pData + 8 + pProtoHdr->headerLength, cubData - 8 - pProtoHdr->headerLength );

	g_pLogger->LogConsole("MultiMsg: %d %d\n", multi.size_unzipped(), multi.message_body().length() );

	uint8 *pMsgData = nullptr;
	uint32 cubMsgData = 0;
	bool bDecomp = false;

	if ( multi.has_size_unzipped() && multi.size_unzipped() != 0 )
	{
		// decompress our data

		uint8 *pDecompressed = new uint8[ multi.size_unzipped() ];
		uint8 *pCompressed = (uint8 *)( multi.message_body().c_str() );
		const uint32 cubCompressed = multi.message_body().length();

		g_pLogger->LogConsole("decomp: %x comp: %x cubcomp: %d unzipped: %d\n", pDecompressed, pCompressed, cubCompressed, multi.size_unzipped());

		const bool bZip = CZip::Inflate( pCompressed, cubCompressed, pDecompressed, multi.size_unzipped() );

		if ( !bZip )
		{
			delete [] pDecompressed;

			g_pLogger->LogConsole("Unable to decompress buffer\n");
			return;
		}

		pMsgData = pDecompressed;
		cubMsgData = multi.size_unzipped();
		bDecomp = bZip;
	}
	else
	{
		pMsgData = (uint8 *)( multi.message_body().c_str() );
		cubMsgData = multi.message_body().length();
	}

	CBinaryReader reader( pMsgData, cubMsgData );

	while ( reader.GetSizeLeft() > 0 )
	{
		const uint32 cubPayload = reader.Read<uint32>();
		const uint8 *pPayload = reader.ReadBytes( cubPayload );

		this->LogNetMessage( eDirection, pPayload, cubPayload );
	}

	if ( bDecomp )
		delete [] pMsgData;
}
