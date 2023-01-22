#include "view/thread/searchdialog.h"
#include "./ui_searchdialog.h"

#include <QMenu>
#include <QPainter>

#include "model/message.h"

SearchDialog::SearchDialog(QWidget* parent,
                           const QList<const data::Message*>& messages)
    : QDialog(parent), ui(new Ui::SearchDialog), m_messages(messages) {
    ui->setupUi(this);

    setWindowTitle(windowTitle().arg(QCoreApplication::applicationName()));

    QPalette listPalette = palette();
    listPalette.setColor(QPalette::Highlight,
                         palette().color(QPalette::AlternateBase));
    listPalette.setColor(QPalette::Base, palette().color(QPalette::Window));
    listPalette.setColor(QPalette::AlternateBase,
                         palette().color(QPalette::Window));
    ui->messagesList->setPalette(listPalette);

    m_resultDelegate = new SearchResultDelegate(ui->messagesList);
    ui->messagesList->setItemDelegate(m_resultDelegate);
    ui->messagesList->setContextMenuPolicy(
        Qt::ContextMenuPolicy::CustomContextMenu);

    matchGroupBoxPattern = ui->matchesGroupBox->title();
    ui->matchesGroupBox->setTitle(matchGroupBoxPattern.arg(0));
}

SearchDialog::~SearchDialog() {
    delete m_resultDelegate;
    delete ui;
}

void SearchDialog::on_messagesList_customContextMenuRequested(
    const QPoint& position) {
    auto selectedItems = ui->messagesList->selectionModel()->selectedRows();
    if (selectedItems.count() == 0) {
        return;
    }

    QModelIndex selected = selectedItems.first();

    data::Message* messagePointer =
        (data::Message*)selected.data(SearchModelData::MessagePointer)
            .toULongLong();

    int index = selected.data(SearchModelData::MessageIndex).toInt();

    QMenu contextMenu(this);
    QAction showInformationAction(
        QIcon("://resources/icon/silk/information.png"), tr("Show information"),
        &contextMenu);

    connect(&showInformationAction, &QAction::triggered,
            [=]() { emit onOpenMessageInformation(messagePointer); });

    QAction scrollToAction(QIcon("://resources/icon/silk/link_go.png"),
                           tr("View in chat"), &contextMenu);

    connect(&scrollToAction, &QAction::triggered,
            [=]() { emit onScrollToMessageIndex(index); });

    contextMenu.addAction(&showInformationAction);
    contextMenu.addAction(&scrollToAction);

    QPoint globalPos = ui->messagesList->mapToGlobal(position);
    contextMenu.exec(globalPos);
}

void SearchDialog::on_messagesList_itemDoubleClicked(QListWidgetItem* item) {
    if (item != nullptr) {
        int index = item->data(SearchModelData::MessageIndex).toInt();
        emit onScrollToMessageIndex(index);
    }
}

void SearchDialog::on_buttonBox_rejected() {
    close();
}

void SearchDialog::on_searchButton_pressed() {
    QString rawPattern = ui->patternLineEdit->text();

    if (rawPattern.trimmed().isEmpty()) {
        return;
    }

    bool isCaseSensitive = ui->matchCaseCheckbox->isChecked();
    bool isRegularExpression = ui->regexCheckBox->isChecked();

    QRegularExpression::PatternOptions patternOptions =
        QRegularExpression::UseUnicodePropertiesOption;

    if (!isCaseSensitive) {
        patternOptions |= QRegularExpression::CaseInsensitiveOption;
    }

    QRegularExpression pattern;

    if (isRegularExpression) {
        pattern = QRegularExpression(rawPattern, patternOptions);
    } else {
        QString literal = QRegularExpression::escape(rawPattern);
        pattern =
            QRegularExpression(QString("%1").arg(literal), patternOptions);
    }

    performSearch(pattern);
}

void SearchDialog::performSearch(const QRegularExpression& regularExpression) {
    ui->messagesList->clear();

    for (int i = 0; i < m_messages.count(); i++) {
        const data::Message* message = m_messages[i];

        QRegularExpressionMatch match =
            regularExpression.match(message->getContent());

        if (match.hasMatch()) {
            QListWidgetItem* item = new QListWidgetItem(ui->messagesList);
            item->setData(SearchModelData::MessageIndex, i);
            item->setData(SearchModelData::MessageContent,
                          message->getContent());
            item->setData(SearchModelData::MatchStart, match.capturedStart());
            item->setData(SearchModelData::MatchEnd, match.capturedEnd());
            item->setData(SearchModelData::MatchLength, match.capturedLength());
            item->setData(SearchModelData::SenderName,
                          message->getSender().m_name);
            item->setData(SearchModelData::Timestamp, message->getTimestamp());
            item->setData(SearchModelData::MessagePointer,
                          (unsigned long long)message);

            ui->messagesList->addItem(item);
        }
    }

    ui->matchesGroupBox->setTitle(
        matchGroupBoxPattern.arg(ui->messagesList->count()));
}

void SearchResultDelegate::paint(QPainter* painter,
                                 const QStyleOptionViewItem& option,
                                 const QModelIndex& index) const {
    QStyleOptionViewItem opt(option);
    painter->save();
    painter->setRenderHints(QPainter::RenderHint::Antialiasing |
                            QPainter::RenderHint::TextAntialiasing);

    QStyledItemDelegate::paint(painter, opt, index);

    opt.font.setHintingPreference(QFont::PreferNoHinting);
    opt.fontMetrics = QFontMetrics(opt.font);

    painter->setFont(opt.font);

    QFont boldFont(opt.font);
    boldFont.setBold(true);

    QFont smallFont(opt.font);
    smallFont.setPointSize(opt.font.pointSize() - 2);

    QString senderName = index.data(SearchModelData::SenderName).toString();
    unsigned long long timestamp =
        index.data(SearchModelData::Timestamp).toULongLong();

    QString messageContent =
        index.data(SearchModelData::MessageContent).toString();
    int matchStart = index.data(SearchModelData::MatchStart).toInt();
    int matchEnd = index.data(SearchModelData::MatchEnd).toInt();
    int matchLength = index.data(SearchModelData::MatchLength).toInt();

    bool elideStart = matchStart - 20 > 0;
    bool elideEnd = matchEnd + 20 < messageContent.length();

    int availableBeforeMatch = matchStart >= 20 ? 20 : matchStart;

    QString beforeMatch = messageContent.mid(matchStart - availableBeforeMatch,
                                             availableBeforeMatch);
    QString afterMatch = messageContent.mid(matchEnd, 20);
    QString match = messageContent.mid(matchStart, matchLength);

    QRect beforeSize = opt.fontMetrics.boundingRect(
        opt.rect, Qt::AlignVCenter | Qt::AlignLeft | Qt::TextSingleLine,
        beforeMatch);

    QRect matchSize = QFontMetrics(boldFont).boundingRect(
        opt.rect, Qt::AlignVCenter | Qt::AlignLeft | Qt::TextSingleLine, match);

    QRect afterSize = opt.fontMetrics.boundingRect(
        opt.rect, Qt::AlignVCenter | Qt::AlignLeft | Qt::TextSingleLine,
        afterMatch);

    QRect elideSize = opt.fontMetrics.boundingRect(
        opt.rect, Qt::AlignVCenter | Qt::AlignLeft | Qt::TextSingleLine, "...");

    painter->setFont(opt.font);

    if (elideStart) {
        painter->drawText(opt.rect, Qt::AlignVCenter | Qt::AlignLeft, "...");
        opt.rect.moveLeft(opt.rect.left() + elideSize.width());
    }

    painter->drawText(opt.rect,
                      Qt::AlignVCenter | Qt::AlignLeft | Qt::TextSingleLine,
                      beforeMatch);
    opt.rect.moveLeft(opt.rect.left() + beforeSize.width());

    painter->setFont(boldFont);
    painter->drawText(
        opt.rect, Qt::AlignVCenter | Qt::AlignLeft | Qt::TextSingleLine, match);
    opt.rect.moveLeft(opt.rect.left() + matchSize.width());
    painter->setFont(opt.font);

    painter->drawText(opt.rect,
                      Qt::AlignVCenter | Qt::AlignLeft | Qt::TextSingleLine,
                      afterMatch);
    opt.rect.moveLeft(opt.rect.left() + afterSize.width());

    if (elideEnd) {
        painter->drawText(opt.rect,
                          Qt::AlignVCenter | Qt::AlignLeft | Qt::TextSingleLine,
                          "...");
        opt.rect.moveLeft(opt.rect.left() + elideSize.width());
    }

    opt.rect.moveLeft(opt.rect.left() + opt.font.pointSize());

    QString extraText = formatExtraText(senderName, timestamp);

    painter->setFont(smallFont);
    painter->setPen(m_disabledColor);
    painter->drawText(opt.rect,
                      Qt::AlignVCenter | Qt::AlignLeft | Qt::TextSingleLine,
                      extraText);

    painter->restore();
}

QSize SearchResultDelegate::sizeHint(const QStyleOptionViewItem& option,
                                     const QModelIndex& index) const {
    QStyleOptionViewItem opt(option);

    opt.font.setHintingPreference(QFont::PreferNoHinting);
    opt.fontMetrics = QFontMetrics(opt.font);

    QFont boldFont(opt.font);
    boldFont.setBold(true);

    QFont smallFont(opt.font);
    smallFont.setPointSize(opt.font.pointSize() - 2);

    QString senderName = index.data(SearchModelData::SenderName).toString();
    unsigned long long timestamp =
        index.data(SearchModelData::Timestamp).toULongLong();

    QString messageContent =
        index.data(SearchModelData::MessageContent).toString();
    int matchStart = index.data(SearchModelData::MatchStart).toInt();
    int matchEnd = index.data(SearchModelData::MatchEnd).toInt();
    int matchLength = index.data(SearchModelData::MatchLength).toInt();

    bool elideStart = matchStart - 20 > 0;
    bool elideEnd = matchEnd + 20 < messageContent.length();

    int availableBeforeMatch = matchStart > 20 ? 20 : matchStart;

    QString beforeMatch = messageContent.mid(matchStart - availableBeforeMatch,
                                             availableBeforeMatch);
    QString afterMatch = messageContent.mid(matchEnd, 20);
    QString match = messageContent.mid(matchStart, matchLength);

    QRect beforeSize = opt.fontMetrics.boundingRect(
        opt.rect, Qt::AlignVCenter | Qt::AlignLeft | Qt::TextSingleLine,
        beforeMatch);

    QRect matchSize = QFontMetrics(boldFont).boundingRect(
        opt.rect, Qt::AlignVCenter | Qt::AlignLeft | Qt::TextSingleLine, match);

    QRect afterSize = opt.fontMetrics.boundingRect(
        opt.rect, Qt::AlignVCenter | Qt::AlignLeft | Qt::TextSingleLine,
        afterMatch);

    QRect elideSize = opt.fontMetrics.boundingRect(
        opt.rect, Qt::AlignVCenter | Qt::AlignLeft | Qt::TextSingleLine, "...");

    QString extraText = formatExtraText(senderName, timestamp);

    QRect extraSize = QFontMetrics(smallFont).boundingRect(
        opt.rect, Qt::AlignVCenter | Qt::AlignLeft | Qt::TextSingleLine,
        extraText);

    return {(elideStart ? elideSize.width() : 0) + beforeSize.width() +
                matchSize.width() + afterSize.width() + elideSize.width() +
                (elideEnd ? elideSize.width() : 0) + opt.font.pointSize() +
                extraSize.width(),
            matchSize.height() + opt.font.pointSize() / 2};
}

QString SearchResultDelegate::formatExtraText(
    const QString& senderName,
    unsigned long long timestamp) const {
    return QString("(%1, %2)")
        .arg(senderName)
        .arg(QDateTime::fromMSecsSinceEpoch(timestamp).toString(
            tr("yyyy. MM. dd. hh:mm:ss")));
}
