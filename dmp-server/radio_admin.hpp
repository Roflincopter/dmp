#ifndef ODB_COMPILER
	#pragma once
#endif

#include "user.hpp"
#include "radio.hpp"

#include <memory>
#include <stdint.h>

namespace odb { class access; }

class SuperAdmin
{
	friend class odb::access;

	SuperAdmin();

	uint32_t
#if !defined( RADIO_ADMIN_ODB_HPP ) && defined( __clang__ )
		__attribute((unused))
#endif
	id;

	std::shared_ptr<User> user;
	std::shared_ptr<Radio> radio;

public:
	SuperAdmin(std::shared_ptr<User> user, std::shared_ptr<Radio> radio);
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

	Admin();

	uint32_t
#if !defined( RADIO_ADMIN_ODB_HPP ) && defined( __clang__ )
	__attribute((unused))
#endif
	id;

	std::shared_ptr<User> user;
	std::shared_ptr<Radio> radio;

public:
	Admin(std::shared_ptr<User> user, std::shared_ptr<Radio> radio);
};

#ifdef ODB_COMPILER
	#pragma db object(Admin)
	#pragma db member(Admin::id) id auto
	#pragma db member(Admin::user) not_null
	#pragma db member(Admin::radio) not_null
#endif
