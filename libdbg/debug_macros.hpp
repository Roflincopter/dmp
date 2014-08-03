#pragma once

#include "time_util.hpp"

#include <iostream>

#define DEBUG_COUT std::cout << get_current_time() << " " << __FILE__ << ":" << __LINE__ << "(" << __FUNCTION__ << "): "
#define DEBUG_CERR std::cerr << get_current_time() << " " << __FILE__ << ":" << __LINE__ << "(" << __FUNCTION__ << "): "
