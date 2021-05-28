#include "i7540canbackend.hpp"

#include <QCanBus>
#include <QCanBusDevice>
#include <QCanBusFactory>

class I7540CanBusPlugin : public QObject, public QCanBusFactoryV2
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QCanBusFactory" FILE "plugin.json")
    Q_INTERFACES(QCanBusFactoryV2)

public:
    QList<QCanBusDeviceInfo> availableDevices(QString *errorMessage) const override
    {
        Q_UNUSED(errorMessage)
        return I7540CanBackend::interfaces();
    }

    QCanBusDevice *createDevice(const QString &interfaceName, QString *errorMessage) const override
    {
        Q_UNUSED(errorMessage);
        return new I7540CanBackend(interfaceName);
    }
};

#include "main.moc"
