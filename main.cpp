#include "viewscape.h"
#include <gst/video/videooverlay.h>
#include <QApplication>
#include "workergstreamer.h"

int main(int argc, char *argv[])
{

    gst_init (&argc, &argv);
    QApplication a(argc, argv);


    WorkerGstreamer *workerGstreamer = new WorkerGstreamer();
    ViewScape *window = new ViewScape(workerGstreamer);

    window->show();


   return a.exec();
}
