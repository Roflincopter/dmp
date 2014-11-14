#include "server_exceptions.hpp"

InvalidClientEndpoint::InvalidClientEndpoint(std::string what)
: std::runtime_error(what)
{}
