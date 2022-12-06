#pragma once

#include "model/common.h"

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
    ThreadParticipant m_sender;

    unsigned long long m_timestamp;
    MessageType m_type = MessageType::UnknownMessageType;

    QString m_content;
    QList<Reaction> m_reactions{};
    QList<ThreadParticipant> m_actedOn{};

    unsigned int m_callDuration;

    // This is an extension I added, and is not contained in the original JSON
    // object
    QString m_setNickname;

   public:
    explicit Message(QJsonObject&, Thread* ownerThread, MessageClassifiers&);

    const ThreadParticipant& getSender() const { return m_sender; }
    const unsigned long long getTimestamp() const { return m_timestamp; }
    const MessageType getType() const { return m_type; }

    const QString& getContent() const { return m_content; }
    const QList<Reaction>& getReactions() const { return m_reactions; }
    const QList<ThreadParticipant>& getActedOn() const { return m_actedOn; }
    const unsigned int getCallDuration() const { return m_callDuration; }
    const QString& getSetNickname() const { return m_setNickname; }
};

};  // namespace data
