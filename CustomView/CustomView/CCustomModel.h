#pragma once

#include <QObject>
#include <QStringListModel>

class CCustomModel  : public QObject
{
    Q_OBJECT

public:
    CCustomModel(QObject *parent);
    ~CCustomModel();

    QStringListModel model;
};
