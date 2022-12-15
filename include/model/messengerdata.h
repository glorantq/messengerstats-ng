#pragma once

#include "model/thread.h"

#include <QDir>
#include <QList>

#include <memory>

namespace data {

// The holder for everything the model provides.
class MessengerData {
   private:
    // Map that holds all names and their associated identifiers accross this
    // data download
    QMap<QString, QUuid> m_nameMapping{};

    // The identifier of the person who owns this data download, if it could be
    // determined
    QUuid m_ownerIdentifier{};

    // All threads contained in this data download
    QList<std::shared_ptr<Thread>> m_threads{};

    QDir m_rootDirectory;

   public:
    explicit MessengerData() {}
    MessengerData(const MessengerData&) {}

    // Method to load a GDPR download directory into a MessengerData object
    static std::shared_ptr<MessengerData> loadFromDirectory(
        QDir directory,
        MessageClassifiers,
        std::function<void(int, int)> progressRangeCallback = [](int, int) {},
        std::function<void(int, const QString&)> progressCallback =
            [](int, const QString&) {});

    const QList<std::shared_ptr<Thread>>& getThreads() const {
        return m_threads;
    }

    const QString getNameForUUID(const QUuid&) const;
    const QUuid getIdentifierForName(const QString&);

    const QUuid& getOwnerIdentifier() const { return m_ownerIdentifier; }
    const ThreadParticipant getOwner() const {
        return {m_ownerIdentifier, getNameForUUID(m_ownerIdentifier)};
    }

    const QDir& getRootDirectory() const { return m_rootDirectory; }
};

};  // namespace data
