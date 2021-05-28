#include "caninterface.hpp"
#include <QThread>

CanInterface::CanInterface(QObject *parent)
	: QObject(parent)
{
    connect(&socket_, &QAbstractSocket::disconnected, this, &CanInterface::onDisconnected,Qt::QueuedConnection);
    connect(&socket_, &QIODevice::readyRead, this, &CanInterface::onDataReady);
}

CanInterface::~CanInterface()
{
	close();
}

bool CanInterface::writeFrame(const QCanBusFrame& frame)
{
	if (socket_.state() == QAbstractSocket::ConnectedState) {
        auto data = serializer_.serialize(frame);
        qDebug() <<"Write"<<data;
        if (-1 == socket_.write(data)) {
			error_ = socket_.errorString();
			return false;
		}
		return true;
	}

	error_ = "Not connected";
	return false;
}

void CanInterface::flush()
{
	socket_.flush();
}

bool CanInterface::open(const QHostAddress& name, int port)
{
	if (socket_.state() == QAbstractSocket::ConnectedState) {
		error_ = "Already connected";
		return false;
	}

    socket_.enableKeepAlive(true);
    socket_.keepcnt(3);
    socket_.keepidle_sec(1);
    socket_.keepint_sec(1);

    socket_.setSocketOption(QAbstractSocket::LowDelayOption, 1);

	socket_.connectToHost(name, port);
	if (!socket_.waitForConnected(1000)) {
		close();
		error_ = socket_.errorString();
		return false;
	}

	serializer_.resetData();
	return true;
}

void CanInterface::close()
{
    socket_.close();
}

void CanInterface::abort()
{
    socket_.abort();
}

QAbstractSocket::SocketState CanInterface::state()
{
    return socket_.state();
}

void CanInterface::onDataReady()
{
    auto ba = socket_.readAll();
    qDebug() <<"Read"<<ba;

    if(ba.isEmpty()){
        return;
    }

	serializer_.addData(std::move(ba));
	frames_.append(serializer_.frames());

	if (!frames_.isEmpty()) {
		emit frameRecieved();
	}
}

void CanInterface::onDisconnected()
{
    emit statusChanged(false);
}
