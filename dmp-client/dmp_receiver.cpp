#include "dmp_receiver.hpp"

#include "gstreamer_util.hpp"

#include <gst/gst.h>

#include <stdexcept>
#include <iostream>

DmpReceiver::DmpReceiver()
: host("")
, port(0)
{}

DmpReceiver::DmpReceiver(std::string host, uint16_t port)
: host(host)
, port(port)
{
    GMainLoop* loop;

    GstElement* pipeline,* source,* decoder,* converter,* resampler,* audiosink;
    GstBus *bus;
    guint bus_watch_id;

    gst_init(0, nullptr);

    loop = g_main_loop_new(nullptr, false);

    pipeline  = gst_pipeline_new("receiver");
    source    = gst_element_factory_make("tcpclientsrc", "stream");
    decoder   = gst_element_factory_make("decodebin", "decoder");
    converter = gst_element_factory_make("audioconvert", "converter");
    resampler = gst_element_factory_make("audioresample", "resampler");
    audiosink = gst_element_factory_make("autoaudiosink", "audiosink");


#define X(x) if(!x) std::cout << #x << " " << x << std::endl;
    if (!pipeline || !source || !decoder || !converter || !resampler || !audiosink)
    {
        X(pipeline);
        X(source);
        X(decoder);
        X(converter);
        X(resampler);
        X(audiosink);
        throw std::runtime_error("Could not create the pipeline components for this receiver.");
    }
#undef X

    g_object_set(G_OBJECT(source), "host", host.c_str(), nullptr);
    g_object_set(G_OBJECT(source), "port", gint(port), nullptr);

    bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
    bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);
    gst_object_unref(bus);

    gst_bin_add_many(GST_BIN(pipeline), source, decoder, converter, resampler, audiosink, nullptr);
    gst_element_link_many(source, decoder, nullptr);
    g_signal_connect(decoder, "pad-added", G_CALLBACK(on_pad_added), converter);
    gst_element_link_many(converter, resampler, audiosink, nullptr);

    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "dmp_receiver");

    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    g_main_loop_run(loop);
}
