#ifndef ODB_COMPILER
	#pragma once
#endif

#include "database_version.hpp"

#include <odb/core.hxx>

#include <string>

class SuperAdmin;
class Admin;

class Radio
{
	friend class odb::access;
	friend class SuperAdmin;
	friend class Admin;

	Radio();

	std::string name;	
public:
	
};

#ifdef ODB_COMPILER
	#pragma db object(Radio)
	#pragma db member(Radio::name) id
#endif