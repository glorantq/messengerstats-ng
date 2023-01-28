#include "view/statistics/impl/piechart.h"

void PieChartStatisticProvider::onPropertyUpdated(const QString& object,
                                                  const QString& name) {
    ChartStatisticProvider::onPropertyUpdated(object, name);

    if (object != m_showLabels.object() && object != m_theme.object()) {
        return;
    }

    updateDataSet(m_dataSet);
}

void PieChartStatisticProvider::updateDataSet(
    const QList<QPair<QString, long long> >& dataSet) {
    m_dataSet = dataSet;

    m_chart->removeAllSeries();
    for (const auto& axis : m_chart->axes()) {
        m_chart->removeAxis(axis);
    }

    if (dataSet.isEmpty()) {
        return;
    }

    QPieSeries* series = new QPieSeries;

    for (const auto& [label, value] : dataSet) {
        series->append(label, value);
    }

    series->setLabelsPosition(
        m_labelPosition->value<QPieSlice::LabelPosition>());
    series->setLabelsVisible(m_showLabels->toBool());

    m_chart->addSeries(series);

    m_chart->legend()->setVisible(m_showLegend->toBool());
    m_chart->legend()->setAlignment(Qt::AlignBottom);

    for (int i = 0; i < dataSet.count(); i++) {
        QPieSlice* slice = series->slices()[i];

        QString labelTemplate =
            m_showNameInLabel->toBool()
                ? tr("%1:\n %2% (%3 words)").arg(dataSet[i].first)
                : tr("%1% (%2 words)");

        slice->setLabel(labelTemplate.arg(round(100 * slice->percentage()))
                            .arg(slice->value()));

        m_chart->legend()->markers(series)[i]->setLabel(dataSet[i].first);
    }

    m_chart->setTheme(m_theme->value<QChart::ChartTheme>());
}
