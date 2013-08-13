#pragma once

#include "liveMedia.hh"
#include "GroupsockHelper.hh"
#include "BasicUsageEnvironment.hh"

#include <string>
#include <cstdint>

class RTSPSender {
private:
    struct SessionState {
      FramedSource* source;
      RTPSink* sink;
      RTCPInstance* rtcpInstance;
      Groupsock* rtpGroupsock;
      Groupsock* rtcpGroupsock;
    };

    static SessionState session_state;
    static UsageEnvironment* env;

    static void afterPlaying(void*)
    {
        *env << "...done streaming\n";

        session_state.sink->stopPlaying();

        // End this loop by closing the current source:
        Medium::close(session_state.source);
    }

public:
    RTSPSender(std::string address, uint16_t rtp_port, uint16_t rtpc_port, unsigned int estimated_bandwith);

    void play(std::string filename);
    void start(std::string filename);
};
