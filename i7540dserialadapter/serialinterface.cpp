#include "serialinterface.hpp"
#include <QDebug>

SerialInterface::SerialInterface(QObject *parent)
    : keepalivetcpsocket::AliveTcpSocket(parent)
{
}

SerialInterface::~SerialInterface()
{
    close();
}

bool SerialInterface::openSerialInterface(const QHostAddress& name, int port)
{
    name_ = name;
    port_ = port;

    if (state() == QAbstractSocket::ConnectedState) {
        return false;
    }

    enableKeepAlive(true);
    keepcnt(3);
    keepidle_sec(1);
    keepint_sec(1);

    setSocketOption(QAbstractSocket::LowDelayOption, 1);
    connectToHost(name, port);
    if (!waitForConnected(500)) {
        close();
        return false;
    }
    isRunning_ = true;

    connect(this, &QAbstractSocket::disconnected, this, &SerialInterface::onDisconnected, Qt::QueuedConnection);
    return true;
}

void SerialInterface::closeSerialInterface()
{
    isRunning_ = false;
    close();
}

void SerialInterface::onDisconnected()
{
    if (isRunning()) {
        qDebug() << "Try reconnect";
        close();
        if (openSerialInterface(name_, port_)) {
            qDebug() << "Reconnect: [OK]";
            return;
        }

        qDebug() << "Reconnect: [FAIL]";
    }
}

bool SerialInterface::isRunning()
{
    return isRunning_;
}
