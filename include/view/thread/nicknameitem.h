#pragma once

#include <QWidget>

#include "model/common.h"
#include "model/thread.h"

namespace Ui {
class NicknameItem;
}

class NicknameItem : public QWidget {
    Q_OBJECT

   public:
    explicit NicknameItem(QWidget* parent,
                          data::Thread* thread,
                          const data::Nickname& nickname);
    ~NicknameItem();

   private:
    Ui::NicknameItem* ui;
};
