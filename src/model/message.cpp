#include "model/message.h"
#include "model/thread.h"

#include <QJsonArray>

// Performs parsing of the JSON representation
data::Message::Message(QJsonObject& object,
                       Thread* ownerThread,
                       MessageClassifiers& messageClassifiers,
                       QDir rootFolder) {
    QString messageType = object["type"].toString();

    if (messageType == "Generic") {
        m_type = MessageType::Generic;
    } else if (messageType == "Share") {
        m_type = MessageType::Share;
    } else if (messageType == "Call") {
        m_type = MessageType::Call;
    } else if (messageType == "Subscribe") {
        m_type = MessageType::Subscribe;
    } else if (messageType == "Unsubscribe") {
        m_type = MessageType::Unsubscribe;
    }

    if (m_type == MessageType::UnknownMessageType) {
        throw std::runtime_error(QString("Unknown message type: %1")
                                     .arg(messageType)
                                     .toUtf8()
                                     .data());
    }

    QString senderName = object["sender_name"].toString();
    m_sender = {ownerThread->getIdentifierForName(senderName), senderName};

    m_timestamp = object["timestamp_ms"].toInteger();

    if (object.contains("content")) {
        m_content = object["content"].toString();
    }

    if (object.contains("reactions")) {
        QJsonArray reactionsArray = object["reactions"].toArray();

        for (const auto& value : reactionsArray) {
            QJsonObject object = value.toObject();
            QString actor = object["actor"].toString();

            m_reactions.push_back(
                {{ownerThread->getIdentifierForName(actor), actor},
                 object["reaction"].toString()});
        }
    }

    if (object.contains("users")) {
        QJsonArray actedOnArray = object["users"].toArray();

        for (const auto& value : actedOnArray) {
            QJsonObject object = value.toObject();
            QString target = object["target"].toString();

            m_actedOn.push_back(
                {ownerThread->getIdentifierForName(target), target});
        }
    }

    m_callDuration = object["call_duration"].toInt();

    if (object.contains("photos")) {
        QJsonArray photosArray = object["photos"].toArray();
        for (const auto& value : photosArray) {
            QJsonObject object = value.toObject();

            QString storedPath = object["uri"].toString();

            m_pictures.push_back(rootFolder.filePath(
                storedPath.mid(storedPath.indexOf('/') + 1)));
        }
    }

    // TODO: Load attachments and shares

    // Determine if this should be a system message instead of what Facebook
    // tells us
    if (messageClassifiers.m_systemMessageClassifier(m_content)) {
        m_type = MessageType::ClassifiedAsSystem;
    }

    std::optional<nickname::ChangeParams> optionalNicknameChange =
        messageClassifiers.m_nicknameChangeParser(m_content);

    // Determine if this message is a nickname change
    if (optionalNicknameChange) {
        ThreadParticipant actedOn{};

        // Here, the subject is determined as precisely as possible using
        // the information given to us in the parameters
        nickname::ChangeSubject subject = optionalNicknameChange->m_subject;
        if (subject.m_subjectType == nickname::SubjectType::Sender) {
            actedOn = m_sender;
        } else if (subject.m_subjectType == nickname::SubjectType::Owner) {
            actedOn = ownerThread->getOwner();
        } else {
            actedOn = {ownerThread->getIdentifierForName(subject.m_name),
                       subject.m_name};
        }

        // We store this information in the actedOn array, as it makes sense
        // there
        m_actedOn.push_back(actedOn);
        m_setNickname = optionalNicknameChange->m_nickname;

        m_type = MessageType::NicknameChange;
    }
}
