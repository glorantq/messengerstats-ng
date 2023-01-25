#pragma once

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
