#include "core_dump.hpp"

#ifdef __linux
#include <signal.h>
#include <unistd.h>
void core_dump(int)
{
	kill(getpid(), SIGSEGV);
}
#endif

void symbol(){}
