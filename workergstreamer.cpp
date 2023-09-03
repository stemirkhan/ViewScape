#include "workergstreamer.h"
#include <gst/video/videooverlay.h>

WorkerGstreamer::WorkerGstreamer(QObject *parent): QObject(parent)
{
    pipeline = gst_parse_launch("playbin", NULL);
    videoSink = gst_element_factory_make ("ximagesink", "vsink");

    GstBus *bus = gst_element_get_bus(pipeline);
    gst_bus_add_watch(bus, &WorkerGstreamer::messageHandler, this);
    gst_object_unref(bus);
}

WorkerGstreamer::~WorkerGstreamer()
{
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    gst_object_unref(videoSink);

    g_error_free(error);
    g_free(debug_info);
}

void WorkerGstreamer::playVideo()
{
    GstState state = GST_STATE_NULL;
    gst_element_get_state(pipeline, &state, NULL, GST_CLOCK_TIME_NONE);

    if(state < GST_STATE_PAUSED){
        gst_element_set_state(pipeline, GST_STATE_NULL);
        bindWindow();
    }
    if(state != GST_STATE_PLAYING){
        gst_element_set_state(pipeline, GST_STATE_PLAYING);
    }
}

void WorkerGstreamer::stopVideo()
{
    GstState state = GST_STATE_NULL;
    gst_element_get_state(pipeline, &state, NULL, GST_CLOCK_TIME_NONE);

    if(state == GST_STATE_PLAYING || state == GST_STATE_PAUSED) {
        gst_element_set_state(pipeline, GST_STATE_READY);
    }
}

void WorkerGstreamer::pauseVideo()
{
    GstState state = GST_STATE_NULL;
    gst_element_get_state(pipeline, &state, NULL, GST_CLOCK_TIME_NONE);

    if(state == GST_STATE_PLAYING) {
        gst_element_set_state(pipeline, GST_STATE_PAUSED);
    }

    if(state == GST_STATE_PAUSED) {
        gst_element_set_state(pipeline, GST_STATE_PLAYING);
    }
}

void WorkerGstreamer::bindWindow()
{
    gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(videoSink), xwinid);
    g_object_set(GST_OBJECT(pipeline), "video-sink", videoSink, NULL);
}

gint64 WorkerGstreamer::getTotalDuration()
{
    if (!GST_CLOCK_TIME_IS_VALID(totalDuration)){
        gst_element_query_duration(pipeline, GST_FORMAT_TIME, &totalDuration);
    }

    return totalDuration/GST_SECOND;
}

gint64 WorkerGstreamer::getCurrentTime()
{
    gst_element_query_position(pipeline, GST_FORMAT_TIME, &currentTime);
    return currentTime/GST_SECOND;
}

void WorkerGstreamer::setVolume(gdouble sound_volume)
{
    GstState state = GST_STATE_NULL;
    gst_element_get_state(pipeline, &state, NULL, GST_CLOCK_TIME_NONE);

    if(state == GST_STATE_PLAYING) {
        g_object_set(GST_OBJECT(pipeline), "volume", (double)sound_volume/100, NULL);
    }
}

void WorkerGstreamer::setReproduction(gint64 position)
{
    GstState state = GST_STATE_NULL;
    gst_element_get_state(pipeline, &state, NULL, GST_CLOCK_TIME_NONE);

    if(state == GST_STATE_PLAYING) {
        gst_element_seek_simple(pipeline, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH, position * GST_SECOND);
    }

}

void WorkerGstreamer::setFileSource(QString fileName)
{
    QString allPath = "file://" + fileName;
    const char *allPathC = allPath.toUtf8().constData();
    g_object_set(GST_OBJECT(pipeline), "uri", allPathC, NULL);
}

void WorkerGstreamer::setWinid(WId xwinid)
{
    this->xwinid = xwinid;
}


gboolean WorkerGstreamer::messageHandler(GstBus * bus, GstMessage * message, gpointer user_data){
    WorkerGstreamer *management = reinterpret_cast<WorkerGstreamer*>(user_data);

    switch (GST_MESSAGE_TYPE(message)) {

    case GST_MESSAGE_STATE_CHANGED: {
        GstState old_state, new_state, pending_state;
        gst_message_parse_state_changed (message, &old_state, &new_state, &pending_state);
        management->updateState(new_state);
        break;
    }

    case GST_MESSAGE_ERROR: {
        gst_message_parse_error(message, &management->error, &management->debug_info);

        g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(message->src), management->error->message);
        g_printerr("Debugging information: %s\n", (management->debug_info) ? management->debug_info : "none");

        g_error_free(management->error);
        g_free(management->debug_info);
        break;
    }

    case GST_MESSAGE_EOS: {
        gst_element_set_state(management->pipeline, GST_STATE_NULL);
        break;
    }

    default:
        break;
    }

    return TRUE;
}
