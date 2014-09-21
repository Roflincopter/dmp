#include "database.hpp"

#include "database_version.hpp"
#include "database_migration.hpp"
#include "index_list.hpp"
#include "debug_macros.hpp"
#include "user-odb.hpp"

#include "odb/sqlite/database.hxx"

#include <boost/filesystem.hpp>

std::shared_ptr<odb::database> initialize_database()
{
	const std::string database_file = "dmp-server.db";
	const bool database_existed = boost::filesystem::exists(database_file);

	std::shared_ptr<odb::database> db = std::make_shared<odb::sqlite::database>(
		database_file,
		SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
	);

	{
		odb::transaction t(db->begin());
		if(!database_existed) {
			DEBUG_COUT << "Creating Schema" << std::endl;
			odb::schema_catalog::create_schema(*db);
			User x("ex_username", "ex_password");
			db->persist(x);
		} else {
			DEBUG_COUT << "Migrating" << std::endl;
			odb::schema_version version = db->schema_version();
			if(version < database_base_version) {
				throw std::runtime_error("Sorry, the database is to old to migrate to the current required version. Please delete the old database: \"" + database_file + "\" and let the application create a new one");
			}
			for(odb::schema_version i = version; i < current_database_version; ++i) {
				odb::schema_catalog::migrate_schema_pre(*db, i+1);
				do_migration_for(i+i, db);
				odb::schema_catalog::migrate_schema_post(*db, i+1);
			}
		}
		t.commit();
	}
	return db;
}