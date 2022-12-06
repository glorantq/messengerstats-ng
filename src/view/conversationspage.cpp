#include "view/conversationspage.h"
#include "./ui_conversationspage.h"

#include "view/conversationlistitem.h"

ConversationsPage::ConversationsPage(
    QWidget* parent,
    const QList<std::shared_ptr<data::Thread>> threads)
    : QWidget(parent), ui(new Ui::ConversationsPage) {
    ui->setupUi(this);

    // Create an item in the list for each thread
    for (const auto& threadPtr : threads) {
        ConversationListItem* element =
            new ConversationListItem(this, threadPtr.get());
        CustomListWidgetItem* item =
            new CustomListWidgetItem(ui->conversationList);

        item->setSizeHint(QSize(element->width(), element->height()));
        ui->conversationList->addItem(item);
        ui->conversationList->setItemWidget(item, element);

        item->setData(thread::ModelData::DisplayName,
                      threadPtr->getDisplayName());
        item->setData(thread::ModelData::MessageCount,
                      threadPtr->getMessages().count());
        item->setData(thread::ModelData::ThreadType,
                      threadPtr->getThreadType());
        item->setData(thread::ModelData::SortingMode,
                      thread::SortingMode::Alphabetical);
        item->setData(thread::ModelData::RawPointer,
                      (unsigned long long)threadPtr.get());
    }

    // Integrate the look of the list with the window
    QPalette listPalette = palette();
    listPalette.setColor(QPalette::Highlight,
                         palette().color(QPalette::AlternateBase));
    listPalette.setColor(QPalette::Base, palette().color(QPalette::Window));
    listPalette.setColor(QPalette::AlternateBase,
                         palette().color(QPalette::Window));
    ui->conversationList->setPalette(listPalette);

    // Set up the filters
    ui->threadTypeComboBox->addItems({tr("All conversations"),
                                      tr("Direct messages only"),
                                      tr("Groups only")});
    ui->threadTypeComboBox->setCurrentIndex(0);

    ui->sortingMethodComboBox->addItems({tr("Alphabetical"), tr("Messages")});
    ui->sortingMethodComboBox->setCurrentIndex(
        thread::SortingMode::Alphabetical);
}

ConversationsPage::~ConversationsPage() {
    delete ui;
}

void ConversationsPage::on_searchBox_textChanged(const QString&) {
    performFiltering();
}

void ConversationsPage::on_threadTypeComboBox_currentIndexChanged(int) {
    performFiltering();
}

void ConversationsPage::performFiltering() {
    for (int i = 0; i < ui->conversationList->count(); i++) {
        QListWidgetItem* item = ui->conversationList->item(i);

        QString threadName =
            item->data(thread::ModelData::DisplayName).toString();

        item->setHidden(
            !(threadName.contains(ui->searchBox->text(), Qt::CaseInsensitive) &&
              evaluateTypeFilter(item)));
    }
}

bool ConversationsPage::evaluateTypeFilter(QListWidgetItem* element) {
    data::ThreadType threadType =
        (data::ThreadType)element->data(thread::ModelData::ThreadType).toInt();

    int filterType = ui->threadTypeComboBox->currentIndex();

    switch (filterType) {
        case 0:
            return true;
        case 1:
            return threadType == data::ThreadType::Regular;
        case 2:
            return threadType == data::ThreadType::RegularGroup;
        default:
            return false;
    }
}

inline void ConversationsPage::performSorting() {
    ui->conversationList->sortItems(sortAscending ? Qt::AscendingOrder
                                                  : Qt::DescendingOrder);
}

// This is a hack; in order to sort according to multiple criteria, a value is
// set in each item that signifies the value we want to sort by. The
// implementation of operator< on the list item then looks at this and compares
// accordingly.
void ConversationsPage::on_sortingMethodComboBox_currentIndexChanged(
    int index) {
    for (int i = 0; i < ui->conversationList->count(); i++) {
        ui->conversationList->item(i)->setData(thread::ModelData::SortingMode,
                                               index);
    }

    performSorting();
}

void ConversationsPage::on_sortOrderButton_clicked() {
    sortAscending = !sortAscending;

    ui->sortOrderButton->setIcon(
        QIcon(sortAscending ? "://resources/icon/silk/arrow_up.png"
                            : "://resources/icon/silk/arrow_down.png"));

    performSorting();
}

// When we need to navigate, we emit an event that the main window will catch,
// with the parameter being a pointer to the thread we need to navigate to
void ConversationsPage::on_conversationList_itemDoubleClicked(
    QListWidgetItem* item) {
    data::Thread* thread =
        (data::Thread*)item->data(thread::ModelData::RawPointer).toULongLong();

    emit navigateToConversation(thread);
}

// The comparator for the thread items, allowing multiple sorting modes, based
// on the value of QT_DATA_SORTING_MODE
bool CustomListWidgetItem::operator<(const QListWidgetItem& other) const {
    int sortingMethod = data(thread::ModelData::SortingMode).toInt();

    if (sortingMethod == thread::SortingMode::Alphabetical) {
        QString ownName = data(thread::ModelData::DisplayName).toString();
        QString otherName =
            other.data(thread::ModelData::DisplayName).toString();

        return ownName.localeAwareCompare(otherName) < 0;
    } else if (sortingMethod == thread::SortingMode::TotalMessageCount) {
        int ownMessageCount = data(thread::ModelData::MessageCount).toInt();
        int otherMessageCount =
            other.data(thread::ModelData::MessageCount).toInt();

        return ownMessageCount < otherMessageCount;
    }

    return this < &other;
}
