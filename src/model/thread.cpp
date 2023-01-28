#include "model/thread.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QRegularExpression>

#include <algorithm>
#include <exception>

#include "model/messengerdata.h"

// Performs parsing for a thread. Loads all message_*.json files in order.
data::Thread::Thread(QDir dataFolder,
                     QDir rootFolder,
                     MessageClassifiers& messageClassifiers,
                     std::shared_ptr<MessengerData> owner)
    : m_owner(owner) {
    for (const auto& messageFileName : dataFolder.entryList(
             {"message_*.json"}, QDir::Filter::Files, QDir::SortFlag::Name)) {
        QFile messageFile = dataFolder.filePath(messageFileName);

        messageFile.open(QIODevice::ReadOnly);
        QString fileContents = QString::fromUtf8(messageFile.readAll());
        messageFile.close();

        fileContents = utility::fixUnicodeEscapes(fileContents);

        QJsonParseError parseError{};
        QJsonDocument messageDocument =
            QJsonDocument::fromJson(fileContents.toUtf8(), &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            throw RuntimeError(QString("Failed to parse message document: %1")
                                   .arg(parseError.errorString()));
        }

        QString threadTypeString = messageDocument["thread_type"].toString();
        if (threadTypeString == "Regular") {
            m_threadType = ThreadType::Regular;
        } else if (threadTypeString == "RegularGroup") {
            m_threadType = ThreadType::RegularGroup;
        }

        m_threadPath = messageDocument["thread_path"].toString();
        m_displayName = messageDocument["title"].toString(dataFolder.dirName());

        if (messageDocument.object().contains("image")) {
            QString storedPath =
                messageDocument["image"].toObject()["uri"].toString();

            storedPath = storedPath.mid(storedPath.indexOf('/') + 1);

            m_threadIcon = rootFolder.filePath(storedPath);
        }

        QJsonArray participantsArray =
            messageDocument["participants"].toArray();

        for (const auto& value : participantsArray) {
            QString name = value.toObject()["name"].toString();
            QUuid identifier = owner->getIdentifierForName(name);

            if (!m_participants.contains(identifier)) {
                m_participants.push_back(identifier);
            }
        }

        QJsonArray messagesArray = messageDocument["messages"].toArray();

        for (const auto& value : messagesArray) {
            QJsonObject object = value.toObject();

            m_messages.push_back(
                Message(object, this, messageClassifiers, rootFolder));
        }

        std::sort(m_messages.begin(), m_messages.end(),
                  [](const auto& a, const auto& b) constexpr {
                      return a.getTimestamp() > b.getTimestamp();
                  });
    }

    if (m_displayName.isEmpty()) {
        m_displayName = dataFolder.dirName();
    }

    for (const auto& message : m_messages) {
        if (message.getType() != MessageType::NicknameChange) {
            continue;
        }

        ThreadParticipant subject = message.getActedOn().first();
        QString nickname = message.getSetNickname();

        m_usedNicknames[subject.m_identifier].push_back(
            {message.getSender().m_identifier, nickname,
             message.getTimestamp()});
    }

    // Guess the thread type
    if (m_threadType == ThreadType::UnknownThreadType) {
        if (m_participants.count() > 2) {
            m_threadType = ThreadType::RegularGroup;
        } else {
            bool m_nameMatchesPerson = false;

            for (const auto& id : m_participants) {
                m_nameMatchesPerson =
                    m_nameMatchesPerson ||
                    m_displayName == owner->getNameForUUID(id);
            }

            if (m_nameMatchesPerson) {
                m_threadType = ThreadType::Regular;
            } else {
                m_threadType = ThreadType::RegularGroup;
            }
        }
    }
}

const QList<data::ThreadParticipant> data::Thread::getParticipants() const {
    QList<ThreadParticipant> value{};

    for (const auto& id : m_participants) {
        value.push_back({id, m_owner->getNameForUUID(id)});
    }

    return value;
}

const QString data::Thread::getNameForUUID(const QUuid& identifier) const {
    return m_owner->getNameForUUID(identifier);
}

const QUuid data::Thread::getIdentifierForName(const QString& name) {
    return m_owner->getIdentifierForName(name);
}

const QUuid& data::Thread::getOwnerIdentifier() const {
    return m_owner->getOwnerIdentifier();
}

const data::ThreadParticipant data::Thread::getOwner() const {
    return m_owner->getOwner();
}
