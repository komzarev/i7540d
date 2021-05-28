#pragma once

#include <QObject>
#include <QHostAddress>
#include <QTcpSocket>
#include <QByteArray>
#include "keepalivetcpsocket.hpp"

class SerialInterface : public keepalivetcpsocket::AliveTcpSocket
{
	Q_OBJECT

public:
	SerialInterface(QObject *parent);
	~SerialInterface();

    bool openSerialInterface(const QHostAddress& name, int port);
    void closeSerialInterface();
private slots:
    void onDisconnected();
private:
    bool isRunning();
	QHostAddress  name_;
	int port_{};
	bool isRunning_{};
};
