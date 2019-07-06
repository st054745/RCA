﻿#include <QCoreApplication>
#include <QFile>
#include <QSettings>

#include "Objects/controlunit.h"
#include "Objects/planner.h"
#include "Objects/scene.h"

#include "test.h"

int main(int argc, char *argv[])
{
    QApplication* a = new QCoreApplication(argc, argv);
    auto res = QTest::qExec(new Test, argc, argv);

    return res;

    /*const QString defaultRcaIp     = "localhost";
    const quint16 defaultRcaPort   = 5555;
    const quint16 defaultScenePort = 6666;

    QSettings settings("config.ini", QSettings::IniFormat);
    QString rcaIp = settings.value("HOSTS/Rca", defaultRcaIp).toString();
    quint16 rcaPort = static_cast<quint16>(settings.value("PORTS/Rca", defaultRcaPort).toInt());
    quint16 scenePort  = static_cast<quint16>(settings.value("PORTS/Scene", defaultScenePort).toInt());

    Scene* scene = new Scene(scenePort);
    Q_UNUSED(scene);

    Planner* planner = new Planner("p", rcaIp, rcaPort);
    //planner->show();

    ControlUnit* unit1 = new ControlUnit("t", rcaIp, rcaPort);
    //unit1->show();

    ControlUnit* unit2 = new ControlUnit("f", rcaIp, rcaPort);
    //unit2->show();

    return a->exec();
    */
}
