#include "core_dump.hpp"

void core_dump(int)
{
	kill(getpid(), SIGSEGV);
}
