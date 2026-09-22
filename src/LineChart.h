#pragma once

#include <QWidget>

class QChart;
class QChartView;
class QLineSeries;
class QValueAxis;

class LineChart final : public QWidget
{
    Q_OBJECT

public:
    explicit LineChart(QWidget *parent = nullptr);

    void setAxisTitles(const QString &xTitle, const QString &yTitle);
    void addPoint(double x, double y);
    void clear();
    void setXRange(double min, double max);
    void clearXRange();
    void setYRange(double min, double max);
    void clearYRange();

private:
    void updateAxisRanges();
    void applyRange(QValueAxis *axis, bool hasFixedRange, double fixedMin, double fixedMax,
                    bool hasPoints, double pointMin, double pointMax) const;

    QChart *m_chart;
    QChartView *m_chartView;
    QLineSeries *m_series;
    QValueAxis *m_xAxis;
    QValueAxis *m_yAxis;
    bool m_hasXRange = false;
    bool m_hasYRange = false;
    double m_fixedXMin = 0;
    double m_fixedXMax = 1;
    double m_fixedYMin = 0;
    double m_fixedYMax = 1;
    bool m_hasPoints = false;
    double m_pointXMin = 0;
    double m_pointXMax = 0;
    double m_pointYMin = 0;
    double m_pointYMax = 0;
};
