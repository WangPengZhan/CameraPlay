#include <Windows.h>
#include <gdiplus.h>


#include "FFmpegPlayWidget.h"
#include "ui_FFmpegPlayWidget.h"

FFmpegPlayWidget::FFmpegPlayWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FFmpegPlayWidget())
{
    ui->setupUi(this);
}

FFmpegPlayWidget::~FFmpegPlayWidget()
{
    delete ui;
}

void FFmpegPlayWidget::SetUi()
{
}

void FFmpegPlayWidget::SignalsAndSlots()
{
    //connect();
}

void FFmpegPlayWidget::SetImage(const QImage& image)
{
    // const QImage showImage = image.scaled(ui->labelShow->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    SetPixmap(QPixmap::fromImage(image));

    auto dc = ::GetDC((HWND)ui->labelShow->winId());

}

void FFmpegPlayWidget::SetPixmap(const QPixmap& pixmap)
{
    QPixmap showPixmap = pixmap.scaled(ui->labelShow->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->labelShow->setPixmap(pixmap);
}
