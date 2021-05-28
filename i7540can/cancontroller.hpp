#pragma once
#include <QObject>
#include <QCanBusFrame>
#include <QHostAddress>
#include <QTcpSocket>
#include <QByteArray>

#include "canconfiguration.hpp"
#include "canserializer.hpp"

class CanController : public QObject
{
	Q_OBJECT

public:
	CanController(QObject* parent);
	~CanController();

	bool open(const QHostAddress& name, int port);
	bool updateConfig();
	bool requestConfig();
	void close();

	CanConfiguration userconfig;
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
	CanConfiguration deviceConfig_;
};
