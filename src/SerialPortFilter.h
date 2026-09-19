#pragma once

class QSerialPortInfo;

namespace SerialPortFilter {

bool isLikelyBoardPort(const QSerialPortInfo &info);

}
