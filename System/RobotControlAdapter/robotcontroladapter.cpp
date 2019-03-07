﻿#include "robotcontroladapter.h"

RobotControlAdapter::RobotControlAdapter(quint16 rcaPort, QString sceneIp, quint16 scenePort)
{
    QTime timer;
    timer.restart();

    planner = nullptr;

    // Init scene socket and try to connect
    qInfo() << "Create scene socket";
    sceneSocket = new QTcpSocket(this);
    sceneSocket->connectToHost(sceneIp, scenePort);

    // Start listening
    if (this->listen(QHostAddress::Any, rcaPort))
    {
        qDebug() << "Listening RCA";
    }
    else
    {
        qDebug() << "Not listening RCA";
    }

    qDebug() << "Elapsed" << timer.elapsed() << "ms";
}

void RobotControlAdapter::incomingConnection(int socketDescriptor)
{
    QTime timer;
    timer.restart();

    // Create socket and set descriptor
    qInfo() << "Create client socket";
    QTcpSocket* socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);

    // Add to waitlist
    waitSockets.append(socket);

    // Connect signals and slots
    connect(socket, &QTcpSocket::readyRead, this, &RobotControlAdapter::readyRead);

    qDebug() << "Elapsed" << timer.elapsed() << "ms";
}

void RobotControlAdapter::readyRead()
{
    QTime timer;
    timer.restart();

    // Find the sender and cast into the socket
    QObject* object = QObject::sender();
    QTcpSocket* socket = static_cast<QTcpSocket*>(object);

    // Read data
    QByteArray data = socket->readAll();

    if (data.size() == 1 && data != "e") // Object names, or shutdown command, or planner cmd with only 1 symbol, except for command 'e'
    {
        processSingleCharCmd(socket, data);
    }
    else // Other commands
    {
        QList<QByteArray> list = data.split('|'); // Get a list of commands

        if  (socket == planner)
        {
            for (int i = 0; i < list.size(); i++)
            {
                processPlannerCmd(list[i]);
            }
        }
        else
        {
            for (int i = 0; i < list.size(); i++)
            {
                processUnitCmd(list[i]);
            }
        }
    }

    qDebug() << "Elapsed" << timer.elapsed() << "ms";
}

bool RobotControlAdapter::isConnectedState(QTcpSocket* socket) const
{
    return socket->state() == QTcpSocket::SocketState::ConnectedState;
}

bool RobotControlAdapter::isUnconnectedState(QTcpSocket* socket) const
{
    return socket->state() == QTcpSocket::SocketState::UnconnectedState;
}

void RobotControlAdapter::processPlannerCmd(QByteArray plannerCmd)
{
    qInfo() << "Process planner command -" << plannerCmd;

    QTime timer;
    timer.restart();

    if (plannerCmd == "e") // Planner sends shutdown command (planner already exists at this moment)
    {
        for (const auto& client : clients) // Send shutdown command to all clients
        {
            client->write(plannerCmd);
        }
        clients.clear();
    }
    else // Planner sends other command
    {
        QList<QByteArray> pairNameAndCmd = plannerCmd.split(':');

        if (pairNameAndCmd.size() != 2) // Wrong command
        {
            qWarning() << "Wrong command";
            return;
        }

        QByteArray name = pairNameAndCmd[0];
        QByteArray cmd  = pairNameAndCmd[1];

        if (clients.contains(name)) // If client exists
        {
            clients[name]->write(cmd); // Send message to the unit
        }
        else // Not exist
        {
            qWarning() << "Client doesn't exists";
        }
    }

    qDebug() << "Elapsed -" << timer.elapsed() << "ms";
}

void RobotControlAdapter::processUnitCmd(QByteArray unitCmd)
{
    qInfo() << "Process unit command -" << unitCmd;

    QTime timer;
    timer.restart();

    QList<QByteArray> pairNameAndCmd = unitCmd.split(':');

    QByteArray name = pairNameAndCmd[0];
    QByteArray cmd  = pairNameAndCmd[1];
    sceneSocket->write("{" + name + " : " + cmd + "}");

    qDebug() << "Elapsed" << timer.elapsed() << "ms";
}

void RobotControlAdapter::processSingleCharCmd(QTcpSocket* socket, QByteArray name)
{
    qInfo() << "Process single char command -" << name;

    QTime timer;
    timer.restart();

    if (name == "p") // Planner sends its name
    {
        if (planner != nullptr && isUnconnectedState(planner)) // Already exist, but disconnected
        {
            planner->deleteLater();
        }
        planner = socket; // Init or reinit socket
        waitSockets.removeOne(socket);
    }
    else if (socket != planner) // It's from one of units
    {
        if (clients.contains(name)) // Client isn't in the list and sends us his name
        {
            clients.insert(name, socket);
        }
        else if (clients.contains(name) && isUnconnectedState(clients[name])) // Exists, but disconnected
        {
            clients[name]->deleteLater();
            clients[name] = socket;
        }
        waitSockets.removeOne(socket);
    }
    else
    {
        qWarning() << "Unknown command from planner";
    }

    qDebug() << "Elapsed" << timer.elapsed() << "ms";
}
