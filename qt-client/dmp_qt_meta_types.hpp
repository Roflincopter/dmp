#pragma once

#include "dmp-library.hpp"

#include <QMetaType>

#include <string>

Q_DECLARE_METATYPE(std::string)
Q_DECLARE_METATYPE(dmp_library::LibraryEntry::Duration)