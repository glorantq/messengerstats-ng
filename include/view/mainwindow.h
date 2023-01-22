#pragma once

#include <QException>
#include <QList>
#include <QMainWindow>
#include <QPromise>
#include <QRegularExpression>

#include "model/messengerdata.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

   private slots:
    void on_action_aboutQt_triggered();
    void on_action_about_triggered();
    void on_action_quit_triggered();
    void on_action_openProject_triggered();

    void on_navigateToConversation(data::Thread*);
    void on_threadBackPressed();
    void on_threadOpenDirectoryPressed(data::Thread*);
    void on_threadInformationPressed(data::Thread*);
    void on_messageInformationRequested(data::Message*);
    void on_personInformationRequested(const QUuid);
    void on_threadStatisticsRequested(data::Thread*);

    void on_action_preferences_triggered();

   signals:
    void onSettingsChanged();

   private:
    Ui::MainWindow* ui;

    std::shared_ptr<data::MessengerData> m_messengerData{};

    QList<QRegularExpression> m_systemMessagePatterns{};
    QList<QRegularExpression> m_nicknameChangePatterns{};
    QList<QPair<data::nickname::SubjectType, QRegularExpression>>
        m_nicknameSubjectMapping{};

    void performDirectoryOpen(QString selectedPath);
    void popCurrentPage();

    void performDirectoryOpenAsync(
        QPromise<std::shared_ptr<data::MessengerData>>& promise,
        QDir directory);

    // TODO: Move this to the model
    bool isMessageSystemMessage(QString content);
    std::optional<data::nickname::ChangeParams> determineNicknameChange(
        QString content);

    class RuntimeError : public QException {
       private:
        QString m_reason;

       public:
        explicit RuntimeError(const QString reason)
            : m_reason(std::move(reason)) {}

        // exception interface
       public:
        const char* what() const noexcept override {
            return m_reason.toUtf8().data();
        }

        // QException interface
       public:
        void raise() const override { throw *this; }
        QException* clone() const override { return new RuntimeError(*this); }
    };
};
