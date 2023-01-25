#pragma once

#include "view/statistics/impl/barchart.h"
#include "view/statistics/statisticprovider.h"

#include <QCoreApplication>
#include <QDateTime>

class TimeRangeStatisticProvider : public virtual StatisticProvider {
    Q_DECLARE_TR_FUNCTIONS(TimeRangeStatisticProvider)

   protected:
    PropertyAccessor m_startDateTime{};
    PropertyAccessor m_endDateTime{};

   public:
    TimeRangeStatisticProvider(const QDateTime& begin, const QDateTime& end)
        : StatisticProvider() {
        m_startDateTime = registerProperty(tr("Start"), begin);
        m_endDateTime = registerProperty(tr("End"), end);
    }

    TimeRangeStatisticProvider()
        : TimeRangeStatisticProvider(QDateTime::currentDateTime(),
                                     QDateTime::currentDateTime()) {}

    // StatisticProvider interface
   public:
    QString getName() const override { return tr("Time range"); }
};

class TestingStatisticsProvider : public TimeRangeStatisticProvider,
                                  public BarChartStatisticProvider {
    // StatisticProvider interface
   public:
    TestingStatisticsProvider()
        : TimeRangeStatisticProvider(), BarChartStatisticProvider() {
        m_valueLegend = "Messages";
        m_chart->setTitle("Monthly message count");
    }

    QString getName() const override { return "TestingStatisticsProvider"; }
    void update(data::Thread*) override {
        QList<QPair<QString, long long>> data{};

        for (int i = 0; i < 50; i++) {
            data.append({QString("Value %1").arg(i), i});
        }

        updateDataSet(data);
    }
};
