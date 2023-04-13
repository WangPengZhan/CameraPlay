#include <QtWidgets/QApplication>

#include "ClientUi/MainWindow/MainWindow.h"
#include "FFmpegCapture/FFmpegCaputre.h"
#include "ClientUi/FFmpegPlay/FFmpegPlayWidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    FFmpegCaputre ffmpegCapture;
    QObject::connect(&ffmpegCapture, &FFmpegCaputre::captureImage, &w, &MainWindow::SetImage);
    ffmpegCapture.SetFilename("D:/workfile/BilibiliDownloaders/x64/Debug/Download/BV19W4y1x71Sall.mp4");
    ffmpegCapture.Start();
    return a.exec();
}
