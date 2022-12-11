#pragma once

#include <QList>
#include <QMainWindow>
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

    void on_action_preferences_triggered();

   private:
    Ui::MainWindow* ui;

    std::shared_ptr<data::MessengerData> m_messengerData{};

    QList<QRegularExpression> m_systemMessagePatterns{};
    QList<QRegularExpression> m_nicknameChangePatterns{};
    QList<QPair<data::nickname::SubjectType, QRegularExpression>>
        m_nicknameSubjectMapping{};

    void performDirectoryOpen(QString selectedPath);
    void popCurrentPage();

    // TODO: Move this to the model
    bool isMessageSystemMessage(QString content);
    std::optional<data::nickname::ChangeParams> determineNicknameChange(
        QString content);
};
