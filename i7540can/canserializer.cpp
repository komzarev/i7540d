#include "canserializer.hpp"
#include <QDateTime>

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
QByteArray CanSerializer::serialize(const QCanBusFrame& frame)
{
	/*
	All the characters are in ASCII format.
	All commands from this port must end with the character
	“<CR>” (The ASCII value is 13).

	tIIILDD…<CR> Send or receive a standard data frame.
	TIIIL<CR> Send or receive a standard remote frame.
	eIIIIIIIILDD…<CR> Send or receive an extended data frame.
	EIIIIIIIIL<CR> Send or receive an extended remote frame.
	*/

	QByteArray ba;
	const auto type = frame.frameType();
	const auto isExtended = frame.hasExtendedFrameFormat();
	const auto data = frame.payload();
	switch (type)
	{
	case QCanBusFrame::DataFrame:
		ba.append(isExtended ? 'e' : 't');
		break;
	case QCanBusFrame::RemoteRequestFrame:
		ba.append(isExtended ? 'E' : 'T');
		break;
	case QCanBusFrame::ErrorFrame:
	case QCanBusFrame::UnknownFrame:
	case QCanBusFrame::InvalidFrame:
	default:
		return ba;
	}

    if (Q_LIKELY(isExtended)) {
        ba.append(toHex(frame.frameId() & 0x1FFFFFFF,8));
	} else {
        ba.append(toHex(frame.frameId() & 0x7ff, 3));
	}

	ba.append(QByteArray::number(data.size()));
    if(Q_LIKELY(type != QCanBusFrame::RemoteRequestFrame)){
        ba.append(data.toHex());
    }
    ba.append('\r');
	return ba;
}

QCanBusFrame CanSerializer::deserialize(const QByteArray& ba)
{
	QCanBusFrame frame(QCanBusFrame::InvalidFrame);
    const int standartMinimunSize = 5;
    const int extendedMinimumSize = 10;

    int dlcPos = extendedMinimumSize - 1;
    int minimumSize = extendedMinimumSize;
    int idSize = 8;

    if (Q_UNLIKELY(ba.isEmpty())) {
		return frame;
	}

	const char type = ba.at(0);
	const bool isExtened = type == 'e' || type == 'E';
	const bool isRR = type == 'E' || type == 'T';

    if(Q_UNLIKELY(!isExtened)){
        dlcPos = standartMinimunSize - 1;
        minimumSize = standartMinimunSize;
        idSize  = 3;
    }

    if (Q_UNLIKELY(ba.size() < minimumSize)) {
		return frame;
	}

    const auto dlc = ba.mid(dlcPos, 1).toInt();

    if (Q_UNLIKELY(!isRR && ba.size() < (minimumSize + dlc))) {
		return frame;
	}


    bool ok{};
    auto t1 = ba.mid(1, idSize );
    auto t2 = t1.toULong(&ok,16);

    if(Q_UNLIKELY(!ok)){
        return frame;
    }

    //all above invalid frames
    //all bellow valid frames
    frame.setFrameId(t2);
    frame.setExtendedFrameFormat(isExtened);
    frame.setFrameType(isRR ? QCanBusFrame::RemoteRequestFrame : QCanBusFrame::DataFrame);

    if(Q_LIKELY(!isRR)){
        auto pay = ba.mid(minimumSize ,dlc*2);
        frame.setPayload(QByteArray::fromHex(pay));
    }
	frame.setTimeStamp(QCanBusFrame::TimeStamp::fromMicroSeconds(timeStamp_.elapsed()*1000));
	return frame;
}

void CanSerializer::addData(QByteArray&& ba)
{
	buffer_.append(ba);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
        auto saveLast = buffer_.back() != '\r';
#else
        auto saveLast = buffer_.at(buffer_.size() - 1) != '\r';
#endif
	auto list = buffer_.split('\r');
	if (Q_UNLIKELY(saveLast)) {
		buffer_ = list.last();
	} else {
		buffer_.clear();
	}

	for (auto v : list){
        auto f = deserialize(v);
        if(f.isValid()){
            frames_.push_back(f);
        }
	}
}

QVector<QCanBusFrame> CanSerializer::frames()
{
	decltype(frames_) tmp;
	frames_.swap(tmp);
	return tmp;
}

void CanSerializer::resetData()
{
    buffer_.clear();
    frames_.clear();
    timeStamp_.start();
}
