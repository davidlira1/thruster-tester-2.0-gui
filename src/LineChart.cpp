#include "LineChart.h"

#include <QChart>
#include <QChartView>
#include <QColor>
#include <QFrame>
#include <QGraphicsLayout>
#include <QLabel>
#include <QLineSeries>
#include <QPainter>
#include <QPen>
#include <QResizeEvent>
#include <QVBoxLayout>
#include <QValueAxis>
#include <QtGlobal>
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
    , m_labelRow(new QWidget(this))
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

    m_labelRow->setFixedHeight(22);
    m_labelRow->setStyleSheet(QStringLiteral("background-color: #18242e;"));

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(m_chartView, 1);
    layout->addWidget(m_labelRow);

    connect(m_chart, &QChart::plotAreaChanged, this, [this](const QRectF &) {
        updateBottomLabels();
    });

    updateAxisRanges();
}

void LineChart::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateBottomLabels();
}

void LineChart::setAxisTitles(const QString &xTitle, const QString &yTitle)
{
    m_xAxis->setTitleText(xTitle);
    m_yAxis->setTitleText(yTitle);
}

void LineChart::addPoint(double x, double y, const QString &bottomLabel)
{
    m_series->append(x, y);
    m_bottomLabels.append(bottomLabel);

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
    m_bottomLabels.clear();
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
    updateBottomLabels();
}

void LineChart::updateBottomLabels()
{
    const auto existing = m_labelRow->findChildren<QLabel *>(Qt::FindDirectChildrenOnly);
    for (QLabel *label : existing) {
        delete label;
    }

    const QRectF plot = m_chart->plotArea();
    const double xMin = m_xAxis->min();
    const double xMax = m_xAxis->max();
    if (m_labelRow->width() <= 0 || plot.width() <= 0 || std::abs(xMax - xMin) < 1e-9) {
        return;
    }

    QVector<int> order;
    order.reserve(m_bottomLabels.size());
    const int count = std::min(static_cast<int>(m_bottomLabels.size()), m_series->count());
    for (int i = 0; i < count; ++i) {
        if (!m_bottomLabels.at(i).isEmpty()) {
            order.append(i);
        }
    }
    std::sort(order.begin(), order.end(), [this](int a, int b) {
        return m_series->at(a).x() < m_series->at(b).x();
    });

    int lastRight = -10000;
    for (int index : order) {
        const double x = m_series->at(index).x();
        const double t = (x - xMin) / (xMax - xMin);
        const double center = plot.left() + t * plot.width();

        auto *label = new QLabel(m_bottomLabels.at(index), m_labelRow);
        label->setStyleSheet(QStringLiteral(
            "color: #8aa0b0; background: transparent; font-size: 11px;"));
        label->adjustSize();

        const int left = qRound(center - label->width() / 2.0);
        const int right = left + label->width();
        if (left < 0 || right > m_labelRow->width() || left < lastRight + 4) {
            delete label;
            continue;
        }

        label->move(left, (m_labelRow->height() - label->height()) / 2);
        label->show();
        lastRight = right;
    }
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
