
#include "RTSPReceiver.hpp"

int main(int argc, char** argv) {
    RTSPReceiver receiver("127.0.0.1", 6666, 6667, 320);
    receiver.start();
    return 0;
}
