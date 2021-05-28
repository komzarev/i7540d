#pragma once
#include <QtCore/qglobal.h>
#include <QTcpServer>
#include <QTcpSocket>

namespace keepalivetcpsocket
{
	class KeepAlive
	{
	public:

		void enableKeepAlive(bool isOn)
		{
			isOn_ = isOn;
		}

		/*
		Gets or sets the number of seconds a TCP connection will remain idle before keepalive probes are sent to the remote.
		Default value is 1 sec
		*/
		void keepidle_sec(int val) { keepidle_sec_ = val; }

		/*
		Gets or sets the number of TCP keep alive probes that will be sent before the connection is terminated.
		It is illegal to set TCP_KEEPCNT to a value greater than 255.
		Default value is 3
		*/
		void keepcnt(int val) { keepcnt_ = val; }

		/*
		Gets or sets the number of seconds a TCP connection will wait for a keepalive response before sending another keepalive probe.
		Default value is 1 sec
		*/
		void keepint_sec(int val) { keepint_sec_ = val; }

	protected:
		bool isOn_ = false;
		int keepidle_sec_ = 1;
		int keepcnt_ = 3;
		int keepint_sec_ = 1;
		int setOptions(qintptr descriptor, bool isOn, int keepidle_sec = 1, int keepcnt = 3, int keepint_sec = 1);
	};

	class AliveTcpServer : public QTcpServer, public KeepAlive
	{
		Q_OBJECT

	public:
		AliveTcpServer(QObject* parent = 0) : QTcpServer(parent) { }

		void incomingConnection(qintptr socketDescriptor) override
		{
			QTcpSocket* socket = new QTcpSocket(this);
			socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
			setOptions(socketDescriptor, isOn_, keepidle_sec_, keepcnt_, keepint_sec_);
			socket->setSocketDescriptor(socketDescriptor);
			addPendingConnection(socket);
		}
	};

	class AliveTcpSocket : public QTcpSocket, public KeepAlive
	{
		Q_OBJECT

	public:

		AliveTcpSocket(QObject* parent = 0) : QTcpSocket(parent) 
		{
			connect(this, &QAbstractSocket::connected,[=] {
				int fd = socketDescriptor();
				setOptions(fd, isOn_, keepidle_sec_, keepcnt_, keepint_sec_);
			});
		}
	};

}
