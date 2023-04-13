#pragma once

#include <QTableView>

class CCustomView  : public QTableView
{
    Q_OBJECT

public:
    CCustomView(QWidget* parent);
    ~CCustomView();
};
