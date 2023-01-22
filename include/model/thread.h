#pragma once

#include <QDir>
#include <QMap>
#include <QString>
#include <QUuid>

#include <optional>

#include "model/common.h"
#include "model/message.h"

namespace data {

class MessengerData;

// The type of this thread
enum ThreadType {
    UnknownThreadType,
    Regular,
    RegularGroup,
};

// A thread in which conversation takes place. Can be a group, a DM, message
// request, etc... Stores the display name, the path on disk, an optional icon
// for groups, the messages sent in it and a mapping from names to unique
// identifiers.
class Thread {
   private:
    std::shared_ptr<MessengerData> m_owner{};

    QString m_displayName;
    QString m_threadPath;

    std::optional<QString> m_threadIcon;

    ThreadType m_threadType = ThreadType::UnknownThreadType;

    QList<Message> m_messages{};
    QList<QUuid> m_participants;

    QMap<QUuid, QList<Nickname>> m_usedNicknames{};

   public:
    explicit Thread(QDir dataFolder,
                    QDir rootFolder,
                    MessageClassifiers&,
                    std::shared_ptr<MessengerData>);

    const QString& getDisplayName() const { return m_displayName; }
    const QString& getThreadPath() const { return m_threadPath; }
    const std::optional<QString>& getThreadIcon() const { return m_threadIcon; }
    const ThreadType getThreadType() const { return m_threadType; }

    const QList<ThreadParticipant> getParticipants() const;

    const QList<Message>& getMessages() const { return m_messages; }

    // These functions just delegate to the MessengerData object owning this
    // thread, so name identifiers are global accross all threads
    const QString getNameForUUID(const QUuid&) const;
    const QUuid getIdentifierForName(const QString&);

    const QUuid& getOwnerIdentifier() const;
    const ThreadParticipant getOwner() const;

    const std::shared_ptr<MessengerData> getOwnerDownload() const {
        return m_owner;
    }

    const QMap<QUuid, QList<Nickname>>& getNicknames() const {
        return m_usedNicknames;
    }
};

};  // namespace data
