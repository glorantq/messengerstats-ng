#include "view/thread/threadlistmodel.h"

int ThreadListModel::rowCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : m_loadedSoFar;
}

QVariant ThreadListModel::data(const QModelIndex& index, int role) const {
    const data::Message& message = m_thread->getMessages()[index.row()];

    if (role == message::ModelData::Content) {
        return message.getContent();
    }

    if (role == message::ModelData::Author) {
        return message.getSender().m_name;
    }

    if (role == message::ModelData::Timestamp) {
        return message.getTimestamp();
    }

    if (role == message::ModelData::Reactions) {
        QString reactionsText = "";

        for (const auto& reaction : message.getReactions()) {
            if (!reactionsText.contains(reaction.m_reaction)) {
                reactionsText.append(reaction.m_reaction);
            }
        }

        return reactionsText;
    }

    if (role == message::ModelData::RawPointer) {
        return (unsigned long long)&message;
    }

    if (role == message::ModelData::Type) {
        return (int)message.getType();
    }

    if (role == message::ModelData::CallDuration) {
        return message.getCallDuration();
    }

    if (role == message::ModelData::AuthorIdentifier) {
        return message.getSender().m_identifier;
    }

    if (role == message::ModelData::OwnerIdentifier) {
        return m_thread->getOwnerIdentifier();
    }

    if (role == message::ModelData::Pictures) {
        return message.getPictures();
    }

    if (role == message::ModelData::Attachments) {
        return message.getAttachments();
    }

    if (role == message::ModelData::SharedLink) {
        return message.getSharedLink();
    }

    if (role == message::ModelData::Gifs) {
        return message.getGifs();
    }

    if (role == message::ModelData::Sticker) {
        return message.getSticker();
    }

    if (role == message::ModelData::AllImageMedia) {
        QString sticker = message.getSticker();
        return message.getPictures() + message.getGifs() +
               (sticker.isEmpty() ? QStringList{} : QStringList{sticker});
    }

    auto getFileNames = [](const QList<QString>& paths) {
        QStringList names;

        for (const auto& path : paths) {
            names.push_back(QFileInfo(path).fileName());
        }

        return names;
    };

    if (role == message::ModelData::AttachmentNames) {
        return getFileNames(message.getAttachments());
    }

    if (role == message::ModelData::VideoNames) {
        return getFileNames(message.getVideos());
    }

    if (role == message::ModelData::AudioNames) {
        return getFileNames(message.getAudioFiles());
    }

    return {};
}

// If the need for a QML frontend arises sometime in the late future
QHash<int, QByteArray> ThreadListModel::roleNames() const {
    return {
        {message::ModelData::Content, "content"},
        {message::ModelData::Author, "sender"},
        {message::ModelData::Timestamp, "timestamp"},
        {message::ModelData::Reactions, "reactions"},
        {message::ModelData::Type, "type"},
        {message::ModelData::RawPointer, "message_ptr"},
        {message::ModelData::CallDuration, "call_duration"},
        {message::ModelData::AuthorIdentifier, "sender_identifier"},
        {message::ModelData::OwnerIdentifier, "owner_identifier"},
        {message::ModelData::Pictures, "pictures"},
        {message::ModelData::AttachmentNames, "attachment_names"},
        {message::ModelData::Attachments, "attachments"},
        {message::ModelData::SharedLink, "shared_link"},
        {message::ModelData::Gifs, "gifs"},
        {message::ModelData::Sticker, "sticker"},
        {message::ModelData::AllImageMedia, "all_image_media"},
        {message::ModelData::VideoNames, "video_names"},
        {message::ModelData::AudioNames, "audio_names"},
    };
}

// Performs the actualy lazy-loading of the messages; if there are still
// messages that haven't been fetched yet, it's done here in chunks of 25 (or if
// there are less then 25 all the remaining messages)
void ThreadListModel::fetchMore(const QModelIndex& parent) {
    if (parent.isValid()) {
        return;
    }

    int toFetch =
        qMin<int>(25, m_thread->getMessages().count() - m_loadedSoFar);

    if (toFetch <= 0) {
        return;
    }

    beginInsertRows(QModelIndex(), m_loadedSoFar, m_loadedSoFar + toFetch - 1);
    m_loadedSoFar += toFetch;
    endInsertRows();
}

// Tells the view whether all messages have been loaded or not
bool ThreadListModel::canFetchMore(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return false;
    }

    return m_loadedSoFar < m_thread->getMessages().count();
}
