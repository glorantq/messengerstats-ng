#include "view/statistics/impl/messages.h"

void MonthlyMessageCountStatisticProvider::update(data::Thread* thread) {
    const QList<data::Message>& messages = thread->getMessages();

    QList<QPair<QString, long long>> dataSet = {};

    QDateTime month = m_startDateTime->toDateTime();
    int lastStop = messages.length() - 1;

    while (month <= m_endDateTime->toDateTime()) {
        long long messageCount = 0;

        for (; lastStop >= 0; lastStop--) {
            const data::Message& message = messages.at(lastStop);

            QDateTime messageTime =
                QDateTime::fromMSecsSinceEpoch(message.getTimestamp());

            if (month.date().year() != messageTime.date().year() ||
                month.date().month() != messageTime.date().month()) {
                break;
            }

            if (message.getType() == data::MessageType::Generic ||
                message.getType() == data::MessageType::Share) {
                messageCount++;
            }
        }

        dataSet.append({month.date().toString("yyyy. MMMM"), messageCount});
        month = month.addMonths(1);
    }

    updateDataSet(dataSet);
}

void MonthlyMessageCountStatisticProvider::onPropertyUpdated(
    const QString& object,
    const QString& name) {
    if (object != m_startDateTime.object()) {
        BarChartStatisticProvider::onPropertyUpdated(object, name);
        return;
    }

    update(m_thread);
}
