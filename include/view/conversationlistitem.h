#pragma once

#include <QWidget>

#include "model/thread.h"

namespace Ui {
class ConversationListItem;
}

class ConversationListItem : public QWidget {
    Q_OBJECT

   public:
    explicit ConversationListItem(QWidget* parent = nullptr,
                                  data::Thread* thread = nullptr);
    ~ConversationListItem();

   private:
    Ui::ConversationListItem* ui;
};
