#include "viewscape.h"
#include "ui_viewscape.h"
#include <gst/video/videooverlay.h>


ViewScape::ViewScape(WorkerGstreamer *workerGstreamer)
    : workerGstreamer(workerGstreamer)
    , ui(new Ui::ViewScape)
{
    ui->setupUi(this);
    ui->soundSlider->setRange(0, 100);
    ui->videoWidget->setStyleSheet("border: 2px solid gren;");

    buttonControl(false);

    playTimer = new QTimer();

    workerGstreamer->setWinid(ui->videoWidget->winId());
    workerGstreamer->bindWindow();
    workerGstreamer->setVolume(ui->soundSlider->value());

    connect(ui->playButton, SIGNAL(clicked()), this, SLOT(clicedPlayButton()));
    connect(ui->stopButton, SIGNAL(clicked()), this, SLOT(clicedStopButton()));
    connect(ui->openFileButton, SIGNAL(clicked()), this, SLOT(clicedOpenFileButton()));
    connect(ui->pauseButton , SIGNAL(clicked()), this, SLOT(clicedPauseButton()));
    connect(ui->soundSlider , SIGNAL(sliderReleased()), this, SLOT(soundReleasedSlider()));
    connect(ui->reproductionSlider , SIGNAL(sliderReleased()), this, SLOT(reproductionReleasedSlider()));
    connect(ui->reproductionSlider , SIGNAL(sliderPressed()), this, SLOT(reproductionPressedSlider()));
    connect(playTimer, SIGNAL(timeout()), this, SLOT(sliderVideoUpdate()));
    connect(workerGstreamer, SIGNAL(updateState(GstState)), this, SLOT(stateUpdateHandler(GstState)));
}

ViewScape::~ViewScape()
{
    delete ui;
    delete workerGstreamer;
    delete playTimer;
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
        playTimer->start(10);
    }
    if (upState < GST_STATE_PAUSED){
        playTimer->stop();
    }
}

void ViewScape::reproductionPressedSlider()
{
    playTimer->stop();
}

void ViewScape::soundReleasedSlider()
{
    workerGstreamer->setVolume(ui->soundSlider->value());
}

void ViewScape::reproductionReleasedSlider()
{
    workerGstreamer->setReproduction(ui->reproductionSlider->value());
    playTimer->start(10);
}

void ViewScape::buttonControl(bool enabledState)
{
    ui->playButton->setEnabled(enabledState);
    ui->pauseButton->setEnabled(enabledState);
    ui->stopButton->setEnabled(enabledState);
    ui->reproductionSlider->setEnabled(enabledState);
}

void ViewScape::clicedOpenFileButton()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Выберите файл", "", "Видео файлы (*.mkv *.webm *.avi)");

    if (!filePath.isEmpty()) {
        ui->pathFileEdit->setText(filePath);
        workerGstreamer->setFileSource(filePath);

        buttonControl(true);
    }
}
