#include "serialcontroller.hpp"

SerialController::SerialController(QObject *parent)
	:QObject(parent)
{
}

SerialController::~SerialController()
{
	close();
}

bool SerialController::open(const QHostAddress& name, int port)
{
	if (socket_.state() == QAbstractSocket::ConnectedState) {
		error_ = "Already connected";
		return false;
	}

	socket_.setSocketOption(QAbstractSocket::LowDelayOption, 1);
	socket_.connectToHost(name, port);

	if (!socket_.waitForConnected(500)) {
		close();
		error_ = socket_.errorString();
		return false;
	}

	if (!requestConfig()) {
		error_ = "Fail to request Serial configuration";
		return false;
	}

	connect(&socket_, &QAbstractSocket::disconnected, this, &SerialController::onDisconnected);
	return true;
}

bool SerialController::updateConfig()
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

bool SerialController::requestConfig()
{
	auto ba = sentRecieve(deviceConfig_.request(), 1000);
	if (ba.isEmpty()) {
		return false;
	}

	return deviceConfig_.deserialize(ba);
}

void SerialController::close()
{
	socket_.close();
}

void SerialController::onDisconnected()
{
	emit statusChanged(false);
}

QByteArray SerialController::sentRecieve(QByteArray sent, int timeout)
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
