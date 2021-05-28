#include "serialconfiguration.hpp"
#include "QRegularExpression"

QByteArray SerialConfiguration::serialize()
{
	/*
	Syntax: 99$P0105BBDSP

	99$P0105 Command character
	BB RS-232 Baud rate
	D Data bit
		0 = 7 bits data formation
		1 = 8 bits data formation
	S Stop bit
		0 = 1 stop bit
		1 = 2 stop bits
	P Parity bits
		0 = None
		1 = Even
		2 = Odd
	*/

	QByteArray ret = "99$P0105";
	ret.append(bitrateCodeFromBitrate(baundRate));
	ret.append(datBits == QSerialPort::Data8 ? "1" : "0");
	ret.append(stopBits == QSerialPort::TwoStop ? "1" : "0");
	ret.append(codeFromParity(parity));
	return ret;
}

bool SerialConfiguration::deserialize(const QByteArray& ba)
{
	/*
	Valid Command: 061BBDSP
	Invalid command: ERROR

	061 Delimiter for valid command
	BB RS-232 Baud rate
	D Data bit
		0 = 7 bits data formation
		1 = 8 bits data formation
	S Stop bit
		0 = 1 stop bit
		1 = 2 stop bits
	P Parity bits
		0 = None
		1 = Even
		2 = Odd
	*/

    const QRegularExpression re(QStringLiteral("061([0-9A-F]{2})([0-1]{1})([0-1]{1})([0-2]{1})"));
	const QRegularExpressionMatch match = re.match(ba);

	if (Q_LIKELY(match.hasMatch())) {
		baundRate = bitrateFromBitrateCode(match.captured(1));
		datBits = match.captured(2).toInt() == 1 ? QSerialPort::Data8 : QSerialPort::Data7;
		stopBits = match.captured(3).toInt() == 1 ? QSerialPort::TwoStop : QSerialPort::OneStop;
		parity = parityFromCode(match.captured(4).toLatin1()[0]);
		return true;
	} 

	return false;
}

QByteArray SerialConfiguration::request()
{
    if (interfacePort == InterfacePort::rs232) {
		return QByteArray("99#P01");
	} else {
		return QByteArray("99#P02");
	}
}

QString SerialConfiguration::bitrateCodeFromBitrate(int bitrate)
{
	struct BitrateItem {
		int bitrate;
		QString code;
	} bitrateTable[] = {
		{   110, "00"},
		{   150, "01"},
		{   300, "02"},
		{   600, "03"},
		{   1200, "04"},
		{   2400, "05"},
		{   4800, "06"},
		{   9600, "07"},
		{   19200, "08"},
		{  38400, "09"},
		{  57600, "0A"},
		{  115200, "0B"},
	};

	const int entries = (sizeof(bitrateTable) / sizeof(*bitrateTable));
	for (int i = 0; i < entries; ++i)
		if (bitrateTable[i].bitrate == bitrate)
			return bitrateTable[i].code;

	return "";
}

int SerialConfiguration::bitrateFromBitrateCode(const QString& bitrate)
{
	struct BitrateItem {
		int bitrate;
		QString code;
	} bitrateTable[] = {
		{   110, "00"},
		{   150, "01"},
		{   300, "02"},
		{   600, "03"},
		{   1200, "04"},
		{   2400, "05"},
		{   4800, "06"},
		{   9600, "07"},
		{   19200, "08"},
		{  38400, "09"},
		{  57600, "0A"},
		{  115200, "0B"},
	};

	const int entries = (sizeof(bitrateTable) / sizeof(*bitrateTable));
	for (int i = 0; i < entries; ++i)
		if (bitrateTable[i].code == bitrate)
			return bitrateTable[i].bitrate;

	return -1;
}

QSerialPort::Parity SerialConfiguration::parityFromCode(char param)
{
	QSerialPort::Parity par = QSerialPort::UnknownParity;

	switch (param)
	{
	case '0':
		par = QSerialPort::NoParity;
		break;
	case '1':
		par = QSerialPort::EvenParity;
		break;
	case '2':
		par = QSerialPort::OddParity;
		break;
	default:
		break;
	}
	return par;
}

char SerialConfiguration::codeFromParity(QSerialPort::Parity parity)
{
	char ba = '\0';
	switch (parity)
	{
	case QSerialPort::NoParity:
		ba = '0';
		break;
	case QSerialPort::EvenParity:
		ba = '1';
		break;
	case QSerialPort::OddParity:
		ba = '2';
		break;
	case QSerialPort::SpaceParity:
	case QSerialPort::MarkParity:
	case QSerialPort::UnknownParity:
	default:
		break;
	}
	return ba;
}
