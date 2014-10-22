#include "radio.hpp"

Radio::Radio()
{}

Radio::Radio(std::string name)
: name(name)
{}

std::string Radio::get_name()
{
	return name;
}
