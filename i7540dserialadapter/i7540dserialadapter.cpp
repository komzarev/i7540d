#include "i7540dserialadapter.hpp"
#include "serialcontroller.hpp"
#include "serialinterface.hpp"

namespace {

inline constexpr InterfacePort toInterfacePort(I7540dSerialAdapter::SerialType type) {
    return (I7540dSerialAdapter::RS485 == type ? InterfacePort::rs485 : InterfacePort::rs232);
}

inline constexpr I7540dSerialAdapter::SerialType sanitized(I7540dSerialAdapter::SerialType type) {
    return (I7540dSerialAdapter::RS485 == type ? I7540dSerialAdapter::RS485 : I7540dSerialAdapter::RS232);
}

}

I7540dSerialAdapter::I7540dSerialAdapter(QObject* parent /*= nullptr*/):
    I7540dSerialAdapter("", SerialType::RS232, parent)
{

}

I7540dSerialAdapter::I7540dSerialAdapter(const QString& name, QObject* parent /*= nullptr*/):
    I7540dSerialAdapter(name, SerialType::RS232, parent)
{
}

I7540dSerialAdapter::I7540dSerialAdapter(const QString& name, SerialType type, QObject* parent /*= nullptr*/):
    QIODevice(parent),name_(name), serialType_(sanitized(type))
{
	control_ = new SerialController(this);
	iface_ = new SerialInterface(this);
    control_->userconfig.interfacePort = toInterfacePort(serialType_);
}

I7540dSerialAdapter::~I7540dSerialAdapter()
{
	if (isOpen()) {
		close();
	}
}

void I7540dSerialAdapter::setPortName(const QString& name)
{
	name_ = name;
}

QString I7540dSerialAdapter::portName() const
{
	return name_;
}

bool I7540dSerialAdapter::setBaudRate(qint32 baudRate, QSerialPort::Directions directions /*= QSerialPort::AllDirections*/)
{
	Q_UNUSED(directions)

	if (SerialConfiguration::bitrateCodeFromBitrate(baudRate).isEmpty()) {
		setError(QString("Can't set baudRate %1").arg(baudRate), QSerialPort::UnknownError);
		return false;
	}
	control_->userconfig.baundRate = baudRate;
	emit baudRateChanged(baudRate,QSerialPort::AllDirections);
	return true;
}

qint32 I7540dSerialAdapter::baudRate(QSerialPort::Directions directions /*= QSerialPort::AllDirections*/) const
{
	Q_UNUSED(directions)

	return control_->userconfig.baundRate;
}

bool I7540dSerialAdapter::setDataBits(QSerialPort::DataBits dataBits)
{
	if (dataBits != QSerialPort::Data7 && dataBits != QSerialPort::Data8) {
		setError(QString("Can't set data bit %1").arg(QVariant::fromValue(dataBits).toString()), QSerialPort::UnknownError);
		return false;
	}

	control_->userconfig.datBits = dataBits;
	emit dataBitsChanged(dataBits);
	return true;
}

QSerialPort::DataBits I7540dSerialAdapter::dataBits() const
{
	return control_->userconfig.datBits;
}

bool I7540dSerialAdapter::setParity(QSerialPort::Parity parity)
{
	if (SerialConfiguration::codeFromParity(parity) == '\0') {
		setError(QString("Can't set parity %1").arg(QVariant::fromValue(parity).toString()), QSerialPort::UnknownError);
		return false;
	}

	control_->userconfig.parity = parity;
	emit parityChanged(parity);
	return true;
}

QSerialPort::Parity I7540dSerialAdapter::parity() const
{
	return control_->userconfig.parity;
}

bool I7540dSerialAdapter::setStopBits(QSerialPort::StopBits stopBits)
{
	if (stopBits != QSerialPort::OneStop && stopBits != QSerialPort::TwoStop) {
		setError(QString("Can't set stopBits %1").arg(QVariant::fromValue(stopBits).toString()), QSerialPort::UnknownError);
		return false;
	}
	control_->userconfig.stopBits = stopBits;
	emit stopBitsChanged(stopBits);
	return true;
}

QSerialPort::StopBits I7540dSerialAdapter::stopBits() const
{
	return control_->userconfig.stopBits;
}

bool I7540dSerialAdapter::setFlowControl(QSerialPort::FlowControl flowControl)
{
	if (flowControl != QSerialPort::NoFlowControl) {
		setError(QString("Can't set flowControl %1").arg(QVariant::fromValue(flowControl).toString()), QSerialPort::UnknownError);
		return false;
	}

	emit flowControlChanged(flowControl);
	return true;
}

QSerialPort::FlowControl I7540dSerialAdapter::flowControl() const
{
	return QSerialPort::NoFlowControl;
}

QSerialPort::SerialPortError I7540dSerialAdapter::error() const
{
	return error_;
}

void I7540dSerialAdapter::clearError()
{
	error_ = QSerialPort::NoError;
}

I7540dSerialAdapter::SerialType I7540dSerialAdapter::type() const
{
    return serialType_;
}

void I7540dSerialAdapter::setType(SerialType val)
{
    serialType_ = sanitized(val);
}

void I7540dSerialAdapter::onDisconnected()
{
	close();
}

bool I7540dSerialAdapter::isSequential() const
{
	return true;
}

bool I7540dSerialAdapter::open(OpenMode mode)
{
	if (iface_->state() == QAbstractSocket::ConnectedState) {
		setError("Already open", QSerialPort::OpenError);
        return false;
	}

	QHostAddress addr;
	if (!addr.setAddress(name_)) {
		setError(tr("Wrong format of IP address: %1.").arg(name_), QSerialPort::OpenError);
		return false;
	}

    control_->userconfig.interfacePort = toInterfacePort(serialType_);

	const int CONTROL_PORT = 10000;
    const int INTERFACE_PORT = static_cast<int>(control_->userconfig.interfacePort);

	if (!control_->open(addr, CONTROL_PORT)) {
		setError(tr("Can't connect to: %1:%2, reason: %3").arg(name_).arg(CONTROL_PORT).arg(control_->errorString()), QSerialPort::OpenError);
		return false;
	}

	if (!control_->updateConfig()) {
		setError(tr("Can't update configuration"), QSerialPort::OpenError);
		return false;
	}

	control_->close();

	if (!iface_->openSerialInterface(addr, INTERFACE_PORT)) {
		setError(tr("Can't connect to: %1:%2").arg(name_).arg(INTERFACE_PORT), QSerialPort::OpenError);
		return false;
	}

	connect(iface_, &QAbstractSocket::disconnected, this, &I7540dSerialAdapter::onDisconnected);
	connect(iface_, &SerialInterface::readyRead, this, &I7540dSerialAdapter::readyRead);
	connect(iface_, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),[=](QAbstractSocket::SocketError socketError) {

		QSerialPort::SerialPortError error = QSerialPort::UnknownError;

		switch (socketError)
		{
		case QAbstractSocket::ConnectionRefusedError:
		case QAbstractSocket::RemoteHostClosedError:
		case QAbstractSocket::SocketAddressNotAvailableError:
		case QAbstractSocket::HostNotFoundError:
			error = QSerialPort::DeviceNotFoundError;
			break;
		case QAbstractSocket::SocketAccessError:
			error = QSerialPort::PermissionError;
			break;
		case QAbstractSocket::SocketResourceError:
			error = QSerialPort::ResourceError;
			break;
		case QAbstractSocket::SocketTimeoutError:
			error = QSerialPort::TimeoutError;
			break;
		case QAbstractSocket::UnsupportedSocketOperationError:
			error = QSerialPort::UnsupportedOperationError;
			break;
		default:
			break;
		}
		setError(iface_->errorString(), error);
	});

	return QIODevice::open(mode);
}

void I7540dSerialAdapter::close()
{
	control_->close();
	iface_->closeSerialInterface();
	QIODevice::close();
}

qint64 I7540dSerialAdapter::bytesAvailable() const
{
	return iface_->bytesAvailable();
}

qint64 I7540dSerialAdapter::bytesToWrite() const
{
	return iface_->bytesToWrite();
}

bool I7540dSerialAdapter::canReadLine() const
{
	return iface_->canReadLine();
}

bool I7540dSerialAdapter::waitForReadyRead(int msecs)
{
	if (iface_->state() != QAbstractSocket::ConnectedState) {
		setError("Port not open", QSerialPort::NotOpenError);
		return false;
	}
	return iface_->waitForReadyRead(msecs);
}

bool I7540dSerialAdapter::waitForBytesWritten(int msecs)
{
	if (iface_->state() != QAbstractSocket::ConnectedState) {
		setError("Port not open", QSerialPort::NotOpenError);
		return false;
	}

	return iface_->waitForBytesWritten(msecs);
}

qint64 I7540dSerialAdapter::readData(char* data, qint64 maxlen)
{
	if (iface_->state() != QAbstractSocket::ConnectedState) {
		setError("Port not open", QSerialPort::NotOpenError);
		return 0;
	}

    return iface_->read(data,maxlen);
}

qint64 I7540dSerialAdapter::readLineData(char* data, qint64 maxlen)
{
	if (iface_->state() != QAbstractSocket::ConnectedState) {
		setError("Port not open", QSerialPort::NotOpenError);
		return 0;
	}
    return iface_->readLine(data, maxlen);
}

qint64 I7540dSerialAdapter::writeData(const char* data, qint64 len)
{
    qDebug()<<"wriet";
	if (iface_->state() != QAbstractSocket::ConnectedState) {
		setError("Port not open", QSerialPort::NotOpenError);
		return 0;
	}
    return iface_->write(data, len);
}

void I7540dSerialAdapter::setError(QString param1, QSerialPort::SerialPortError param2)
{
	error_ = param2;
	setErrorString(param1);
	emit errorOccurred(error_);
}
