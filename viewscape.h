#ifndef VIEWSCAPE_H
#define VIEWSCAPE_H

#include <QMainWindow>
#include <QPushButton>
#include <QTimer>
#include <QFileDialog>


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
    void clicedOpenFileButton();
    void clicedStopButton();
    void clicedPauseButton();
    void clicedVolumeButton();
    void sliderVideoUpdate();
    void stateUpdateHandler(GstState upState);
    void volumeReleasedSlider();
    void reproductionReleasedSlider();
    void reproductionPressedSlider();


private:
    Ui::ViewScape *ui;
    WorkerGstreamer *workerGstreamer;
    QTimer *playTimer;
    QString totalDurationTime;
    QString currentTime;
    qint32 oldVolumeValue;
    void buttonControlState(bool enabledState);
    void setIconButton(QPushButton *buntton, QString pathIcon);
};
#endif // VIEWSCAPE_H
