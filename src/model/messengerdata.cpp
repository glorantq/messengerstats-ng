#include "model/messengerdata.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

// Loads each thread from its directory into an object and stores it
std::shared_ptr<data::MessengerData> data::MessengerData::loadFromDirectory(
    QDir directory,
    MessageClassifiers messageClassifiers) {
    QDir inboxDirectory = directory.filePath("inbox/");

    if (!inboxDirectory.exists()) {
        throw std::runtime_error("Specified directory does not exist!");
    }

    std::shared_ptr<MessengerData> object = std::make_shared<MessengerData>();

    object->m_rootDirectory = directory;

    // Try to determine the owner of this data download. If it's not possible,
    // just use a random string as a name. This doesn't really matter, as this
    // information is only used for rendering over in the view, and nickname
    // changes, which will just show this random name instead.
    // I suppose if this can't be determined we can just ask the user to give us
    // their name, but I'm still debating if it's worth it.

    QFile autofillFile = directory.filePath("autofill_information.json");
    if (autofillFile.exists()) {
        autofillFile.open(QIODevice::ReadOnly);
        QString autofillContents = QString::fromUtf8(autofillFile.readAll());
        autofillFile.close();

        autofillContents = utility::fixUnicodeEscapes(autofillContents);

        QJsonParseError parseError{};
        QJsonDocument autofillDocument =
            QJsonDocument::fromJson(autofillContents.toUtf8(), &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            throw std::runtime_error(parseError.errorString().toUtf8().data());
        }

        QJsonObject autofillInformation =
            autofillDocument["autofill_information_v2"].toObject();

        QString ownerName =
            autofillInformation["FULL_NAME"].toArray()[0].toString(
                QUuid::createUuid().toString());
        object->m_ownerIdentifier = object->getIdentifierForName(ownerName);

        qDebug() << "Data owner determined as:" << ownerName;
    } else {
        qWarning() << "Can't determine data owner, as "
                      "autofill_information.json is missing!";

        object->m_ownerIdentifier =
            object->getIdentifierForName(QUuid::createUuid().toString());
    }

    for (const auto& entry : inboxDirectory.entryList(
             QDir::Filter::NoDotAndDotDot | QDir::Filter::Dirs)) {
        QDir threadDirectory =
            inboxDirectory.filePath(QString("%1/").arg(entry));

        qDebug() << "Starting parse of:" << threadDirectory.dirName();

        std::shared_ptr<Thread> thread = std::make_shared<Thread>(
            threadDirectory, directory, messageClassifiers, object);
        object->m_threads.push_back(thread);
    }

    return object;
}

const QString data::MessengerData::getNameForUUID(
    const QUuid& identifier) const {
    return m_nameMapping.key(identifier, "");
}

const QUuid data::MessengerData::getIdentifierForName(const QString& name) {
    if (!m_nameMapping.contains(name)) {
        QUuid newIdentifier = QUuid::createUuid();
        m_nameMapping[name] = newIdentifier;

        return newIdentifier;
    }

    return m_nameMapping.value(name, {});
}
