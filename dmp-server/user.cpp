#include "user.hpp"

User::User()
: username()
, password()
{}

User::User(std::string username, std::string passwd)
: username(username)
, password(passwd)
{}
