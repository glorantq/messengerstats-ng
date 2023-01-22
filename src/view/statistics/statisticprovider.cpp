#include "view/statistics/statisticprovider.h"

PropertyAccessor StatisticProvider::registerProperty(
    const QString& name,
    const QVariant& defaultValue) {
    QString object = getName();

    if (m_properties.contains(object) && m_properties[object].contains(name)) {
        qWarning() << "Property" << name << "is already registered!";
        return {};
    }

    if (!m_properties.contains(object)) {
        m_properties[object] = {};
    }

    m_properties[object][name] = defaultValue;

    return PropertyAccessor(this, object, name);
}

void StatisticProvider::setProperty(const QString& object,
                                    const QString& name,
                                    const QVariant& value) {
    if (m_properties.contains(object) && m_properties[object].contains(name)) {
        m_properties[object][name].setValue(value);
    } else {
        if (!m_properties.contains(object)) {
            m_properties[object] = {};
        }

        m_properties[object][name] = value;
    }

    onPropertyUpdated(object, name);
}

const QVariant& PropertyAccessor::get() const {
    return m_provider->getProperty(m_object, m_name);
}

void PropertyAccessor::set(const QVariant& value) {
    m_provider->setProperty(m_object, m_name, value);
}
