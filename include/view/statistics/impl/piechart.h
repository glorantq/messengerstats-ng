#pragma once

#include <QCoreApplication>
#include <QWidget>
#include <QtCharts>

#include "view/statistics/impl/chart.h"
#include "view/statistics/statisticprovider.h"

class PieChartStatisticProvider : public virtual ChartStatisticProvider {
    Q_DECLARE_TR_FUNCTIONS(PieChartStatisticProvider)

   private:
    QList<QPair<QString, long long>> m_dataSet{};

    PropertyAccessor m_showLabels{};
    PropertyAccessor m_labelPosition{};
    PropertyAccessor m_showNameInLabel{};

    // StatisticProvider interface
   public:
    PieChartStatisticProvider() : ChartStatisticProvider() {
        m_showLabels = registerProperty(tr("Show labels"), true);
        m_labelPosition = registerProperty(
            tr("Label position"),
            QVariant::fromValue(QPieSlice::LabelPosition::LabelOutside));
        m_showNameInLabel = registerProperty(tr("Show name in label"), true);
    }

    QString getName() const override { return tr("Pie chart"); }

   protected:
    void onPropertyUpdated(const QString& object, const QString& name) override;
    void updateDataSet(const QList<QPair<QString, long long>>& dataSet);
};
