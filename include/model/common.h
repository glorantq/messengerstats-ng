#pragma once

#include <QString>
#include <QUuid>

#include <functional>
#include <optional>

namespace data {
// Describes a single person with their unique identifier and name
struct ThreadParticipant {
    QUuid m_identifier;
    QString m_name;

    constexpr bool operator==(const ThreadParticipant& other) const {
        return m_identifier == other.m_identifier;
    }
};

struct Nickname {
    QUuid m_performer;
    QString m_nickname;
    unsigned long long m_timestamp;
};

namespace nickname {
// The type of a nickname change's subject, sender, if the message sender
// changed their own nickname, owner, if the sender changed the nickname of
// the owner of the data download, and other, if the sender changed the
// nickname for a 3rd party
enum SubjectType {
    Sender,
    Owner,
    Other,
};

// Describes the subject with the type (see above) and their name. The name
// is only used if the type is Other
struct ChangeSubject {
    SubjectType m_subjectType;
    QString m_name;
};

// Describes a nickname change, with its subject and the actual nickname
struct ChangeParams {
    ChangeSubject m_subject;
    QString m_nickname;
};
};  // namespace nickname

typedef std::function<bool(QString)> SystemMessageClassifier;
typedef std::function<std::optional<nickname::ChangeParams>(QString)>
    NicknameChangeParser;

struct MessageClassifiers {
    SystemMessageClassifier m_systemMessageClassifier;
    NicknameChangeParser m_nicknameChangeParser;
};

namespace utility {

// This is some hackery to unescape "\u0000" sequences contained in the
// JSON files, because Qt can't do that on its own for some reason
QString fixUnicodeEscapes(const QString&);

};  // namespace utility
};  // namespace data
