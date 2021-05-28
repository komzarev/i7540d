#ifndef SYSTECCANBACKEND_H
#define SYSTECCANBACKEND_H

#include <QCanBusFrame>
#include <QCanBusDevice>
#include <QCanBusDeviceInfo>

#include <QTimer>
#include <QVariant>
#include <QList>

#include "cancontroller.hpp"
#include "caninterface.hpp"


class I7540CanBackend : public QCanBusDevice
{
    Q_OBJECT
    Q_DISABLE_COPY(I7540CanBackend)
public:
    explicit I7540CanBackend(const QString& name, QObject* parent = nullptr);
    ~I7540CanBackend();

    bool open() override;
    void close() override;

    void setConfigurationParameter(int key, const QVariant &value) override;

    bool writeFrame(const QCanBusFrame &newData) override;

    QString interpretErrorFrame(const QCanBusFrame& errorFrame) override;

    static QList<QCanBusDeviceInfo> interfaces();
    static bool canCreate(QString *errorReason);
protected slots:
    void onFrameRecieved();
    void onStatusChanged(bool isConnected);
private:
    void startWrite();
    bool verifyBitRate(int bitrate);
    int getBatchSize(int baudRate);
    CanController* control_;
    CanInterface *iface_;

    QTimer outGoingTimer_;
    QString name_;
    int batchSize_;
    bool isRunning(){return isRunning_;}
    bool isRunning_ = false;
    QHostAddress addr_;

};


#endif // SYSTECCANBACKEND_H
