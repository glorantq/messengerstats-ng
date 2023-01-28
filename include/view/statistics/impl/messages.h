#pragma once

#include "view/statistics/impl/barchart.h"
#include "view/statistics/impl/basic.h"
#include "view/statistics/impl/piechart.h"

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

class MonthlyWordCountStatisticProvider : public BarChartStatisticProvider,
                                          public TimeRangeStatisticProvider {
    Q_DECLARE_TR_FUNCTIONS(MonthlyWordCountStatisticProvider)

   public:
    MonthlyWordCountStatisticProvider(data::Thread* thread)
        : BarChartStatisticProvider(),
          TimeRangeStatisticProvider(
              QDateTime::fromMSecsSinceEpoch(
                  thread->getMessages().last().getTimestamp()),
              QDateTime::currentDateTime()),
          m_thread(thread) {
        m_valueLegend = tr("Words");
        m_chart->setTitle(tr("Monthly word count"));

        m_minimumWordLength = registerProperty(tr("Minimum length"), 2);
    }

   private:
    data::Thread* m_thread{};
    PropertyAccessor m_minimumWordLength{};

    // StatisticProvider interface
   public:
    void update(data::Thread*) override;
    QString getName() const override { return tr("Monthly word count"); }

   protected:
    void onPropertyUpdated(const QString& object, const QString& name) override;
};

class ConversationShareStatisticProvider : public PieChartStatisticProvider,
                                           public TimeRangeStatisticProvider {
    Q_DECLARE_TR_FUNCTIONS(ConversationShareStatisticProvider)

   public:
    ConversationShareStatisticProvider(data::Thread* thread)
        : PieChartStatisticProvider(),
          TimeRangeStatisticProvider(
              QDateTime::fromMSecsSinceEpoch(
                  thread->getMessages().last().getTimestamp()),
              QDateTime::currentDateTime()),
          m_thread(thread) {
        m_chart->setTitle(tr("Share in conversation"));

        m_minimumWordLength = registerProperty(tr("Minimum length"), 2);
    }

   private:
    data::Thread* m_thread{};
    PropertyAccessor m_minimumWordLength{};

    // StatisticProvider interface
   public:
    void update(data::Thread*) override;
    QString getName() const override { return tr("Share in conversation"); }

   protected:
    void onPropertyUpdated(const QString& object, const QString& name) override;
};
