#pragma once

#ifdef __linux
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
 
void core_dump(int = -1);
#endif

void symbol();
