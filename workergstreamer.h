#ifndef WORKERGSTREAMER_H
#define WORKERGSTREAMER_H
#include <gst/gst.h>
#include <QApplication>

#include <QObject>
#include <QTime>

class WorkerGstreamer: public QObject
{
   Q_OBJECT
public:
    explicit WorkerGstreamer(QObject *parent = 0);
    void playVideo();
    void stopVideo();
    void pauseVideo();
    void bindWindow();
    gint64 getTotalDuration();
    gint64 getCurrentTime();
    void setXwinid(WId xwinid);
    static gboolean messageHandler(GstBus * bus, GstMessage * message, gpointer user_data);
    void setBusMonitoring();

signals:
  void updateState(GstState upState);

private:
    GstElement *pipeline;
    void activationEos();
    WId xwinid;

    GstState currentState;
    gint64 totalDuration = GST_CLOCK_TIME_NONE;
    gint64 currentTime = GST_CLOCK_TIME_NONE;
};

#endif // WORKERGSTREAMER_H
