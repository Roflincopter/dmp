#include "core_dump.hpp"

#ifdef __linux
void core_dump(int)
{
	kill(getpid(), SIGSEGV);
}
#endif
