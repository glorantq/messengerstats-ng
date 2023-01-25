#pragma once

#include "view/statistics/impl/barchart.h"
#include "view/statistics/impl/basic.h"

#include <QCoreApplication>

class MonthlyMessageCountStatisticProvider : public BarChartStatisticProvider,
                                             public TimeRangeStatisticProvider {
    Q_DECLARE_TR_FUNCTIONS(MonthlyMessageCountStatisticProvider)

   public:
    MonthlyMessageCountStatisticProvider(data::Thread* thread)
        : BarChartStatisticProvider(),
          TimeRangeStatisticProvider(
              QDateTime::fromMSecsSinceEpoch(
                  thread->getMessages().last().getTimestamp()),
              QDateTime::currentDateTime()),
          m_thread(thread) {
        m_valueLegend = tr("Messages");
        m_chart->setTitle(tr("Monthly message count"));
    }

   private:
    data::Thread* m_thread{};

    // StatisticProvider interface
   public:
    void update(data::Thread*) override;
    QString getName() const override { return tr("Monthly message count"); }

   protected:
    void onPropertyUpdated(const QString& object, const QString& name) override;
};
