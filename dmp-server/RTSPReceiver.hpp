#pragma once

#include "liveMedia.hh"
#include "GroupsockHelper.hh"

#include "BasicUsageEnvironment.hh"

#include <string>
#include <cstdint>

class RTSPReceiver
{
private:
    struct sessionState {
      FramedSource* source;
      FileSink* sink;
      RTCPInstance* rtcpInstance;
    };

    static UsageEnvironment* env;
    static sessionState session_state;

    Groupsock* rtpGroupsock;
    Groupsock* rtcpGroupsock;

    static void afterPlaying(void*)
    {
        *env << "...done receiving\n";

        // End by closing the media:
        Medium::close(session_state.rtcpInstance); // Note: Sends a RTCP BYE
        Medium::close(session_state.sink);
        Medium::close(session_state.source);
    }

public:
    RTSPReceiver(std::string session_address, uint16_t rtp_port, uint16_t rtpc_port, unsigned int estimated_session_bandwidth);
    ~RTSPReceiver()
    {
        delete rtpGroupsock;
        delete rtcpGroupsock;
    }

    void start();
};
