#include "include/view/statistics/statisticsviewer.h"
#include "./ui_statisticsviewer.h"

#include "view/statistics/impl/basic.h"
#include "view/statistics/impl/messages.h"
#include "view/statistics/statisticprovider.h"

#include <QVBoxLayout>

StatisticsViewer::StatisticsViewer(QWidget* parent, data::Thread* thread)
    : QDialog(parent), m_thread(thread), ui(new Ui::StatisticsViewer) {
    ui->setupUi(this);

    m_propertyManager = new QtVariantPropertyManager;
    ui->propertyBrowser->setFactoryForManager(m_propertyManager,
                                              new QtVariantEditorFactory);
    ui->propertyBrowser->setPropertiesWithoutValueMarked(true);

    connect(m_propertyManager, &QtVariantPropertyManager::valueChanged, this,
            &StatisticsViewer::on_propertyManager_valueChanged);

    // TODO (glorantv): testing

    QList<StatisticProvider*> testing = {
        new TestingStatisticsProvider,
        new MonthlyMessageCountStatisticProvider(thread),
    };

    for (const auto& e : testing) {
        ui->typeComboBox->addItem(e->getName(), (unsigned long long)e);
    }
}

StatisticsViewer::~StatisticsViewer() {
    // TODO (glorantv): testing

    for (int i = 0; i < ui->typeComboBox->count(); i++) {
        delete (StatisticProvider*)ui->typeComboBox->itemData(i).toULongLong();
    }

    delete ui;
    delete m_propertyManager;
}

void StatisticsViewer::on_typeComboBox_currentIndexChanged(int index) {
    StatisticProvider* provider =
        (StatisticProvider*)ui->typeComboBox->itemData(index).toULongLong();

    ui->graphStack->setCurrentIndex(0);

    while (ui->graphStack->count() > 1) {
        int i = ui->graphStack->count() - 1;

        QWidget* widget = ui->graphStack->widget(i);
        ui->graphStack->removeWidget(widget);
        widget->deleteLater();
    }

    m_propertyManager->blockSignals(true);

    ui->propertyBrowser->clear();
    m_propertyManager->clear();
    m_propertyNamespaceMapping.clear();

    m_initialPropertiesSnapshot = provider->getProperties();

    for (const auto& [ns, props] : provider->getProperties().toStdMap()) {
        QtProperty* namespaceProperty = m_propertyManager->addProperty(
            QtVariantPropertyManager::groupTypeId(), ns);

        for (const auto& [key, value] : props.toStdMap()) {
            QString typeName = QString(value.typeName());
            int typeId = value.typeId();

            if (m_flagTypeNames.contains(typeName)) {
                typeId = QtVariantPropertyManager::flagTypeId();
            }

            if (m_enumTypeNames.contains(typeName)) {
                typeId = QtVariantPropertyManager::enumTypeId();
            }

            QtVariantProperty* property =
                m_propertyManager->addProperty(typeId, key);
            property->setValue(value);

            if (typeId == QtVariantPropertyManager::flagTypeId()) {
                property->setAttribute("flagNames", m_flagTypeNames[typeName]);
                property->setValue(value.toInt());
            } else if (typeId == QtVariantPropertyManager::enumTypeId()) {
                property->setAttribute("enumNames", m_enumTypeNames[typeName]);
                property->setValue(value.toInt());
            }

            namespaceProperty->addSubProperty(property);
            m_propertyNamespaceMapping[property] = ns;
        }

        ui->propertyBrowser->addProperty(namespaceProperty);
    }

    m_propertyManager->blockSignals(false);

    ui->graphStack->addWidget(provider->getVisualisation());
    ui->graphStack->setCurrentIndex(ui->graphStack->count() - 1);

    provider->update(m_thread);
}

void StatisticsViewer::on_propertyManager_valueChanged(QtProperty* property,
                                                       const QVariant& value) {
    QString objectName = m_propertyNamespaceMapping[property];
    QString name = property->propertyName();

    StatisticProvider* provider =
        (StatisticProvider*)ui->typeComboBox->currentData().toULongLong();

    if (provider != nullptr) {
        provider->setProperty(objectName, name, value);

        const QVariant& originalValue =
            m_initialPropertiesSnapshot[objectName][name];

        QString typeName = QString(originalValue.typeName());

        if (m_flagTypeNames.contains(typeName) ||
            m_enumTypeNames.contains(typeName)) {
            property->setModified(value.toInt() != originalValue.toInt());
        } else {
            property->setModified(value != originalValue);
        }
    }
}
