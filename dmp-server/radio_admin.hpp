#ifndef ODB_COMPILER
	#pragma once
#endif

#include "user.hpp"
#include "radio.hpp"

#include "database_version.hpp"

#include <odb/core.hxx>

#include <memory>

class SuperAdmin
{
	friend class odb::access;

	uint32_t id;

	std::shared_ptr<User> user;
	std::shared_ptr<Radio> radio;

public:
	SuperAdmin();
};

#ifdef ODB_COMPILER
	#pragma db object(SuperAdmin)
	#pragma db member(SuperAdmin::id) id auto
	#pragma db member(SuperAdmin::user) not_null
	#pragma db member(SuperAdmin::radio) not_null
#endif

class Admin
{
	friend class odb::access;

	uint32_t id;

	std::shared_ptr<User> user;
	std::shared_ptr<Radio> radio;

public:
	Admin();
};

#ifdef ODB_COMPILER
	#pragma db object(Admin)
	#pragma db member(Admin::id) id auto
	#pragma db member(Admin::user) not_null
	#pragma db member(Admin::radio) not_null
#endif
