#ifndef VIEWSCAPE_H
#define VIEWSCAPE_H

#include <QMainWindow>
#include <QPushButton>
#include <gst/gst.h>
#include "workergstreamer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ViewScape; }
QT_END_NAMESPACE

class ViewScape : public QMainWindow
{
    Q_OBJECT

public:
    ViewScape(WorkerGstreamer *workerGstreamer);
    ~ViewScape();
    WId getVideoWId() const ;

private slots:
    void clicedPlayButton();
    void clicedStopButton();
    void clicedPauseButton();

private:
    Ui::ViewScape *ui;
    WorkerGstreamer *workerGstreamer;
};
#endif // VIEWSCAPE_H
