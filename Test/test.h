﻿#ifndef TEST_H
#define TEST_H

#include <QObject>
#include <QtTest>

#include "Objects/scene.h"
#include "Objects/controlunit.h"
#include "Objects/planner.h"

class Test : public QObject
{
    Q_OBJECT
private:
    QString rcaIp;
    QString sceneIp;
    quint16 rcaPort;
    quint16 scenePort;
public:
    explicit Test(QObject *parent = nullptr);
signals:

public slots:

private slots:
    void connectUnitToRca();
    void connectPlannerToRca();
    void sendMsgFromPlannerToUnitT();
};

#endif // TEST_H
