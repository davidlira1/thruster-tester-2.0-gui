#include "SerialService.h"
#include "SerialPortFilter.h"

#include <QIODevice>
#include <QSerialPortInfo>

SerialService::SerialService(QObject *parent)
    : QObject(parent)
{
    connect(&serialPort, &QSerialPort::readyRead,
            this, &SerialService::onReadyRead);
    connect(&serialPort, &QSerialPort::errorOccurred,
            this, &SerialService::onSerialErrorOccurred);
}

bool SerialService::connectTo(const QString &portName)
{
    if (portName.isEmpty()) {
        m_lastErrorString = QStringLiteral("No serial port selected.");
        return false;
    }

    if (serialPort.isOpen()) {
        m_closing = true;
        serialPort.close();
        m_closing = false;
    }

    m_opening = true;

    QSerialPortInfo selectedPort;
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        if (info.portName() == portName) {
            selectedPort = info;
            break;
        }
    }

    serialPort.setBaudRate(m_baudRate);
    serialPort.setDataBits(QSerialPort::Data8);
    serialPort.setParity(QSerialPort::NoParity);
    serialPort.setStopBits(QSerialPort::OneStop);
    serialPort.setFlowControl(QSerialPort::NoFlowControl);

    if (!selectedPort.isNull()) {
        serialPort.setPort(selectedPort);
        if (serialPort.open(QIODevice::ReadWrite)) {
            m_opening = false;
            m_lastErrorString.clear();
            return true;
        }
    }

    serialPort.setPortName(portName);
    if (serialPort.open(QIODevice::ReadWrite)) {
        m_opening = false;
        m_lastErrorString.clear();
        return true;
    }

    m_opening = false;
    m_lastErrorString = serialPort.errorString();
    if (m_lastErrorString.isEmpty()) {
        m_lastErrorString = QStringLiteral("Failed to open serial port.");
    }
    return false;
}

void SerialService::disconnectPort()
{
    m_closing = true;
    if (serialPort.isOpen()) {
        serialPort.close();
    }
    m_closing = false;
}

bool SerialService::isConnected() const
{
    return serialPort.isOpen();
}

QString SerialService::lastErrorString() const
{
    return m_lastErrorString;
}

bool SerialService::send(const QByteArray &payload)
{
    if (!serialPort.isOpen()) {
        m_lastErrorString = QStringLiteral("Not connected to a serial port.");
        return false;
    }

    const qint64 bytesWritten = serialPort.write(payload);
    if (bytesWritten != payload.size() || !serialPort.flush()) {
        m_lastErrorString = serialPort.errorString();
        if (m_lastErrorString.isEmpty()) {
            m_lastErrorString = QStringLiteral("Failed to send serial data.");
        }
        return false;
    }

    m_lastErrorString.clear();
    return true;
}

void SerialService::onReadyRead()
{
    const QByteArray payload = serialPort.readAll();
    if (!payload.isEmpty()) {
        emit dataReceived(payload);
    }
}

void SerialService::onSerialErrorOccurred(QSerialPort::SerialPortError error)
{
    if (m_closing || m_opening || error == QSerialPort::NoError) {
        return;
    }

    m_lastErrorString = serialPort.errorString();
    if (m_lastErrorString.isEmpty()) {
        m_lastErrorString = QStringLiteral("Serial port error.");
    }

    if (serialPort.isOpen()) {
        m_closing = true;
        serialPort.close();
        m_closing = false;
    }

    emit errorOccurred(m_lastErrorString);
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
