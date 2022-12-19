#include "view/mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFutureWatcher>
#include <QMessageBox>
#include <QPixmapCache>
#include <QProgressDialog>
#include <QSettings>
#include <QtConcurrentRun>

#include "view/conversationspage.h"
#include "view/message/messageinformationdialog.h"
#include "view/preferencesdialog.h"
#include "view/settings.h"
#include "view/thread/threadinformationdialog.h"
#include "view/threadpage.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    setWindowTitle(QCoreApplication::applicationName());

    QDir classifierDirectory = QCoreApplication::applicationDirPath()
                                   .append("%1classifiers%1")
                                   .arg(QDir::separator());

    if (classifierDirectory.exists()) {
        for (const auto& entry : classifierDirectory.entryList(
                 {"system_*.txt"}, QDir::Filter::Files)) {
            QFile classifierFile = classifierDirectory.filePath(entry);

            if (classifierFile.exists()) {
                classifierFile.open(QIODevice::ReadOnly);
                QString fileContents =
                    QString::fromUtf8(classifierFile.readAll());
                classifierFile.close();

                QStringList lines =
                    fileContents.split(QRegularExpression("[\r\n]"),
                                       Qt::SplitBehaviorFlags::SkipEmptyParts);

                for (const auto& line : lines) {
                    m_systemMessagePatterns.push_back(QRegularExpression(line));
                }
            }
        }

        qDebug() << "Loaded" << m_systemMessagePatterns.count()
                 << "system message classifiers!";

        for (const auto& entry : classifierDirectory.entryList(
                 {"nickname_*.txt"}, QDir::Filter::Files)) {
            QFile classifierFile = classifierDirectory.filePath(entry);

            if (classifierFile.exists()) {
                classifierFile.open(QIODevice::ReadOnly);
                QString fileContents =
                    QString::fromUtf8(classifierFile.readAll());
                classifierFile.close();

                QStringList lines =
                    fileContents.split(QRegularExpression("[\r\n]"),
                                       Qt::SplitBehaviorFlags::SkipEmptyParts);

                for (const auto& line : lines) {
                    if (!line.startsWith("#")) {
                        m_nicknameChangePatterns.push_back(
                            QRegularExpression(line));
                    } else {
                        QStringList parts = line.mid(1).split("->");

                        QRegularExpression expression(parts[0].trimmed());
                        QString subjectString = parts[1].trimmed();

                        if (subjectString == "sender") {
                            m_nicknameSubjectMapping.push_back(
                                {data::nickname::SubjectType::Sender,
                                 expression});
                        } else if (subjectString == "owner") {
                            m_nicknameSubjectMapping.push_back(
                                {data::nickname::SubjectType::Owner,
                                 expression});
                        } else {
                            qWarning()
                                << "Unknown nickname subject:" << subjectString;
                        }
                    }
                }
            }
        }

        qDebug() << "Loaded" << m_nicknameChangePatterns.count()
                 << "nickname message classifiers!";
    } else {
        qWarning() << "Can't load message classifiers as the directory "
                      "doesn't exist! ("
                   << classifierDirectory.path() << ")";
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_action_aboutQt_triggered() {
    QMessageBox::aboutQt(this, windowTitle());
}

void MainWindow::on_action_about_triggered() {
    QMessageBox::about(this, windowTitle(),
                       tr(R"(
<h2>%1 <small>%2</small></h2>
<p>Marcella.</p>

<h4>Third-party attributions</h4>
<p>Uses the "Silk" icon set, licensed under CC-BY-2.5, from <a href="http://www.famfamfam.com/lab/icons/silk/">FAMFAMFAM</a>
Uses the "Silk Icons Companion #1" icon set, licensed under CC-BY-2.5, from <a href="https://github.com/damieng/silk-companion">damieng</a><br></p>
)")
                           .arg(QCoreApplication::applicationName())
                           .arg(QCoreApplication::applicationVersion()));
}

void MainWindow::on_action_quit_triggered() {
    QCoreApplication::quit();
}

void MainWindow::on_action_preferences_triggered() {
    PreferencesDialog preferencesDialog(this);

    if (preferencesDialog.exec() == QDialog::Accepted) {
        setPalette(QApplication::palette());

        for (int i = 0; i < ui->stackedWidget->count(); i++) {
            ui->stackedWidget->widget(i)->setPalette(palette());
        }

        emit onSettingsChanged();

        QMessageBox::warning(this, QCoreApplication::applicationName(),
                             tr("Some settings may need an application restart "
                                "to take effect."));
    }
}

void MainWindow::on_action_openProject_triggered() {
    QFileDialog openDialog(this, tr("Open a data download"), QDir::homePath());
    openDialog.setAcceptMode(QFileDialog::AcceptOpen);
    openDialog.setFileMode(QFileDialog::Directory);

    if (openDialog.exec()) {
        QString selectedPath = openDialog.selectedFiles().first();
        performDirectoryOpen(selectedPath);
    }
}

// Handles the requests for navigating to a thread, emitted by the
// conversations page
void MainWindow::on_navigateToConversation(data::Thread* thread) {
    if (thread == nullptr) {
        return;
    }

    qDebug() << "Navigate to thread:" << thread->getDisplayName();

    ThreadPage* threadPage = new ThreadPage(ui->stackedWidget, thread);
    ui->stackedWidget->addWidget(threadPage);

    QSettings settings;

    int newIndex = ui->stackedWidget->count() - 1;
    if (settings.value(SETTINGS_KEY_ANIMATE_PAGE_TRANSITION, true).toBool()) {
        ui->stackedWidget->slideInIdx(newIndex);
    } else {
        ui->stackedWidget->setCurrentIndex(newIndex);
    }

    // Connect our settings changed event to the new page's slot so it
    // can adapt to theme changes
    connect(this, &MainWindow::onSettingsChanged, threadPage,
            &ThreadPage::on_settingsChanged);

    // Connect the back button event of the newly opened page, so the user
    // can close it and open another thread instead
    connect(threadPage, &ThreadPage::onBackPressed, this,
            &MainWindow::on_threadBackPressed);

    // Connect the event for opening the thread's directory, and actually
    // perform said opening of directory
    connect(threadPage, &ThreadPage::onOpenDirectoryPressed, this,
            &MainWindow::on_threadOpenDirectoryPressed);

    // Connect the event for opening information about this thread
    connect(threadPage, &ThreadPage::onThreadInformationPressed, this,
            &MainWindow::on_threadInformationPressed);

    // Connect the event for opening information about a specific message
    connect(threadPage, &ThreadPage::onMessageInformationRequested, this,
            &MainWindow::on_messageInformationRequested);
}

// Handles the event of pressing the back button in a thread, emitted by
// the thread page
void MainWindow::on_threadBackPressed() {
    popCurrentPage();
}

void MainWindow::on_threadOpenDirectoryPressed(data::Thread* thread) {
    QDesktopServices::openUrl(QUrl::fromLocalFile(
        thread->getOwnerDownload()->getRootDirectory().filePath(
            thread->getThreadPath())));
}

void MainWindow::on_threadInformationPressed(data::Thread* thread) {
    qDebug() << "Showing thread information for" << thread->getDisplayName();

    ThreadInformationDialog* threadInformationDialog =
        new ThreadInformationDialog(this, thread);
    threadInformationDialog->setWindowTitle(
        QString("%1 - %2").arg(windowTitle()).arg(thread->getDisplayName()));
    threadInformationDialog->setWindowModality(
        Qt::WindowModality::ApplicationModal);

    connect(threadInformationDialog,
            &ThreadInformationDialog::onPersonInformationRequested, this,
            &MainWindow::on_personInformationRequested);

    threadInformationDialog->show();
}

void MainWindow::on_messageInformationRequested(data::Message* message) {
    qDebug() << "Showing message information for" << message->getContent();

    MessageInformationDialog* messageInformationDialog =
        new MessageInformationDialog(this, message);
    messageInformationDialog->setWindowTitle(
        tr("%1 - Message information").arg(windowTitle()));
    messageInformationDialog->setWindowModality(
        Qt::WindowModality::ApplicationModal);
    messageInformationDialog->show();
}

void MainWindow::on_personInformationRequested(const QUuid identifier) {
    qDebug() << "Showing person information for" << identifier;
}

void MainWindow::performDirectoryOpenAsync(
    QPromise<std::shared_ptr<data::MessengerData>>& promise,
    QDir directory) {
    try {
        auto messengerData = data::MessengerData::loadFromDirectory(
            directory,
            {[this](QString content) {
                 return this->isMessageSystemMessage(content);
             },
             [this](QString content) {
                 return this->determineNicknameChange(content);
             }},
            [&](int min, int max) { promise.setProgressRange(min, max); },
            [&](int value, const QString& text) {
                promise.setProgressValueAndText(value, text);
            });

        promise.addResult(messengerData);
    } catch (std::runtime_error& e) {
        promise.setException(RuntimeError(e.what()));
    }

    promise.finish();
}

// Asks the user for a directory to open, then tries to parse it as a GDPR
// data takeout. If it succeeds, then a navigation to the thread page occurs
void MainWindow::performDirectoryOpen(QString selectedPath) {
    QDir directory(selectedPath);

    if (!directory.makeAbsolute() || !directory.exists()) {
        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              tr("The selected directory does not exist!"));
        return;
    }

    if (directory.exists("messages/") && !directory.cd("messages")) {
        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              tr("The selected directory contains a messages "
                                 "directory, but changing to it failed!"));
        return;
    }

    // Perform some basic checking if this is a valid directory or not

    if (!directory.exists("inbox/")) {
        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              tr("The directory you selected does not appear "
                                 "to be a valid Messenger data download."));
        return;
    }

    if (ui->stackedWidget->count() > 1) {
        ui->stackedWidget->setCurrentIndex(0);

        for (int i = ui->stackedWidget->count() - 1; i > 0; i--) {
            QWidget* widget = ui->stackedWidget->widget(i);
            ui->stackedWidget->removeWidget(widget);
            widget->deleteLater();
        }
    }

    QProgressDialog* progressDialog = new QProgressDialog(this);
    progressDialog->setLabelText(
        tr("Loading archive...<br>(<small>preparing</small>)"));
    progressDialog->setWindowTitle(windowTitle());
    progressDialog->setCancelButton(nullptr);
    progressDialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint |
                                   Qt::CustomizeWindowHint);
    progressDialog->setFixedSize(progressDialog->size());
    progressDialog->setMinimumDuration(0);

    QFutureWatcher<std::shared_ptr<data::MessengerData>>* futureWatcher =
        new QFutureWatcher<std::shared_ptr<data::MessengerData>>(this);

    connect(futureWatcher,
            &QFutureWatcher<
                std::shared_ptr<data::MessengerData>>::progressRangeChanged,
            [=](int min, int max) { progressDialog->setRange(min, max); });

    connect(futureWatcher,
            &QFutureWatcher<
                std::shared_ptr<data::MessengerData>>::progressTextChanged,
            [=](const QString& text) {
                progressDialog->setLabelText(
                    tr("Loading archive...<br>(<small>%1</small>)").arg(text));
            });

    connect(futureWatcher,
            &QFutureWatcher<
                std::shared_ptr<data::MessengerData>>::progressValueChanged,
            [=](int value) { progressDialog->setValue(value); });

    QFuture<std::shared_ptr<data::MessengerData>> future = QtConcurrent::run(
        [=](QPromise<std::shared_ptr<data::MessengerData>>& promise,
            QDir directory) {
            this->performDirectoryOpenAsync(promise, directory);
        },
        directory);

    connect(
        futureWatcher,
        &QFutureWatcher<std::shared_ptr<data::MessengerData>>::finished, [=]() {
            try {
                m_messengerData = future.result();

                if (m_messengerData) {
                    ConversationsPage* conversationsPage =
                        new ConversationsPage(ui->stackedWidget,
                                              m_messengerData->getThreads());

                    ui->stackedWidget->addWidget(conversationsPage);
                    ui->stackedWidget->setCurrentIndex(1);

                    // Connect the navigation signal so we can actually open
                    // threads
                    connect(conversationsPage,
                            &ConversationsPage::navigateToConversation, this,
                            &MainWindow::on_navigateToConversation);

                    // Connect our settings changed event to the new page's
                    // slot so it can adapt to theme changes
                    connect(this, &MainWindow::onSettingsChanged,
                            conversationsPage,
                            &ConversationsPage::on_settingsChanged);

                    progressDialog->close();
                }
            } catch (const RuntimeError& e) {
                QMessageBox::critical(
                    this, QCoreApplication::applicationName(),
                    tr("Failed to parse data archive: %1").arg(e.what()));
            }

            progressDialog->deleteLater();
            futureWatcher->deleteLater();
        });

    futureWatcher->setFuture(future);
    progressDialog->exec();
}

// Closes the topmost page in the stackwidget, this is the implementation
// for the back button
void MainWindow::popCurrentPage() {
    int i = ui->stackedWidget->count() - 1;

    QSettings settings;

    if (settings.value(SETTINGS_KEY_ANIMATE_PAGE_TRANSITION, true).toBool()) {
        QObject* context = new QObject(this);
        connect(ui->stackedWidget, &SlidingStackedWidget::animationFinished,
                context, [=]() {
                    delete context;

                    QWidget* widget = ui->stackedWidget->widget(i);
                    ui->stackedWidget->removeWidget(widget);
                    widget->deleteLater();
                });

        ui->stackedWidget->slideInIdx(i - 1);
    } else {
        ui->stackedWidget->setCurrentIndex(i - 1);

        QWidget* widget = ui->stackedWidget->widget(i);
        ui->stackedWidget->removeWidget(widget);
        widget->deleteLater();
    }
}

bool MainWindow::isMessageSystemMessage(QString content) {
    for (const auto& pattern : m_systemMessagePatterns) {
        if (pattern.match(content).hasMatch()) {
            return true;
        }
    }

    return false;
}

std::optional<data::nickname::ChangeParams> MainWindow::determineNicknameChange(
    QString content) {
    for (const auto& pattern : m_nicknameChangePatterns) {
        QRegularExpressionMatch match = pattern.match(content);

        if (match.hasMatch()) {
            QString subject = match.captured("subject");
            QString nickname = match.captured("nickname");

            if (subject.isEmpty()) {
                qWarning() << "Nickname subject is empty!";
                continue;
            }

            if (nickname.isEmpty()) {
                qWarning() << "Nickname is empty!";
                continue;
            }

            data::nickname::SubjectType subjectType =
                data::nickname::SubjectType::Other;

            for (const auto& typeMatchingPair : m_nicknameSubjectMapping) {
                if (typeMatchingPair.second.match(subject).hasMatch()) {
                    subjectType = typeMatchingPair.first;
                }
            }

            return data::nickname::ChangeParams{{subjectType, subject},
                                                nickname};
        }
    }

    return {};
}
