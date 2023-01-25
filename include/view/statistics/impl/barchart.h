#pragma once

#include <QCoreApplication>
#include <QWidget>
#include <QtCharts>

#include "view/statistics/impl/chart.h"
#include "view/statistics/statisticprovider.h"

class BarChartStatisticProvider : public virtual ChartStatisticProvider {
    Q_DECLARE_TR_FUNCTIONS(BarChartStatisticProvider)

   private:
    PropertyAccessor m_showCategoryAxis{};
    PropertyAccessor m_labelsAngle{};
    PropertyAccessor m_truncateLabels{};
    PropertyAccessor m_labelSize{};

    QList<QPair<QString, long long>> m_dataSet{};

    // StatisticProvider interface
   public:
    BarChartStatisticProvider() : ChartStatisticProvider() {
        m_showCategoryAxis = registerProperty(tr("Category axis"), true);
        m_labelsAngle = registerProperty(tr("Labels angle"), 45);
        m_truncateLabels = registerProperty(tr("Truncate labels"), false);
        m_labelSize =
            registerProperty("Label size", m_chart->font().pointSize());
    }

    QString getName() const override { return tr("Bar chart"); }

   protected:
    void onPropertyUpdated(const QString& object, const QString& name) override;
    void updateDataSet(const QList<QPair<QString, long long>>& dataSet);
};
