#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_CMainWindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class CMainWindow; };
QT_END_NAMESPACE

class CMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    CMainWindow(QWidget *parent = nullptr);
    ~CMainWindow();

private:
    Ui::CMainWindow*ui;
};
