
#include "RTSPSender.hpp"

UsageEnvironment* RTSPSender::env = nullptr;
RTSPSender::SessionState RTSPSender::session_state;

RTSPSender::RTSPSender(std::string address, uint16_t rtp_port, uint16_t rtpc_port, unsigned int estimated_bandwith)
{
    // Begin by setting up our usage environment:
    TaskScheduler* scheduler = BasicTaskScheduler::createNew();
    env = BasicUsageEnvironment::createNew(*scheduler);

    // Create 'groupsocks' for RTP and RTCP:
    struct in_addr destinationAddress;
    destinationAddress.s_addr = our_inet_addr(address.c_str());
    const Port rtpPort(rtp_port);
    const Port rtcpPort(rtpc_port);

    const unsigned char ttl = 1;
    session_state.rtpGroupsock = new Groupsock(*env, destinationAddress, rtpPort, ttl);
    session_state.rtcpGroupsock = new Groupsock(*env, destinationAddress, rtcpPort, ttl);

    // Create a 'MP3 RTP' sink from the RTP 'groupsock':

    unsigned char rtpPayloadFormat = 96; // A dynamic payload format code
    session_state.sink = MP3ADURTPSink::createNew(*env, session_state.rtpGroupsock, rtpPayloadFormat);

    // Create (and start) a 'RTCP instance' for this RTP sink:
    const unsigned maxCNAMElen = 100;
    unsigned char CNAME[maxCNAMElen+1];
    gethostname((char*)CNAME, maxCNAMElen);
    CNAME[maxCNAMElen] = '\0'; // just in case
    session_state.rtcpInstance = RTCPInstance::createNew(*env, session_state.rtcpGroupsock, estimated_bandwith, CNAME, session_state.sink, NULL);
    // Note: This starts RTCP running automatically
}

void RTSPSender::play(std::string filename)
{
    // Open the file as a 'MP3 file source':
    session_state.source = MP3FileSource::createNew(*env, filename.c_str());
    if (session_state.source == NULL) {
        *env << "Unable to open file \"" << filename.c_str() << "\" as a MP3 file source\n";
        exit(1);
    }

    // Add a filter that converts the source MP3s to ADUs:
    session_state.source = ADUFromMP3Source::createNew(*env, session_state.source);
    if (session_state.source == NULL) {
        *env << "Unable to create a MP3->ADU filter for the source\n";
        exit(1);
    }

    // Add another filter that interleaves the ADUs before packetizing them:
    unsigned char interleaveCycle[] = {0,2,1,3}; // or choose your own order...
    unsigned const interleaveCycleSize = (sizeof interleaveCycle)/(sizeof (unsigned char));
    Interleaving interleaving(interleaveCycleSize, interleaveCycle);
    session_state.source = MP3ADUinterleaver::createNew(*env, interleaving, session_state.source);
    if (session_state.source == NULL) {
        *env << "Unable to create an ADU interleaving filter for the source\n";
        exit(1);
    }

    // Finally, start the streaming:
    *env << "Beginning streaming...\n";
    session_state.sink->startPlaying(*session_state.source, afterPlaying, NULL);
}

void RTSPSender::start(std::string filename)
{
    play(filename);
    env->taskScheduler().doEventLoop(); // does not return
}
