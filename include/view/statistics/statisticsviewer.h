#pragma once

#include <QDialog>

#include <vendor/qtpropertybrowser/qtpropertymanager.h>
#include <vendor/qtpropertybrowser/qtvariantproperty.h>

#include "model/thread.h"

namespace Ui {
class StatisticsViewer;
}

class StatisticsViewer : public QDialog {
    Q_OBJECT

   private:
    data::Thread* m_thread{};

    const QMap<QString, QStringList> m_flagTypeNames{
        {"QFlags<QChart::AnimationOption>", {"Axis", "Series"}},
    };
    const QMap<QString, QStringList> m_enumTypeNames{
        {"QChart::ChartTheme",
         {"Light", "Blue Cerulean", "Dark", "Brown Sand", "Blue Ncs",
          "High Contrast", "Blue Icy", "Qt"}},
    };

   public:
    explicit StatisticsViewer(QWidget* parent, data::Thread* thread);
    ~StatisticsViewer();

   private slots:
    void on_typeComboBox_currentIndexChanged(int index);
    void on_propertyManager_valueChanged(QtProperty*, const QVariant&);

   private:
    Ui::StatisticsViewer* ui;

    QtVariantPropertyManager* m_propertyManager;
    QMap<QtProperty*, QString> m_propertyNamespaceMapping{};
    QMap<QString, QMap<QString, QVariant>> m_initialPropertiesSnapshot{};
};
