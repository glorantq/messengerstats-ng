#pragma once

#include <QList>
#include <QListWidgetItem>
#include <QWidget>

#include <memory>

#include "model/thread.h"

namespace Ui {
class ConversationsPage;
}

namespace thread {
enum ModelData {
    DisplayName = Qt::UserRole,
    MessageCount,
    ThreadType,
    SortingMode,
    RawPointer,
    ParticipantCount,
};

enum SortingMode {
    Alphabetical = 0,
    TotalMessageCount,
    Participants,
};
};  // namespace thread

class ConversationsPage : public QWidget {
    Q_OBJECT

   public:
    explicit ConversationsPage(
        QWidget* parent = nullptr,
        const QList<std::shared_ptr<data::Thread>> threads = {});
    ~ConversationsPage();

   signals:
    void navigateToConversation(data::Thread* thread);

   private slots:
    void on_searchBox_textChanged(const QString& filterText);
    void on_threadTypeComboBox_currentIndexChanged(int index);
    void on_sortingMethodComboBox_currentIndexChanged(int index);
    void on_sortOrderButton_clicked();
    void on_conversationList_itemDoubleClicked(QListWidgetItem* item);

   public slots:
    void on_settingsChanged();

   private:
    Ui::ConversationsPage* ui;

    bool sortAscending = true;

    // The method that performs the filtering of the threads, hiding elements in
    // the list that don't match
    void performFiltering();

    // Convenience method to determine if a thread type should be shown or not
    bool evaluateTypeFilter(QListWidgetItem*);

    // Just calls sort on the list. That's it.
    inline void performSorting();
};

class CustomListWidgetItem : public QListWidgetItem {
   public:
    CustomListWidgetItem(QListWidget* parent) : QListWidgetItem(parent) {}

   private:
    bool operator<(const QListWidgetItem& other) const;
};
