#pragma once

#include <stdexcept>
#include <string>

struct InvalidClientEndpoint : public std::runtime_error {
	InvalidClientEndpoint(std::string what = "ClientEndpoint was invalid");
};
