
#include "RTSPSender.hpp"

int main(int argc, char** argv) {
    RTSPSender sender("127.0.0.1", 6666, 6667, 320);
    sender.start("test.mp3");
    return 0;
}
