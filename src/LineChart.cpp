#include "LineChart.h"

#include <QChart>
#include <QChartView>
#include <QColor>
#include <QFrame>
#include <QGraphicsLayout>
#include <QLineSeries>
#include <QPainter>
#include <QPen>
#include <QVBoxLayout>
#include <QValueAxis>
#include <algorithm>
#include <cmath>

namespace {
constexpr double EmptyAxisMin = 0.0;
constexpr double EmptyAxisMax = 1.0;
constexpr double RangePad = 0.05;
}

LineChart::LineChart(QWidget *parent)
    : QWidget(parent)
    , m_chart(new QChart())
    , m_chartView(new QChartView(m_chart, this))
    , m_series(new QLineSeries(this))
    , m_xAxis(new QValueAxis(this))
    , m_yAxis(new QValueAxis(this))
{
    const QColor background(QStringLiteral("#18242e"));
    const QColor muted(QStringLiteral("#8aa0b0"));
    const QColor grid(QStringLiteral("#223140"));

    m_chart->setBackgroundBrush(background);
    m_chart->setBackgroundPen(Qt::NoPen);
    m_chart->setPlotAreaBackgroundBrush(background);
    m_chart->setPlotAreaBackgroundVisible(true);
    m_chart->setMargins(QMargins(0, 0, 0, 0));
    m_chart->legend()->hide();
    m_chart->layout()->setContentsMargins(0, 0, 0, 0);

    QPen linePen(QColor(QStringLiteral("#0069f1")));
    linePen.setWidth(2);
    m_series->setPen(linePen);

    auto styleAxis = [&](QValueAxis *axis) {
        axis->setLabelsColor(muted);
        axis->setTitleBrush(muted);
        axis->setLinePenColor(grid);
        axis->setGridLineColor(grid);
        axis->setMinorGridLineVisible(false);
    };
    styleAxis(m_xAxis);
    styleAxis(m_yAxis);

    m_chart->addSeries(m_series);
    m_chart->addAxis(m_xAxis, Qt::AlignBottom);
    m_chart->addAxis(m_yAxis, Qt::AlignLeft);
    m_series->attachAxis(m_xAxis);
    m_series->attachAxis(m_yAxis);

    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setFrameShape(QFrame::NoFrame);
    m_chartView->setBackgroundBrush(background);
    m_chartView->setStyleSheet(QStringLiteral("background: transparent; border: none;"));

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_chartView);

    updateAxisRanges();
}

void LineChart::setAxisTitles(const QString &xTitle, const QString &yTitle)
{
    m_xAxis->setTitleText(xTitle);
    m_yAxis->setTitleText(yTitle);
}

void LineChart::addPoint(double x, double y)
{
    m_series->append(x, y);

    if (!m_hasPoints) {
        m_pointXMin = m_pointXMax = x;
        m_pointYMin = m_pointYMax = y;
        m_hasPoints = true;
    } else {
        m_pointXMin = std::min(m_pointXMin, x);
        m_pointXMax = std::max(m_pointXMax, x);
        m_pointYMin = std::min(m_pointYMin, y);
        m_pointYMax = std::max(m_pointYMax, y);
    }

    updateAxisRanges();
}

void LineChart::clear()
{
    m_series->clear();
    m_hasPoints = false;
    m_pointXMin = m_pointXMax = 0;
    m_pointYMin = m_pointYMax = 0;
    updateAxisRanges();
}

void LineChart::setXRange(double min, double max)
{
    m_hasXRange = true;
    m_fixedXMin = std::min(min, max);
    m_fixedXMax = std::max(min, max);
    updateAxisRanges();
}

void LineChart::clearXRange()
{
    m_hasXRange = false;
    updateAxisRanges();
}

void LineChart::setYRange(double min, double max)
{
    m_hasYRange = true;
    m_fixedYMin = std::min(min, max);
    m_fixedYMax = std::max(min, max);
    updateAxisRanges();
}

void LineChart::clearYRange()
{
    m_hasYRange = false;
    updateAxisRanges();
}

void LineChart::updateAxisRanges()
{
    applyRange(m_xAxis, m_hasXRange, m_fixedXMin, m_fixedXMax,
               m_hasPoints, m_pointXMin, m_pointXMax);
    applyRange(m_yAxis, m_hasYRange, m_fixedYMin, m_fixedYMax,
               m_hasPoints, m_pointYMin, m_pointYMax);
}

void LineChart::applyRange(QValueAxis *axis, bool hasFixedRange, double fixedMin, double fixedMax,
                           bool hasPoints, double pointMin, double pointMax) const
{
    if (hasFixedRange) {
        axis->setRange(fixedMin, fixedMax);
        return;
    }

    if (!hasPoints) {
        axis->setRange(EmptyAxisMin, EmptyAxisMax);
        return;
    }

    double min = pointMin;
    double max = pointMax;
    if (qFuzzyCompare(min, max)) {
        const double pad = (qFuzzyIsNull(min) ? 1.0 : std::abs(min) * RangePad);
        min -= pad;
        max += pad;
    } else {
        const double pad = (max - min) * RangePad;
        min -= pad;
        max += pad;
    }

    axis->setRange(min, max);
}
