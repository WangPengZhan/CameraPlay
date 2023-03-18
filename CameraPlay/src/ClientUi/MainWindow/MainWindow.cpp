#include "MainWindow.h"
#include "ClientUi/FFmpegPlay/FFmpegPlayWidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindowClass())
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetImage(const QImage& image)
{
    ui->widget->SetImage(image);
}
