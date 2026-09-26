#include "TestSample.h"

#include <QStringList>

std::optional<TestSample> parseTestSample(const QString &line)
{
    const QString trimmed = line.trimmed();
    if (trimmed.isEmpty()) {
        return std::nullopt;
    }

    bool hasPwm = false;
    bool hasVolts = false;
    bool hasAmps = false;
    bool hasForce = false;
    int pwm = 0;
    double volts = 0;
    double amps = 0;
    double force = 0;

    const QStringList fields = trimmed.split(QLatin1Char(','), Qt::SkipEmptyParts);
    for (const QString &field : fields) {
        const int colon = field.indexOf(QLatin1Char(':'));
        if (colon <= 0) {
            return std::nullopt;
        }

        const QString key = field.left(colon).trimmed();
        const QString value = field.mid(colon + 1).trimmed();
        bool ok = false;

        if (key == QLatin1String("PWM")) {
            pwm = value.toInt(&ok);
            hasPwm = ok;
        } else if (key == QLatin1String("VOLTS")) {
            volts = value.toDouble(&ok);
            hasVolts = ok;
        } else if (key == QLatin1String("AMPS")) {
            amps = value.toDouble(&ok);
            hasAmps = ok;
        } else if (key == QLatin1String("FORCE")) {
            force = value.toDouble(&ok);
            hasForce = ok;
        } else {
            continue;
        }

        if (!ok) {
            return std::nullopt;
        }
    }

    if (!hasPwm || !hasVolts || !hasAmps || !hasForce) {
        return std::nullopt;
    }

    TestSample sample;
    sample.pwm = pwm;
    sample.watts = volts * amps;
    sample.force = force;
    return sample;
}
