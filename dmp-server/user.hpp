#ifndef ODB_COMPILER
	#pragma once
#endif

#ifdef ODB_COMPILER
	#include "database_version.hpp"
#endif

#include <string>

namespace odb { class access; }

class User
{
	friend class odb::access;
	friend class SuperAdmin;
	friend class Admin;

	User();

	std::string username;

	std::string password;

public:
	User(std::string username, std::string passwd);

	std::string get_username(){return username;}
	std::string get_password(){return password;}
};

#ifdef ODB_COMPILER
	#pragma db object(User)
	#pragma db member(User::username) id
	#pragma db member(User::password)
#endif
