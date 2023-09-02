#include "viewscape.h"
#include "ui_viewscape.h"
#include <gst/video/videooverlay.h>


ViewScape::ViewScape(WorkerGstreamer *workerGstreamer)
    : workerGstreamer(workerGstreamer)
    , ui(new Ui::ViewScape)
{
    ui->setupUi(this);

    workerGstreamer->setXwinid(ui->videoWidget->winId());
    workerGstreamer->bindWindow();

    connect(ui->playButton, SIGNAL(clicked()), this, SLOT(clicedPlayButton()));
    connect(ui->stopButton, SIGNAL(clicked()), this, SLOT(clicedStopButton()));
    connect(ui->pauseButton , SIGNAL(clicked()), this, SLOT(clicedPauseButton()));
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
