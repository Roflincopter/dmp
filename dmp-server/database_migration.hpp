#pragma once

#include "index_list.hpp"

//base migration case, do nothing.
template<odb::schema_version v>
struct migration
{
	static void call(std::shared_ptr<odb::database>){}
};

template<int... Indices>
void do_migration_for(odb::schema_version v, indices<Indices...>, std::shared_ptr<odb::database> db)
{
	static std::function<void(std::shared_ptr<odb::database>)> f_array[]{
		migration<Indices>::call
		...
	};

	f_array[v](db);
}

void do_migration_for(odb::schema_version v, std::shared_ptr<odb::database> db)
{
	do_migration_for(v, build_indices<current_database_version>::type{}, db);
}

/* define migration as follows

template<>
struct migration<version>
{
	static void call(std::shared_ptr<odb::database> db) {
		migration goes here.
	}
};
*/
