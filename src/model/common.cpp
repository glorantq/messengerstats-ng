#include "model/common.h"

#include <QMap>
#include <QRegularExpression>

#include <stdexcept>

QString data::utility::fixUnicodeEscapes(const QString& input) {
    QString output = input;

    QMap<QString, QString> unicodeMapping{};

    QByteArray currentBuffer{};
    QString currentString{};
    int sequenceLength = 0;

    QRegularExpression rx("(\\\\u[0-9a-fA-F]{4})");
    QRegularExpressionMatchIterator regexIterator = rx.globalMatch(input);

    while (regexIterator.hasNext()) {
        QRegularExpressionMatch match = regexIterator.next();
        unsigned char unicodeValue =
            match.captured(1).right(4).toUShort(nullptr, 16);

        if (sequenceLength == 0) {
            if (0 <= unicodeValue && unicodeValue <= 0x7F) {
                sequenceLength = 1;
            } else if (0xC2 <= unicodeValue && unicodeValue <= 0xDF) {
                sequenceLength = 2;
            } else if (0xE0 <= unicodeValue && unicodeValue <= 0xEF) {
                sequenceLength = 3;
            } else if (0xF0 <= unicodeValue && unicodeValue <= 0xFF) {
                sequenceLength = 4;
            } else {
                throw std::runtime_error(
                    "Invalid starting byte in UTF-8 sequence!");
            }
        }

        currentString.append(match.captured(1));
        currentBuffer.append(unicodeValue);

        if (currentBuffer.size() == sequenceLength) {
            unicodeMapping[currentString] = QString::fromUtf8(currentBuffer);

            currentString = "";
            currentBuffer.clear();
            sequenceLength = 0;
        }
    }

    for (const auto& key : unicodeMapping.keys()) {
        output.replace(key, unicodeMapping[key]);
    }

    return output;
}
