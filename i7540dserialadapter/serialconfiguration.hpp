#pragma once

#include <QObject>
#include <QSerialPort>

enum class InterfacePort {
	rs232 = 10001,
	rs485 = 10002,
};

struct SerialConfiguration
{
	QByteArray serialize();
	bool deserialize(const QByteArray& ba);
	QByteArray request();

	static QString bitrateCodeFromBitrate(int bitrate);
	static int bitrateFromBitrateCode(const QString &bitrate);

	static QSerialPort::Parity parityFromCode(char param1);
	static char codeFromParity(QSerialPort::Parity parity);

    InterfacePort interfacePort = InterfacePort::rs232;
	qint32 baundRate = QSerialPort::Baud9600;
	QSerialPort::DataBits datBits = QSerialPort::Data8;
	QSerialPort::Parity parity = QSerialPort::NoParity;
	QSerialPort::StopBits stopBits = QSerialPort::OneStop;
	QSerialPort::FlowControl flowControl = QSerialPort::NoFlowControl;

};
