#pragma once

#include <stdexcept>

struct InvalidClientEndpoint : public std::runtime_error {
	InvalidClientEndpoint(std::string what = "ClientEndpoint was invalid");
};
