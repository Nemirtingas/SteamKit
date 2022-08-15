
#include "csimpledetour.h"
#include "logger.h"

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
	void* r = hook.hook_func(*m_fnOld, m_fnReplacement);
	if (r != nullptr)
	{
		*m_fnOld = r;
		m_bAttached = true;

        g_pLogger->LogConsole("Hooked\n");
	}
    else
    {
        g_pLogger->LogConsole("Failed to hook\n");
    }
}

void CSimpleDetour::Detach() noexcept
{
	if (!m_bAttached)
		return;

	*m_fnOld = hook.restore_func();
}

#endif
