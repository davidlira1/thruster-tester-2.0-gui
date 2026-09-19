#include "SerialService.h"
#include "SerialPortFilter.h"

#include <QIODevice>
#include <QSerialPortInfo>

SerialService::SerialService(QObject *parent)
    : QObject(parent)
{
}

bool SerialService::connectTo(const QString &portName)
{
    if (portName.isEmpty()) {
        return false;
    }

    if (serialPort.isOpen()) {
        serialPort.close();
    }

    serialPort.setPortName(portName);
    serialPort.setBaudRate(m_baudRate);

    if (!serialPort.open(QIODevice::ReadWrite)) {
        return false;
    }

    return true;
}

void SerialService::disconnectPort()
{
    if (serialPort.isOpen()) {
        serialPort.close();
    }
}

bool SerialService::isConnected() const
{
    return serialPort.isOpen();
}

QVector<SerialService::Port> SerialService::availablePorts() const
{
    QVector<Port> ports;
    const auto infos = QSerialPortInfo::availablePorts();
    ports.reserve(infos.size());

    for (const QSerialPortInfo &info : infos) {
        if (!SerialPortFilter::isLikelyBoardPort(info)) {
            continue;
        }

        Port port;
        port.portName = info.portName();

        const QString description = info.description().trimmed();
        port.displayText = description.isEmpty()
            ? info.portName()
            : QStringLiteral("%1  %2").arg(info.portName(), description);

        ports.append(port);
    }

    return ports;
}
