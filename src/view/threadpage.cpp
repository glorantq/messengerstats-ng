#include "view/threadpage.h"
#include "./ui_threadpage.h"

#include <QDateTime>
#include <QFontMetrics>
#include <QMenu>
#include <QPainter>
#include <QPainterPath>
#include <QScrollBar>

#include "view/thread/messageitemdelegate.h"
#include "view/thread/threadlistmodel.h"

ThreadPage::ThreadPage(QWidget* parent, data::Thread* thread)
    : QWidget(parent), ui(new Ui::ThreadPage) {
    ui->setupUi(this);

    if (thread == nullptr) {
        return;
    }

    m_thread = thread;

    ui->nameLabel->setText(thread->getDisplayName());

    auto setDefaultIcon = [&]() {
        if (thread->getThreadType() == data::ThreadType::RegularGroup) {
            ui->userPictureLabel->setPixmap(
                QPixmap("://resources/images/default-group.png"));
        }
    };

    if (thread->getThreadIcon()) {  // If the thread has an icon, use it
        QPixmap iconPixmap(*thread->getThreadIcon());

        if (!iconPixmap.isNull()) {
            ui->userPictureLabel->setPixmap(iconPixmap);
        } else {
            setDefaultIcon();
        }
    } else {
        // Else, if this is a group use the default group avatar
        setDefaultIcon();
    }

    // Set up the list view with out model and delegate. The model provides the
    // actual data to the list view via lazy loading, and the delegate handles
    // rendering based on the type of message
    ui->messagesListView->setModel(
        new ThreadListModel(ui->messagesListView, m_thread));
    ui->messagesListView->setItemDelegate(
        new MessageItemDelegate(ui->messagesListView));

    // Blend in the list with the background
    QPalette listPalette = palette();
    listPalette.setColor(QPalette::Highlight,
                         palette().color(QPalette::AlternateBase));
    listPalette.setColor(QPalette::Base, palette().color(QPalette::Window));
    listPalette.setColor(QPalette::AlternateBase,
                         palette().color(QPalette::Window));
    ui->messagesListView->setPalette(listPalette);

    // For some reason setting PerPixelScroll is not enough; this is a
    // workaround to that issue
    ui->messagesListView->verticalScrollBar()->setSingleStep(10);

    // Set up a context menu that will be opened when right-clicking on the
    // messages
    ui->messagesListView->setContextMenuPolicy(
        Qt::ContextMenuPolicy::CustomContextMenu);
    connect(ui->messagesListView, &QListView::customContextMenuRequested, this,
            &ThreadPage::on_chatContextMenuRequested);

    // This menu is shown when the user clicks on the hamburger menu to the
    // right
    QMenu* popupMenu = new QMenu;

    QAction* threadInformationAction =
        new QAction(QIcon("://resources/icon/silk/information.png"),
                    tr("Thread information"), popupMenu);

    connect(threadInformationAction, &QAction::triggered,
            [&]() { emit onThreadInformationPressed(m_thread); });

    QAction* threadStatisticsAction =
        new QAction(QIcon("://resources/icon/silk/chart_bar.png"),
                    tr("Statistics"), popupMenu);

    QAction* threadOpenFolderAction =
        new QAction(QIcon("://resources/icon/silk/folder.png"),
                    tr("Open folder"), popupMenu);

    connect(threadOpenFolderAction, &QAction::triggered,
            [&]() { emit onOpenDirectoryPressed(m_thread); });

    popupMenu->addAction(threadInformationAction);
    popupMenu->addAction(threadStatisticsAction);
    popupMenu->addSeparator();
    popupMenu->addAction(threadOpenFolderAction);

    ui->menuButton->setMenu(popupMenu);

    // Evil stylesheets
    ui->menuButton->setStyleSheet("::menu-indicator{ image: none; }");
}

ThreadPage::~ThreadPage() {
    delete ui;
}

// Emit a signal to notify the main window that we want to go back
void ThreadPage::on_backButton_clicked() {
    emit onBackPressed();
}

void ThreadPage::on_messagesListView_doubleClicked(const QModelIndex& index) {
    data::Message* messagePointer =
        (data::Message*)index.data(message::ModelData::RawPointer)
            .toULongLong();

    emit onMessageInformationRequested(messagePointer);
}

void ThreadPage::on_chatContextMenuRequested(const QPoint& position) {
    auto selectedItems = ui->messagesListView->selectionModel()->selectedRows();
    if (selectedItems.count() == 0) {
        return;
    }

    QModelIndex selected = selectedItems.first();

    QMenu contextMenu;
    contextMenu.addAction(QString("%1").arg(selected.row()));

    QPoint globalPos = ui->messagesListView->mapToGlobal(position);
    contextMenu.exec(globalPos);
}

void ThreadPage::on_settingsChanged() {
    QPalette listPalette = palette();
    listPalette.setColor(QPalette::Highlight,
                         palette().color(QPalette::AlternateBase));
    listPalette.setColor(QPalette::Base, palette().color(QPalette::Window));
    listPalette.setColor(QPalette::AlternateBase,
                         palette().color(QPalette::Window));
    ui->messagesListView->setPalette(listPalette);

    MessageItemDelegate* delegate =
        (MessageItemDelegate*)ui->messagesListView->itemDelegate();

    delegate->refreshTheme();

    // This is why you don't use stylesheets, it needs to be set again to
    // reflect palette changes
    ui->menuButton->setStyleSheet("::menu-indicator{ image: none; }");
}
