#pragma once

#include <QByteArray>
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
    bool send(const QByteArray &payload);
    QString lastErrorString() const;

signals:
    void dataReceived(const QByteArray &payload);
    void errorOccurred(const QString &message);

private:
    void onReadyRead();
    void onSerialErrorOccurred(QSerialPort::SerialPortError error);

    static constexpr int DefaultBaudRate = 115200;

    QSerialPort serialPort;
    int m_baudRate = DefaultBaudRate;
    QString m_lastErrorString;
    bool m_closing = false;
    bool m_opening = false;
};
