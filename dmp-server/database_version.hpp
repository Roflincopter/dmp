#ifndef ODB_COMPILER
	#pragma once
#endif

#include <odb/core.hxx>

constexpr odb::schema_version current_database_version = 1;

#ifdef ODB_COMPILER
	#pragma db model version(1, 1)
#endif
