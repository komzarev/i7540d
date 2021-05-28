#include "i7540canbackend.hpp"

#include <QCanBusDevice>

#include <QCoreApplication>
#include <QDebug>
#include <QLoggingCategory>
#include <QRegularExpression>
#include <QTimer>

namespace {
    const int control_port = 10000;
    const int interface_port = 10003;
}

bool I7540CanBackend::canCreate(QString* errorReason)
{
	Q_UNUSED(errorReason)
    return true;
}

void I7540CanBackend::onStatusChanged(bool isConnected)
{
    if (!isConnected && isRunning()) {
        printf("Try reconnect\n");
        if (iface_->state() == QAbstractSocket::ConnectedState) {
            return;
        }

        outGoingTimer_.stop();
        iface_->abort();

        if (iface_->open(addr_, interface_port)) {
            printf("Reconnect: [OK]\n");
            outGoingTimer_.start();
            return;
        }

        setError(tr("Can't connect to: %1:%2").arg(name_).arg(interface_port), QCanBusDevice::ConnectionError);
        printf("Reconnect: [FAIL]\n");
    }

    if (!isConnected) {
        setState(QCanBusDevice::UnconnectedState);
    }
}

QList<QCanBusDeviceInfo> I7540CanBackend::interfaces()
{
	QList<QCanBusDeviceInfo> result;
	return result;
}


void I7540CanBackend::setConfigurationParameter(int key, const QVariant& value)
{
	bool ret = true;
	switch (key) {
	case QCanBusDevice::BitRateKey:
		if (!verifyBitRate(value.toInt())) {
			//setError used in verifyBitRate
			ret = false;
		} else {
			control_->userconfig.baudRate = value.toInt();
		}
		break;

	case QCanBusDevice::ReceiveOwnKey:
		if (Q_UNLIKELY(state() != QCanBusDevice::UnconnectedState)) {
			setError("Cannot configure TxEcho for open device", QCanBusDevice::ConfigurationError);
			ret = false;
		}
		break;
	case QCanBusDevice::RawFilterKey:
	{
		auto list = value.value<QList<QCanBusDevice::Filter>>();
		if (list.size() > 1) {
			setError(tr("Only one filter supported"), QCanBusDevice::ConfigurationError);
			ret = false;
		} else {
			auto filter = list[0];
			control_->userconfig.acceptanceCode = filter.frameId;
			control_->userconfig.acceptanceMask = filter.frameIdMask;
		}
	}
	break;
	default:
		setError(QString("Unsupported configuration key: %1").arg(key), QCanBusDevice::ConfigurationError);
		ret = false;
	}

	if (ret) {
		QCanBusDevice::setConfigurationParameter(key, value);
	}
}

void I7540CanBackend::onFrameRecieved()
{
	enqueueReceivedFrames(iface_->frames());
}

bool I7540CanBackend::verifyBitRate(int bitrate)
{
	if (Q_UNLIKELY(state() != QCanBusDevice::UnconnectedState)) {
		setError("Cannot configure bitrate for open device",
			QCanBusDevice::ConfigurationError);
		return false;
	}

	if (Q_UNLIKELY(CanConfiguration::bitrateCodeFromBitrate(bitrate) == -1)) {
		setError(QString("Unsupported bitrate %1.").arg(bitrate),
			QCanBusDevice::ConfigurationError);
		return false;
	}

	return true;
}

int I7540CanBackend::getBatchSize(int baudRate)
{
    struct BitrateItem {
        int bitrate;
        uint code;
    } bitrateTable[] = {
        {   10000, 4  },
        {   20000, 7  },
        {   50000, 14 },
        {  100000, 27 },
        {  125000, 31 },
        {  250000, 39 },
        {  500000, 50 },
        {  800000, 50 },
        { 1000000, 50 }
    };

    const int entries = (sizeof(bitrateTable) / sizeof(*bitrateTable));
    for (int i = 0; i < entries; ++i)
        if (bitrateTable[i].bitrate == baudRate)
            return bitrateTable[i].code;

    return -1;
}

I7540CanBackend::I7540CanBackend(const QString& name, QObject* parent) :
    QCanBusDevice(parent),name_(name)
{
	control_ = new CanController(this);
	iface_ = new CanInterface(this);

    connect(iface_, &CanInterface::statusChanged, this, &I7540CanBackend::onStatusChanged);
    connect(iface_, &CanInterface::frameRecieved, this, &I7540CanBackend::onFrameRecieved);

	outGoingTimer_.callOnTimeout([this] {
		startWrite();
	});
	outGoingTimer_.setInterval(50);
}

I7540CanBackend::~I7540CanBackend()
{
	if (state() == QCanBusDevice::ConnectedState) {
		close();
	}
}

bool I7540CanBackend::open()
{
    if (!addr_.setAddress(name_)) {
		setError(tr("Wrong format of IP address: %1.").arg(name_), QCanBusDevice::ConnectionError);
		return false;
	}

    if (!control_->open(addr_, control_port)) {
        setError(tr("Can't connect to: %1:%2, reason: %3").arg(name_).arg(control_port).arg(control_->errorString()), QCanBusDevice::ConnectionError);
		return false;
	}

	if (!control_->updateConfig()) {
		setError(tr("Can't update configuration"), QCanBusDevice::ConnectionError);
		return false;
	}

	control_->close();

    if (!iface_->open(addr_, interface_port)) {
        setError(tr("Can't connect to: %1:%2").arg(name_).arg(interface_port), QCanBusDevice::ConnectionError);
		return false;
	}

	batchSize_ = getBatchSize(control_->userconfig.baudRate);
    isRunning_ = true;
	setState(QCanBusDevice::ConnectedState);
	outGoingTimer_.start();
	return true;
}

void I7540CanBackend::close()
{
    outGoingTimer_.stop();
    isRunning_ = false;
	control_->close();
	iface_->close();
	setState(QCanBusDevice::UnconnectedState);
}

bool I7540CanBackend::writeFrame(const QCanBusFrame& newData)
{
	if (Q_UNLIKELY(state() != QCanBusDevice::ConnectedState)) {
		setError(tr("Cannot write frame in unconnected state"), QCanBusDevice::WriteError);
		return false;
	}

	if (Q_UNLIKELY(!newData.isValid())) {
		setError(tr("Cannot write invalid QCanBusFrame"), QCanBusDevice::WriteError);
		return false;
	}

	const QCanBusFrame::FrameType type = newData.frameType();
	if (Q_UNLIKELY(type != QCanBusFrame::DataFrame && type != QCanBusFrame::RemoteRequestFrame)) {
		setError(tr("Unable to write a frame with unacceptable type"),
			QCanBusDevice::WriteError);
		return false;
	}

	// CAN FD frame format is not supported by the hardware yet
	if (Q_UNLIKELY(newData.hasFlexibleDataRateFormat())) {
		setError(tr("CAN FD frame format not supported"), QCanBusDevice::WriteError);
		return false;
	}

    enqueueOutgoingFrame(newData);
	return true;
}

void I7540CanBackend::startWrite()
{
	if (Q_UNLIKELY(state() != QCanBusDevice::ConnectedState)) {
		return;
	}

	if (!hasOutgoingFrames()) {
		return;
	}

    if(iface_->state() != QAbstractSocket::ConnectedState){
        return;
    }

	int framesWrittenOut = 0;
	for (; framesWrittenOut < batchSize_; ++framesWrittenOut) {
		if (hasOutgoingFrames()) {
			const QCanBusFrame frame = dequeueOutgoingFrame();
			if (Q_UNLIKELY(!iface_->writeFrame(frame))) {
				setError(tr("Write error: %1").arg(iface_->errorString()), QCanBusDevice::WriteError);
				break;
			}
		} else {
			break;
		}
	}

	if (framesWrittenOut > 0){
		iface_->flush();
		emit framesWritten(qint64(framesWrittenOut));
	}
}

QString I7540CanBackend::interpretErrorFrame(const QCanBusFrame& errorFrame)
{
	Q_UNUSED(errorFrame);
	return QString();
}
