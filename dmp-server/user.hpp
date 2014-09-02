#ifndef ODB_COMPILER
	#pragma once
#endif

#include "database_version.hpp"

#include <odb/core.hxx>

#include <string>

class User
{
	friend class odb::access;

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
