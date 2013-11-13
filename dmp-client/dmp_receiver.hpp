#ifndef DMP_RECEIVER_HPP
#define DMP_RECEIVER_HPP

#include <cstdint>
#include <string>

class DmpReceiver
{
    std::string host;
    uint16_t port;

    void cleanup();
public:
    DmpReceiver();
    DmpReceiver(std::string host, uint16_t port);
};

#endif // DMP_RECEIVER_HPP
