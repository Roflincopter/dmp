#ifndef ODB_COMPILER
	#pragma once
#endif

#ifdef ODB_COMPILER
	#include "database_version.hpp"
#endif

#include <string>

namespace odb { class access; }

class Radio
{
	friend class odb::access;
	friend class SuperAdmin;
	friend class Admin;

	Radio();

	std::string name;
public:
	Radio(std::string name);
	
	std::string get_name();
};

#ifdef ODB_COMPILER
	#pragma db object(Radio)
	#pragma db member(Radio::name) id
#endif