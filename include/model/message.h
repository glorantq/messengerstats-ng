#pragma once

#include "model/common.h"

#include <QDir>
#include <QJsonObject>

namespace data {

// The type of the message
enum MessageType {
    UnknownMessageType,
    Generic,
    Share,
    Call,
    Subscribe,
    Unsubscribe,

    // This is not an official message type, but since Facebook tags
    // messages such as nickname changes and call joins and leaves as Generic
    // messages I made this up to not include those in word counts and to make
    // display consistent with what people expect
    ClassifiedAsSystem,

    // This is again, not an official message type, but for
    // statistics we're interested in all nickname changes. The subject is
    // stored in the actedOn array, and the nickname in m_setNickname
    NicknameChange,
};

// Forward-declare because they depend on eachother
class Thread;

// A reaction to a message, stores both the person who made the reaction
// and the reaction itself
struct Reaction {
    ThreadParticipant m_actor;
    QString m_reaction;
};

// A message in the thread. Contains the sender, timestamp, type, content,
// reactions and any attachments and extra metadata (like for thread joins the
// user who joined)
class Message {
   private:
    data::Thread* m_thread;

    ThreadParticipant m_sender;

    unsigned long long m_timestamp;
    MessageType m_type = MessageType::UnknownMessageType;

    QString m_content;
    QList<Reaction> m_reactions{};
    QList<ThreadParticipant> m_actedOn{};

    unsigned int m_callDuration;

    QList<QString> m_pictures{};
    QString m_sharedLink;
    QList<QString> m_attachments{};
    QString m_sticker;
    QList<QString> m_gifs{};
    QList<QString> m_videos{};
    QList<QString> m_audioFiles{};

    // This is an extension I added, and is not contained in the original JSON
    // object
    QString m_setNickname;

   public:
    explicit Message(QJsonObject&,
                     Thread* ownerThread,
                     MessageClassifiers&,
                     QDir rootFolder);

    const ThreadParticipant& getSender() const { return m_sender; }
    const unsigned long long getTimestamp() const { return m_timestamp; }
    const MessageType getType() const { return m_type; }

    const QString& getContent() const { return m_content; }
    const QList<Reaction>& getReactions() const { return m_reactions; }
    const QList<ThreadParticipant>& getActedOn() const { return m_actedOn; }
    const unsigned int getCallDuration() const { return m_callDuration; }
    const QString& getSetNickname() const { return m_setNickname; }
    const QList<QString>& getPictures() const { return m_pictures; }
    const QString& getSharedLink() const { return m_sharedLink; }
    const QList<QString>& getAttachments() const { return m_attachments; }
    const QString& getSticker() const { return m_sticker; }
    const QList<QString>& getGifs() const { return m_gifs; }
    const QList<QString>& getVideos() const { return m_videos; }
    const QList<QString>& getAudioFiles() const { return m_audioFiles; }

    const data::Thread* getThread() const { return m_thread; }
};

};  // namespace data
