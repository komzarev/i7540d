#pragma once
#include <QObject>
#include <QHostAddress>
#include <QTcpSocket>
#include <QByteArray>

#include "serialconfiguration.hpp"

class SerialController : public QObject
{
	Q_OBJECT

public:
	SerialController(QObject* parent);
	~SerialController();

	bool open(const QHostAddress& name, int port);
	bool updateConfig();
	bool requestConfig();
	void close();

	SerialConfiguration userconfig;

	QString errorString() const
	{
		return error_;
	}

signals:
	void statusChanged(bool isConnected);
protected slots:
	void onDisconnected();
private:
	QByteArray sentRecieve(QByteArray sent, int timeout);
	QTcpSocket socket_;
	QString error_;
	SerialConfiguration deviceConfig_;
};
