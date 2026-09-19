#include "SerialPortFilter.h"

#include <QSerialPortInfo>

namespace {

bool containsNoise(const QSerialPortInfo &info)
{
    const QString blob = (info.portName() + QLatin1Char(' ')
                          + info.description() + QLatin1Char(' ')
                          + info.systemLocation())
                             .toLower();

    return blob.contains(QLatin1String("bluetooth"))
        || blob.contains(QLatin1String("debug-console"))
        || blob.contains(QLatin1String("incoming port"));
}

bool isMacTtyDuplicate(const QSerialPortInfo &info)
{
#ifdef Q_OS_MACOS
    const QString name = info.portName().toLower();
    const QString location = info.systemLocation().toLower();
    return name.startsWith(QLatin1String("tty."))
        || location.contains(QLatin1String("/tty."));
#else
    Q_UNUSED(info);
    return false;
#endif
}

bool hasKnownBoardVendor(const QSerialPortInfo &info)
{
    if (!info.hasVendorIdentifier()) {
        return false;
    }

    switch (info.vendorIdentifier()) {
    case 0x303A: // Espressif
    case 0x10C4: // Silicon Labs CP210x
    case 0x1A86: // WCH CH340
    case 0x0403: // FTDI
    case 0x2341: // Arduino
    case 0x2A03: // Arduino.org
        return true;
    default:
        return false;
    }
}

bool looksLikeUsbSerial(const QSerialPortInfo &info)
{
    const QString name = info.portName().toLower();
    const QString location = info.systemLocation().toLower();
    const QString description = info.description().toLower();
    const QString blob = name + QLatin1Char(' ') + location + QLatin1Char(' ') + description;

    if (blob.contains(QLatin1String("usbmodem"))
        || blob.contains(QLatin1String("usbserial"))
        || blob.contains(QLatin1String("ttyacm"))
        || blob.contains(QLatin1String("ttyusb"))) {
        return true;
    }

#ifdef Q_OS_WIN
    if (name.startsWith(QLatin1String("com"))
        && (hasKnownBoardVendor(info)
            || description.contains(QLatin1String("usb"))
            || description.contains(QLatin1String("serial")))) {
        return true;
    }
#endif

    return false;
}

} // namespace

namespace SerialPortFilter {

bool isLikelyBoardPort(const QSerialPortInfo &info)
{
    if (containsNoise(info) || isMacTtyDuplicate(info)) {
        return false;
    }

    return hasKnownBoardVendor(info) || looksLikeUsbSerial(info);
}

}
