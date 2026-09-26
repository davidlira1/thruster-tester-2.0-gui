#pragma once

#include <QString>
#include <optional>

struct TestSample {
    int pwm = 0;
    double watts = 0;
    double force = 0;
};

std::optional<TestSample> parseTestSample(const QString &line);
