#include "../PCH.hpp"

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID /*reserved*/)
{
	switch (reason)
	{
		case DLL_PROCESS_ATTACH:
			return DisableThreadLibraryCalls(instance);
	}

	return TRUE;
}