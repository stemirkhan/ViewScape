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
    ~WorkerGstreamer();
    void playVideo();
    void stopVideo();
    void pauseVideo();
    void bindWindow();
    gint64 getTotalDuration();
    gint64 getCurrentTime();
    void setVolume(gdouble volume);
    void setReproduction(gint64 position);
    void setFileSource(QString fileName);
    void setWinid(WId xwinid);
    static gboolean messageHandler(GstBus * bus, GstMessage * message, gpointer user_data);
    void setBusMonitoring();

signals:
  void updateState(GstState upState);
  void errorGstreamer(QString textError);

private:
  GstElement *pipeline, *videoSink;
  GstBus *bus;
  GError* error = NULL;
  gchar* debug_info = NULL;
  void activationEos();
  WId xwinid;
  GstState currentState;
  gint64 totalDuration = GST_CLOCK_TIME_NONE;
  gint64 currentTime = GST_CLOCK_TIME_NONE;
};

#endif // WORKERGSTREAMER_H
