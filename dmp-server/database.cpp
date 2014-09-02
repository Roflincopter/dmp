#include "database.hpp"

#include "database_version.hpp"
#include "database_migration.hpp"
#include "index_list.hpp"

#include "odb/sqlite/database.hxx"

#include <boost/filesystem.hpp>

std::shared_ptr<odb::database> initialize_database()
{
	const std::string database_file = "dmp-server.db";
	const bool database_existed = boost::filesystem::exists(database_file);

	std::shared_ptr<odb::database> db = std::make_shared<odb::sqlite::database>("database.db", SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);

	{
		odb::transaction t(db->begin());
		if(!database_existed) {
			odb::schema_catalog::create_schema(*db);
		} else {
			odb::schema_version version = db->schema_version();
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
