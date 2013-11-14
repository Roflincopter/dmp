#include "dmp_radio.hpp"

#include "gstreamer_util.hpp"

DmpRadio::DmpRadio(uint16_t from, uint16_t to)
{
    GMainLoop* loop;

    GstElement* pipeline,* source,* sink;
    GstBus *bus;
    guint bus_watch_id;

    gst_init(0, nullptr);

    loop = g_main_loop_new(nullptr, false);

    pipeline = gst_pipeline_new("tcp_bridge");
    source   = gst_element_factory_make("tcpserversrc", "recv");
    sink     = gst_element_factory_make("tcpserversink", "send");

    if (!pipeline || !source || !sink)
    {
        throw std::runtime_error("Could not create the pipeline components for this radio.");
    }

    g_object_set(G_OBJECT(source), "port", gint(from), nullptr);
    g_object_set(G_OBJECT(sink), "port", gint(to), nullptr);

    bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
    bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);
    gst_object_unref (bus);

    gst_bin_add_many (GST_BIN(pipeline), source, sink, nullptr);
    gst_element_link_many(source, sink, nullptr);

    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "dmp_radio");

    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    g_main_loop_run(loop);
}
