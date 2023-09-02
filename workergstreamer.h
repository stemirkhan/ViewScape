#ifndef WORKERGSTREAMER_H
#define WORKERGSTREAMER_H
#include <gst/gst.h>
#include <QApplication>

class WorkerGstreamer
{
public:
    WorkerGstreamer();
    void playVideo();
    void stopVideo();
    void pauseVideo();
    void bindWindow();
    void setXwinid(WId xwinid);
    static gboolean messageHandler(GstBus * bus, GstMessage * message, gpointer user_data);
    void setBusMonitoring();

private:
    GstElement *pipeline;
    void activationEos();
    WId xwinid;
};

#endif // WORKERGSTREAMER_H
