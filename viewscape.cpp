#include "viewscape.h"
#include "ui_viewscape.h"
#include <gst/video/videooverlay.h>


ViewScape::ViewScape(WorkerGstreamer *workerGstreamer)
    : workerGstreamer(workerGstreamer)
    , ui(new Ui::ViewScape)
{
    ui->setupUi(this);

    playTimer = new QTimer();

    workerGstreamer->setXwinid(ui->videoWidget->winId());
    workerGstreamer->bindWindow();

    connect(ui->playButton, SIGNAL(clicked()), this, SLOT(clicedPlayButton()));
    connect(ui->stopButton, SIGNAL(clicked()), this, SLOT(clicedStopButton()));
    connect(ui->pauseButton , SIGNAL(clicked()), this, SLOT(clicedPauseButton()));
    connect(playTimer, SIGNAL(timeout()), this, SLOT(sliderVideoUpdate()));
    connect(workerGstreamer, SIGNAL(updateState(GstState)), this, SLOT(stateUpdateHandler(GstState)));
}

ViewScape::~ViewScape()
{
    delete ui;
}

void ViewScape::clicedPlayButton(){
    workerGstreamer->playVideo();
}

void ViewScape::clicedStopButton()
{
    workerGstreamer->stopVideo();
}

void ViewScape::clicedPauseButton()
{
    workerGstreamer->pauseVideo();
}

void ViewScape::sliderVideoUpdate()
{
    ui->reproductionSlider->setValue(workerGstreamer->getCurrentTime());
    currentTime = QDateTime::fromTime_t(workerGstreamer->getCurrentTime()).toUTC().toString("mm:ss");

    ui->timeProgressLabel->setText(QString("%1 / %2").arg(currentTime).arg(totalDurationTime));

}

void ViewScape::stateUpdateHandler(GstState upState)
{
    if (upState == GST_STATE_PLAYING){
        ui->reproductionSlider->setRange(0, workerGstreamer->getTotalDuration());

        totalDurationTime = QDateTime::fromTime_t(workerGstreamer->getTotalDuration()).toUTC().toString("mm:ss");
        playTimer->start(1000);
    }
    if (upState < GST_STATE_PAUSED){
        playTimer->stop();
    }
}
