#include "CMainWindow.h"

CMainWindow::CMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CMainWindow())
{
    ui->setupUi(this);
}

CMainWindow::~CMainWindow()
{
    delete ui;
}
