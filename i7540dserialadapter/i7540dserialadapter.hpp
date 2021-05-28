#ifndef I7540DSERIALADAPTER_HPP
#define I7540DSERIALADAPTER_HPP

#include "i7540dserialadapter_global.hpp"
#include <QSerialPort>
#include <QIODevice>

class SerialController;
class SerialInterface;

class QSERIALADAPTERI7540D_EXPORT I7540dSerialAdapter : public QIODevice
{
    Q_OBJECT

	Q_PROPERTY(qint32 baudRate READ baudRate WRITE setBaudRate NOTIFY baudRateChanged)
	Q_PROPERTY(QSerialPort::DataBits dataBits READ dataBits WRITE setDataBits NOTIFY dataBitsChanged)
	Q_PROPERTY(QSerialPort::Parity parity READ parity WRITE setParity NOTIFY parityChanged)
	Q_PROPERTY(QSerialPort::StopBits stopBits READ stopBits WRITE setStopBits NOTIFY stopBitsChanged)
	Q_PROPERTY(QSerialPort::SerialPortError error READ error RESET clearError NOTIFY errorOccurred)
	Q_PROPERTY(QSerialPort::FlowControl flowControl READ flowControl WRITE setFlowControl NOTIFY flowControlChanged)

public:
    enum SerialType {
      RS232 = 232,
      RS485 = 485
    };
    Q_ENUM(SerialType)

	explicit I7540dSerialAdapter(QObject* parent = nullptr);
	explicit I7540dSerialAdapter(const QString& name, QObject* parent = nullptr);
	explicit I7540dSerialAdapter(const QString& name, SerialType type, QObject* parent = nullptr);
    ~I7540dSerialAdapter();

	void setPortName(const QString& name);
	QString portName() const;

	bool setBaudRate(qint32 baudRate, QSerialPort::Directions directions = QSerialPort::AllDirections);
	qint32 baudRate(QSerialPort::Directions directions = QSerialPort::AllDirections) const;

	bool setDataBits(QSerialPort::DataBits dataBits);
	QSerialPort::DataBits dataBits() const;

	bool setParity(QSerialPort::Parity parity);
	QSerialPort::Parity parity() const;

	bool setStopBits(QSerialPort::StopBits stopBits);
	QSerialPort::StopBits stopBits() const;

	bool setFlowControl(QSerialPort::FlowControl flowControl);
	QSerialPort::FlowControl flowControl() const;

	QSerialPort::SerialPortError error() const;
	void clearError();

	SerialType type() const;
	void setType(SerialType val);

protected slots:
	void onDisconnected();
signals:
	void baudRateChanged(qint32 baudRate, QSerialPort::Directions directions);
	void dataBitsChanged(QSerialPort::DataBits dataBits);
	void parityChanged(QSerialPort::Parity parity);
	void stopBitsChanged(QSerialPort::StopBits stopBits);
	void flowControlChanged(QSerialPort::FlowControl flowControl);
	void errorOccurred(QSerialPort::SerialPortError error);
public:
    //QIODEvice interface
    bool isSequential() const override;
    bool open(OpenMode mode) override;
    void close() override;

    qint64 bytesAvailable() const override;
    qint64 bytesToWrite() const override;

    bool canReadLine() const override;
    bool waitForReadyRead(int msecs) override;
    bool waitForBytesWritten(int msecs) override;

protected:
    qint64 readData(char* data, qint64 maxlen) override;
    qint64 readLineData(char* data, qint64 maxlen) override;
    qint64 writeData(const char* data, qint64 len) override;
private:
	QString name_;
	SerialController* control_ = nullptr;
	SerialInterface* iface_ = nullptr;
	void setError(QString param1, QSerialPort::SerialPortError param2);
	QSerialPort::SerialPortError error_ = QSerialPort::NoError;
    SerialType serialType_ = SerialType::RS232;
};

#endif // I7540DSERIALADAPTER_HPP
