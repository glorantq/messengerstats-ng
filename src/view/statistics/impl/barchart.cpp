#include "view/statistics/impl/barchart.h"

void BarChartStatisticProvider::onPropertyUpdated(const QString& object,
                                                  const QString& name) {
    ChartStatisticProvider::onPropertyUpdated(object, name);

    if (object != m_showCategoryAxis.object() && object != m_theme.object()) {
        return;
    }

    if (name == m_showCategoryAxis.name()) {
        for (auto& axis : m_chart->axes()) {
            if (axis->inherits("QBarCategoryAxis")) {
                axis->setVisible(m_showCategoryAxis->toBool());
            }
        }
    } else if (name == m_showValueAxis.name()) {
        for (auto& axis : m_chart->axes()) {
            if (axis->inherits("QValueAxis")) {
                axis->setVisible(m_showValueAxis->toBool());
            }
        }
    } else {
        updateDataSet(m_dataSet);
    }
}

void BarChartStatisticProvider::updateDataSet(
    const QList<QPair<QString, long long>>& dataSet) {
    m_dataSet = dataSet;

    m_chart->removeAllSeries();
    for (const auto& axis : m_chart->axes()) {
        m_chart->removeAxis(axis);
    }

    if (dataSet.isEmpty()) {
        return;
    }

    QStringList categories{};
    QBarSet* data = new QBarSet(m_valueLegend);

    long long minimum = LONG_MAX;
    long long maximum = LONG_MIN;

    for (const auto& [label, value] : dataSet) {
        categories.append(label);
        data->append(value);

        minimum = qMin(minimum, value);
        maximum = qMax(maximum, value);
    }

    QBarSeries* series = new QBarSeries;
    series->append(data);

    m_chart->addSeries(series);

    m_chart->setTheme(m_theme->value<QChart::ChartTheme>());

    if (m_showCategoryAxis->toBool()) {
        QBarCategoryAxis* categoryAxis = new QBarCategoryAxis;

        categoryAxis->setLabelsVisible(true);
        categoryAxis->setTruncateLabels(m_truncateLabels->toBool());
        categoryAxis->setLabelsAngle(m_labelsAngle->toInt());

        QFont labelFont = QFont(categoryAxis->labelsFont());
        labelFont.setPointSize(m_labelSize->toInt());
        categoryAxis->setLabelsFont(labelFont);

        categoryAxis->append(categories);
        m_chart->addAxis(categoryAxis, Qt::AlignBottom);
        series->attachAxis(categoryAxis);

        categoryAxis->setRange(categories.first(), categories.last());
    }

    if (m_showValueAxis->toBool()) {
        QValueAxis* valueAxis = new QValueAxis;
        valueAxis->setRange(minimum, maximum);
        m_chart->addAxis(valueAxis, Qt::AlignLeft);
        series->attachAxis(valueAxis);
    }

    m_chart->legend()->setVisible(m_showLegend->toBool());
    m_chart->legend()->setAlignment(Qt::AlignBottom);
}
