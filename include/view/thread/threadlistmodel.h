#pragma once

#include <QAbstractListModel>

#include "model/thread.h"

namespace message {
enum ModelData {
    Content = Qt::UserRole,
    Author,
    Timestamp,
    Reactions,
    Type,
    RawPointer,
    CallDuration,
    AuthorIdentifier,
    OwnerIdentifier,
    Pictures,
    Attachments,
    AttachmentNames,
    SharedLink,
    Gifs,
    Sticker,
    AllImageMedia,
    VideoNames,
    AudioNames,
};
};  // namespace message

class ThreadListModel : public QAbstractListModel {
    Q_OBJECT

   private:
    // The thread supplying the actual data
    data::Thread* m_thread = nullptr;

    // Number of messages provided to the view so far
    int m_loadedSoFar = 0;

   public:
    explicit ThreadListModel(QObject* parent, data::Thread* thread)
        : QAbstractListModel(parent), m_thread(thread) {}

    // QAbstractItemModel interface
   public:
    // Returns the amount of messages loaded so far
    int rowCount(const QModelIndex& parent) const override;

    // Provides data for the view according to the roles defined above.
    // Optionally you can get the raw pointer to the message object though this,
    // but that's cheating
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

   protected:
    void fetchMore(const QModelIndex& parent) override;
    bool canFetchMore(const QModelIndex& parent) const override;
};
