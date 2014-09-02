#pragma once

#include <odb/database.hxx>
#include <odb/schema-catalog.hxx>

std::shared_ptr<odb::database> initialize_database();
