#pragma once

#include <QWidget>
#include "ui_FFmpegPlayWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class FFmpegPlayWidget; };
QT_END_NAMESPACE

class FFmpegPlayWidget : public QWidget
{
    Q_OBJECT

public:
    FFmpegPlayWidget(QWidget *parent = nullptr);
    ~FFmpegPlayWidget();

    void SetUi();
    void SignalsAndSlots();

    void SetImage(const QImage& image);
    void SetPixmap(const QPixmap& pixmap);

private:
    Ui::FFmpegPlayWidget* ui;
};
