#include "viewscape.h"
#include "ui_viewscape.h"
#include <gst/video/videooverlay.h>


ViewScape::ViewScape(WorkerGstreamer *workerGstreamer):  ui(new Ui::ViewScape), workerGstreamer(workerGstreamer)
{
    ui->setupUi(this);
    ui->soundSlider->setRange(0, 100);
    ui->videoWidget->setStyleSheet("background-color: #000000");
    ui->openFileButton->setStyleSheet("background-color: #207A9A;");
    ui->pathFileEdit->setStyleSheet("background-color: #F5FFFA");

    QPalette palette = this->palette();
    palette.setColor(QPalette::Background,  QColor("#FFFAFA"));
    this->setPalette(palette);

    buttonControl(false);

    playTimer = new QTimer();

    setIconButton(ui->pauseButton, "/home/linux/ViewScape/icon/pause.png");
    setIconButton(ui->stopButton, "/home/linux/ViewScape/icon/stop.png");
    setIconButton(ui->playButton, "/home/linux/ViewScape/icon/play.png");
    setIconButton(ui->expandButton, "/home/linux/ViewScape/icon/expand.png");
    setIconButton(ui->volumeButton, "/home/linux/ViewScape/icon/volume.png");

    workerGstreamer->setWinid(ui->videoWidget->winId());
    workerGstreamer->bindWindow();


    connect(ui->playButton, SIGNAL(clicked()), this, SLOT(clicedPlayButton()));
    connect(ui->stopButton, SIGNAL(clicked()), this, SLOT(clicedStopButton()));
    connect(ui->openFileButton, SIGNAL(clicked()), this, SLOT(clicedOpenFileButton()));
    connect(ui->pauseButton , SIGNAL(clicked()), this, SLOT(clicedPauseButton()));
    connect(ui->volumeButton , SIGNAL(clicked()), this, SLOT(clicedVolumeButton()));
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
    workerGstreamer->setVolume(ui->soundSlider->value());
}

void ViewScape::clicedStopButton()
{
    workerGstreamer->stopVideo();
}

void ViewScape::clicedPauseButton()
{
    workerGstreamer->pauseVideo();
}

void ViewScape::clicedVolumeButton()
{
    if (ui->soundSlider->value() != 0){
        oldVolumeValue = ui->soundSlider->value();
        workerGstreamer->setVolume(0);

        setIconButton(ui->volumeButton, "/home/linux/ViewScape/icon/volume-mute.png");
        ui->soundSlider->setValue(0);
    }
    else {
        ui->soundSlider->setValue(oldVolumeValue);
        workerGstreamer->setVolume(oldVolumeValue);
        setIconButton(ui->volumeButton, "/home/linux/ViewScape/icon/volume.png");
    }

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

void ViewScape::setIconButton(QPushButton *buntton, QString pathIcon)
{
    QIcon icon(pathIcon);
    buntton->setIcon(icon);
    buntton->setIconSize(QSize(32, 32));
}

void ViewScape::clicedOpenFileButton()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Выберите файл", "", "Видео файлы (*.mp4 *.webm *.avi)");

    if (!filePath.isEmpty()) {
        ui->pathFileEdit->setText(filePath);
        workerGstreamer->setFileSource(filePath);

        buttonControl(true);
    }
}
