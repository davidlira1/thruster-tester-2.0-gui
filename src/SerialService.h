#pragma once

#include <QObject>
#include <QSerialPort>
#include <QString>
#include <QVector>

class SerialService final : public QObject
{
    Q_OBJECT

public:
    struct Port {
        QString portName;
        QString displayText;
    };

    explicit SerialService(QObject *parent = nullptr);

    QVector<Port> availablePorts() const;
    bool connectTo(const QString &portName);
    void disconnectPort();
    bool isConnected() const;

private:
    static constexpr int DefaultBaudRate = 115200;

    QSerialPort serialPort;
    int m_baudRate = DefaultBaudRate;
};
