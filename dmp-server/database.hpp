#pragma once

#include <memory>

namespace odb { class database; }

std::shared_ptr<odb::database> initialize_database();
