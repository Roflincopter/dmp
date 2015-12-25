#pragma once

#include <odb/forward.hxx>

#define DATABASE_BASE_VERSION 1
#define DATABASE_VERSION 1

constexpr odb::schema_version database_base_version = DATABASE_BASE_VERSION;
constexpr odb::schema_version current_database_version = DATABASE_VERSION;

#ifdef ODB_COMPILER
	#pragma db model version(DATABASE_BASE_VERSION, DATABASE_VERSION, open)
#endif

#undef DATABASE_BASE_VERSION
#undef DATABASE_VERSION
