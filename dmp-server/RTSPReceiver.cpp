#include "RTSPReceiver.hpp"

UsageEnvironment* RTSPReceiver::env = nullptr;
RTSPReceiver::sessionState RTSPReceiver::session_state;

RTSPReceiver::RTSPReceiver(std::string session_address, uint16_t rtp_port, uint16_t rtpc_port, unsigned int estimated_session_bandwidth)
{
    // Begin by setting up our usage environment:
    TaskScheduler* scheduler = BasicTaskScheduler::createNew();
    env = BasicUsageEnvironment::createNew(*scheduler);

    // Create the data sink for 'stdout':
    session_state.sink = FileSink::createNew(*env, "stdout");
    // Note: The string "stdout" is handled as a special case.
    // A real file name could have been used instead.

    // Create 'groupsocks' for RTP and RTCP:

    struct in_addr sessionAddress;
    sessionAddress.s_addr = our_inet_addr(session_address.c_str());
    const Port rtpPort(rtp_port);
    const Port rtcpPort(rtpc_port);

    const unsigned char ttl = 1;
    rtpGroupsock = new Groupsock(*env, sessionAddress, rtpPort, ttl);
    rtcpGroupsock = new Groupsock(*env, sessionAddress, rtcpPort, ttl);

    RTPSource* rtpSource;
    // Create the data source: a "MP3 *ADU* RTP source"
    unsigned char rtpPayloadFormat = 96; // a dynamic payload type
    rtpSource = MP3ADURTPSource::createNew(*env, rtpGroupsock, rtpPayloadFormat);

    // Create (and start) a 'RTCP instance' for the RTP source:
    const unsigned maxCNAMElen = 100;
    unsigned char CNAME[maxCNAMElen+1];
    gethostname((char*)CNAME, maxCNAMElen);
    CNAME[maxCNAMElen] = '\0'; // just in case

    session_state.rtcpInstance = RTCPInstance::createNew(*env, rtcpGroupsock, estimated_session_bandwidth, CNAME, NULL, rtpSource);
    // Note: This starts RTCP running automatically

    session_state.source = rtpSource;

    // Add a filter that deinterleaves the ADUs after depacketizing them:
    session_state.source
      = MP3ADUdeinterleaver::createNew(*env, session_state.source);
    if (session_state.source == NULL) {
      *env << "Unable to create an ADU deinterleaving filter for the source\n";
      exit(1);
    }

    // Add another filter that converts these ADUs to MP3s:
    session_state.source
      = MP3FromADUSource::createNew(*env, session_state.source);
    if (session_state.source == NULL) {
      *env << "Unable to create an ADU->MP3 filter for the source\n";
      exit(1);
    }
}

void RTSPReceiver::start()
{
    // Finally, start receiving the multicast stream:
    *env << "Beginning receiving multicast stream...\n";
    session_state.sink->startPlaying(*session_state.source, afterPlaying, NULL);

    env->taskScheduler().doEventLoop(); // does not return
    return;
}
