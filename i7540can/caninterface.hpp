#pragma once

#include <QObject>
#include <QCanBusFrame>
#include <QHostAddress>
#include <QTcpSocket>
#include <QByteArray>
#include "keepalivetcpsocket.hpp"
#include "canserializer.hpp"

class CanInterface : public QObject
{
	Q_OBJECT

public:
	CanInterface(QObject *parent);
	~CanInterface();

	bool writeFrame(const QCanBusFrame& frame);
	void flush();
	bool open(const QHostAddress& name, int port);
	void close();

	QString errorString() const
	{
		return error_;
	}
	QVector<QCanBusFrame> frames()
	{
		QVector<QCanBusFrame> tmp;
		frames_.swap(tmp);
		return tmp;
	}

    void abort();
    QAbstractSocket::SocketState state();
protected slots:
	void onDataReady();
	void onDisconnected();
signals:
	void frameRecieved();
	void statusChanged(bool isConnected);
private:

    keepalivetcpsocket::AliveTcpSocket socket_;
	QString error_;
	CanSerializer serializer_;
	QVector<QCanBusFrame> frames_;
};
