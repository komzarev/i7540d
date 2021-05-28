#pragma once
#include <QCanBusFrame>

#include <QVector>
#include <QElapsedTimer>

class CanSerializer
{
public:
	QByteArray serialize(const QCanBusFrame& frame);
	QCanBusFrame deserialize(const QByteArray& frame);

	void addData(QByteArray&& ba);
	QVector<QCanBusFrame> frames();
    void resetData();
private:
	QByteArray buffer_;
	QVector<QCanBusFrame> frames_;
	QElapsedTimer timeStamp_;
	
};
