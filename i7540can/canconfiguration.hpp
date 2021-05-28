#pragma once
#include <QByteArray>
#include <QCanBusDevice>

struct CanConfiguration
{
	QByteArray serialize();
	bool deserialize(const QByteArray& ba);
	QByteArray request();

	static int bitrateCodeFromBitrate(int bitrate);
	static int bitrateFromBitrateCode(int bitrate);

	QCanBusDevice::Filter filter;
	bool isCANBVersion = true;
	int baudRate = 500000;
	uint32_t acceptanceCode = 0;
	uint32_t acceptanceMask = 0xffffffff;
	bool isErrorResponse = 0;
	bool isTimestampResponse = 0;
};
