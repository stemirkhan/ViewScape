#include "workergstreamer.h"
#include <gst/video/videooverlay.h>

WorkerGstreamer::WorkerGstreamer()
{
    pipeline = gst_parse_launch ("playbin uri=https://test-videos.co.uk/vids/bigbuckbunny/webm/vp9/360/Big_Buck_Bunny_360_10s_1MB.webm", NULL);

    GstBus *bus = gst_element_get_bus(pipeline);
    gst_bus_add_watch(bus, &WorkerGstreamer::messageHandler, this);
    gst_object_unref(bus);
}

void WorkerGstreamer::playVideo()
{
    GstState st = GST_STATE_NULL;
        gst_element_get_state (pipeline, &st, NULL, GST_CLOCK_TIME_NONE);
        if (st < GST_STATE_PAUSED) {
            bindWindow();
        }
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
}

void WorkerGstreamer::stopVideo()
{
    gst_element_set_state (pipeline, GST_STATE_READY);
}

void WorkerGstreamer::pauseVideo()
{
    gst_element_set_state (pipeline, GST_STATE_PAUSED);
}

void WorkerGstreamer::bindWindow()
{
    GstElement *vsink = gst_element_factory_make ("ximagesink", "vsink");
    gst_video_overlay_set_window_handle (GST_VIDEO_OVERLAY (vsink), xwinid);
    g_object_set(GST_OBJECT(pipeline), "video-sink", vsink, NULL);
}

void WorkerGstreamer::setXwinid(WId xwinid)
{
    this->xwinid = xwinid;
}


gboolean WorkerGstreamer::messageHandler(GstBus * bus, GstMessage * message, gpointer user_data){
    WorkerGstreamer *management = reinterpret_cast<WorkerGstreamer*>(user_data);

    switch (GST_MESSAGE_TYPE(message)) {

    case GST_MESSAGE_STATE_CHANGED: {
        GstState old_state, new_state, pending_state;
        gst_message_parse_state_changed (message, &old_state, &new_state, &pending_state);
        break;
    }

    case GST_MESSAGE_ERROR: {
        GError *err;
        gchar *debug_info;

        gst_message_parse_error (message, &err, &debug_info);
        g_clear_error (&err);
        g_free (debug_info);
        break;
    }

    case GST_MESSAGE_EOS: {
        gst_element_set_state (management->pipeline, GST_STATE_NULL);
        break;
    }

    default:
        break;
    }


    return TRUE;
}
