#include "canconfiguration.hpp"
#include <QRegularExpression>

static QByteArray toHex(uint32_t value, int count)
{
	auto str = QByteArray::number(value, 16);
	auto prep = count - str.size();
	if (prep > 0) {
		while (prep--) {
			str.prepend('0');
		}
	} else if (prep < 0) {
		str = str.right(count);
	}

	return str;
}

QByteArray CanConfiguration::serialize()
{
	/*
	Syntax: 99$P114PBCCCCCCCCMMMMMMMMET
	99$P114 Command character
	P CAN specification
		0 = 2.0A
		1 = 2.0B
	B CAN Baud rate
	CCCCCCCC 32 bits Acceptance Code Register (00000000~FFFFFFFF)
	MMMMMMMM 32 bits Acceptance Mask Register (00000000~FFFFFFFF)
	E Error response or not
		0 = Disable
		1 = Enable
	T Timestamp response or not
		0 = Disable
		1 = Enable
	*/

	QByteArray ret = "99$P114";
	ret.append(isCANBVersion ? "1" : "0");
	ret.append(QByteArray::number(bitrateCodeFromBitrate(baudRate)));
	ret.append(toHex(acceptanceCode, 8));
	ret.append(toHex(acceptanceMask, 8));
	ret.append(isErrorResponse ? "1" : "0");
	ret.append(isTimestampResponse ? "1" : "0");
	return ret;
}

bool CanConfiguration::deserialize(const QByteArray& ba)
{
	/*
	Valid Command: 14PBCCCCCCCCMMMMMMMMET
	Invalid command: ERROR

	14 Delimiter for valid command
	P CAN specification
		0 = 2.0A
		1 = 2.0B
	B CAN Baud rate
	CCCCCCCC 32 bits Acceptance Code Register (00000000~FFFFFFFF)
	MMMMMMMM 32 bits Acceptance Mask Register (00000000~FFFFFFFF)
	E Error response or not
		0 = Disable
		1 = Enable
	T Timestamp response or not
		0 = Disable
		1 = Enable

	*/


	const QRegularExpression re(QStringLiteral("14([0-1]{1})([0-9]{1})([0-9A-F]{8})([0-9A-F]{8})([0-1]{1})([0-1]{1})"));
	const QRegularExpressionMatch match = re.match(ba);

	if (Q_LIKELY(match.hasMatch())) {
		isCANBVersion = match.captured(1).toInt() == 1;
		baudRate = bitrateFromBitrateCode(match.captured(2).toInt());
		bool ok{};
		//I do not check 'ok' because it was already checked in match method
		acceptanceCode = match.captured(3).toULong(&ok, 16);
		acceptanceMask = match.captured(4).toULong(&ok, 16);
		isErrorResponse = match.captured(5).toInt() == 1;
		isTimestampResponse = match.captured(6).toInt() == 1;
		return true;
	} 

	return false;
}

QByteArray CanConfiguration::request()
{
	return QByteArray("99#P1");
}

int CanConfiguration::bitrateCodeFromBitrate(int bitrate)
{
	struct BitrateItem {
		int bitrate;
		int code;
	} bitrateTable[] = {
		{   10000, 0  },
		{   20000, 1},
		{   50000, 2},
		{  100000, 3},
		{  125000, 4},
		{  250000, 5},
		{  500000, 6},
		{  800000, 7},
		{ 1000000, 8}
	};

	const int entries = (sizeof(bitrateTable) / sizeof(*bitrateTable));
	for (int i = 0; i < entries; ++i)
		if (bitrateTable[i].bitrate == bitrate)
			return bitrateTable[i].code;

	return -1;
}

int CanConfiguration::bitrateFromBitrateCode(int bitrate)
{
	struct BitrateItem {
		int bitrate;
		int code;
	} bitrateTable[] = {
		{   10000, 0  },
		{   20000, 1},
		{   50000, 2},
		{  100000, 3},
		{  125000, 4},
		{  250000, 5},
		{  500000, 6},
		{  800000, 7},
		{ 1000000, 8}
	};

	const int entries = (sizeof(bitrateTable) / sizeof(*bitrateTable));
	for (int i = 0; i < entries; ++i)
		if (bitrateTable[i].code == bitrate)
			return bitrateTable[i].bitrate;

	return -1;

}
