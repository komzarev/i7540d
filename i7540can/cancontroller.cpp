#include "cancontroller.hpp"

CanController::CanController(QObject *parent)
	:QObject(parent)
{
}

CanController::~CanController()
{
	close();
}

bool CanController::open(const QHostAddress& name, int port)
{
	if (socket_.state() == QAbstractSocket::ConnectedState) {
		error_ = "Already connected";
		return false;
	}

	socket_.setSocketOption(QAbstractSocket::LowDelayOption, 1);
	socket_.connectToHost(name, port);

	if (!socket_.waitForConnected(1000)) {
		close();
		error_ = socket_.errorString();
		return false;
	}

	if (!requestConfig()) {
		error_ = "Fail to request CAN configuration";
		return false;
	}

	connect(&socket_, &QAbstractSocket::disconnected, this, &CanController::onDisconnected);
	return true;
}

bool CanController::updateConfig()
{
	auto user = userconfig.serialize();
	auto dev = deviceConfig_.serialize();
	if (user != dev) {

		auto ba = sentRecieve(user, 1000);
		if (ba.isEmpty()) {
			return false;
		}

		if (!requestConfig()) {
			return false;
		}

		dev = deviceConfig_.serialize();
		if (user != dev) {
			return false;
		}
	}
	return true;
}

bool CanController::requestConfig()
{
	auto ba = sentRecieve(deviceConfig_.request(), 1000);
	if (ba.isEmpty()) {
		return false;
	}

	return deviceConfig_.deserialize(ba);
}

void CanController::close()
{
	socket_.close();
}

void CanController::onDisconnected()
{
	emit statusChanged(false);
}

QByteArray CanController::sentRecieve(QByteArray sent, int timeout)
{
	QByteArray ba;
	if (socket_.state() == QAbstractSocket::ConnectedState) {
        qDebug() << "Write:" << sent;
        if (-1 == socket_.write(sent)) {
			error_ = socket_.errorString();
		} else if (!socket_.waitForReadyRead(timeout)) {
			error_ = "No answer for configuration";
		} else {
			ba = socket_.readAll();
            qDebug() << "READ:" << ba;
		}
	} else {
		error_ = "Not connected";
	}

	return ba;
}
