
#include <stdint.h>
#include <functional>
#include <memory>

class Connection;
namespace boost { namespace asio { class io_service; } }

void accept_loop(uint16_t port, std::weak_ptr<boost::asio::io_service> ios, std::function<void(Connection&&)> f);

