#pragma once

#include <QString>
#include <QVector>
#include <QWidget>

class QChart;
class QChartView;
class QLineSeries;
class QResizeEvent;
class QValueAxis;

class LineChart final : public QWidget
{
    Q_OBJECT

public:
    explicit LineChart(QWidget *parent = nullptr);

    void setAxisTitles(const QString &xTitle, const QString &yTitle);
    void addPoint(double x, double y, const QString &bottomLabel = {});
    void clear();
    void setXRange(double min, double max);
    void clearXRange();
    void setYRange(double min, double max);
    void clearYRange();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void updateAxisRanges();
    void updateBottomLabels();
    void applyRange(QValueAxis *axis, bool hasFixedRange, double fixedMin, double fixedMax,
                    bool hasPoints, double pointMin, double pointMax) const;

    QChart *m_chart;
    QChartView *m_chartView;
    QWidget *m_labelRow;
    QLineSeries *m_series;
    QValueAxis *m_xAxis;
    QValueAxis *m_yAxis;
    QVector<QString> m_bottomLabels;
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
