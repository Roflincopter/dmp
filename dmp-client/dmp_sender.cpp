#include "dmp_sender.hpp"

#include <gst/gst.h>

#include <stdexcept>
#include <iostream>

DmpSender::DmpSender()
: host("")
, port(0)
{}

static gboolean
bus_call (GstBus     *bus,
          GstMessage *msg,
          gpointer    data)
{
  GMainLoop *loop = (GMainLoop *) data;

  switch (GST_MESSAGE_TYPE (msg)) {

    case GST_MESSAGE_EOS:
      g_print ("End of stream\n");
      g_main_loop_quit (loop);
      break;

    case GST_MESSAGE_ERROR: {
      gchar  *debug;
      GError *error;

      gst_message_parse_error (msg, &error, &debug);
      g_free (debug);

      g_printerr ("Error: %s\n", error->message);
      g_error_free (error);

      g_main_loop_quit (loop);
      break;
    }
    default:
      break;
  }

  return true;
}

DmpSender::DmpSender(std::string host, uint16_t port, std::string file)
: host(host)
, port(port)
{
    GMainLoop* loop;

    GstElement* pipeline,* source,* decoder,* encoder,* parser,* pay, * sink;
    GstBus *bus;
    guint bus_watch_id;

    gst_init(0, nullptr);

    loop = g_main_loop_new(nullptr, false);

    pipeline = gst_pipeline_new("sender");
    source   = gst_element_factory_make("filesrc", "file");
    decoder  = gst_element_factory_make("decodebin", "decoder");
    encoder  = gst_element_factory_make("lamemp3enc", "encoder");
    parser   = gst_element_factory_make("mpegaudioparse", "parser");
    pay      = gst_element_factory_make("rtpmpapay", "pay");
    sink     = gst_element_factory_make("tcpclientsink", "send");

#define X(x) if(!x) std::cout << #x << " " << x << std::endl;
    if (!pipeline || !source || !decoder || !encoder || !parser || !pay || !sink)
    {
        X(pipeline);
        X(source);
        X(decoder);
        X(encoder);
        X(parser);
        X(pay);
        X(sink)
        throw std::runtime_error("Could not create the pipeline components for this sender.");
    }
#undef X

    g_object_set(G_OBJECT(source), "location", file.c_str(), nullptr);

    g_object_set(G_OBJECT(encoder), "bitrate", gint(320), nullptr);
    g_object_set(G_OBJECT(encoder), "cbr", gboolean(true), nullptr);

    g_object_set(G_OBJECT(sink), "host", host.c_str(), nullptr);
    g_object_set(G_OBJECT(sink), "port", gint(port), nullptr);

    bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
    bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);
    gst_object_unref (bus);

    gst_bin_add_many (GST_BIN(pipeline), source, sink, nullptr);
    gst_element_link_many(source, sink, nullptr);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    g_main_loop_run(loop);
}
