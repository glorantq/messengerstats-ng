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

void MonthlyWordCountStatisticProvider::update(data::Thread* thread) {
    const QList<data::Message>& messages = thread->getMessages();

    QList<QPair<QString, long long>> dataSet = {};

    QDateTime month = m_startDateTime->toDateTime();
    int lastStop = messages.length() - 1;

    while (month <= m_endDateTime->toDateTime()) {
        long long wordCount = 0;

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
                const auto& words = message.getContent().split(
                    " ", Qt::SplitBehaviorFlags::SkipEmptyParts);

                for (const auto& word : words) {
                    if (word.length() >= m_minimumWordLength->toInt()) {
                        wordCount++;
                    }
                }
            }
        }

        dataSet.append({month.date().toString("yyyy. MMMM"), wordCount});
        month = month.addMonths(1);
    }

    updateDataSet(dataSet);
}

void MonthlyWordCountStatisticProvider::onPropertyUpdated(const QString& object,
                                                          const QString& name) {
    if (object != m_startDateTime.object() &&
        object != m_minimumWordLength.object()) {
        BarChartStatisticProvider::onPropertyUpdated(object, name);
        return;
    }

    update(m_thread);
}

void ConversationShareStatisticProvider::update(data::Thread* thread) {
    const QList<data::Message>& messages = thread->getMessages();

    QList<QPair<QString, long long>> dataSet = {};
    QMap<QString, long long> rawDataSet = {};

    for (const auto& message : messages) {
        QDateTime messageTime =
            QDateTime::fromMSecsSinceEpoch(message.getTimestamp());

        if (messageTime < m_startDateTime->toDateTime() ||
            messageTime > m_endDateTime->toDateTime()) {
            continue;
        }

        if (message.getType() == data::MessageType::Generic ||
            message.getType() == data::MessageType::Share) {
            const auto& words = message.getContent().split(
                " ", Qt::SplitBehaviorFlags::SkipEmptyParts);

            long long wordCount = message.getPictures().count() +
                                  message.getVideos().count() +
                                  message.getAttachments().count() +
                                  message.getAudioFiles().count();

            for (const auto& word : words) {
                if (word.length() >= m_minimumWordLength->toInt()) {
                    wordCount++;
                }
            }

            QString sender = message.getSender().m_name;
            long long currentValue = rawDataSet.value(sender, 0);

            rawDataSet[sender] = currentValue + 1;
        }
    }

    for (const auto& key : rawDataSet.keys()) {
        dataSet.append({key, rawDataSet[key]});
    }

    updateDataSet(dataSet);
}

void ConversationShareStatisticProvider::onPropertyUpdated(
    const QString& object,
    const QString& name) {
    if (object != m_startDateTime.object() &&
        object != m_minimumWordLength.object()) {
        PieChartStatisticProvider::onPropertyUpdated(object, name);
        return;
    }

    update(m_thread);
}
