#include "radio_admin.hpp"

SuperAdmin::SuperAdmin()
{}

SuperAdmin::SuperAdmin(std::shared_ptr<User> user, std::shared_ptr<Radio> radio)
: user(user)
, radio(radio)
{}

Admin::Admin()
{}

Admin::Admin(std::shared_ptr<User> user, std::shared_ptr<Radio> radio)
: user(user)
, radio(radio)
{}
