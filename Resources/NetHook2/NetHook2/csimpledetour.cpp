
#include "csimpledetour.h"


CSimpleDetour::CSimpleDetour(void **old, void *replacement) noexcept
{
	m_fnOld = old;
	m_fnReplacement = replacement;
	m_bAttached = false;
}

#if defined(NETHOOK2_OS_WINDOWS)

void CSimpleDetour::Attach() noexcept
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourAttach(m_fnOld, m_fnReplacement);

	DetourTransactionCommit();
	
	m_bAttached = true;
}

void CSimpleDetour::Detach() noexcept
{
	if (!m_bAttached)
		return;

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourDetach(m_fnOld, m_fnReplacement);

	DetourTransactionCommit();
}

#elif defined(NETHOOK2_OS_LINUX)

void CSimpleDetour::Attach() noexcept
{
	m_bAttached = true;
}

void CSimpleDetour::Detach() noexcept
{
	if (!m_bAttached)
		return;

}

#endif
