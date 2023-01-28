#pragma once

#include <QDateTime>
#include <QLabel>
#include <QMap>
#include <QString>
#include <QWidget>

#include "model/thread.h"

class StatisticProvider;

class PropertyAccessor {
   private:
    StatisticProvider* m_provider{};
    QString m_object{};
    QString m_name{};

   public:
    PropertyAccessor() : m_provider(nullptr), m_object(), m_name() {}

    PropertyAccessor(StatisticProvider* provider,
                     const QString& object,
                     const QString& name)
        : m_provider(provider), m_object(object), m_name(name) {}

    const QVariant& get() const;
    const QVariant* operator->() { return &get(); }
    void set(const QVariant&);
    bool valid() const { return m_provider != nullptr; }

    const QString& object() { return m_object; }
    const QString& name() { return m_name; }
};

class StatisticProvider {
   private:
    QMap<QString, QMap<QString, QVariant>> m_properties{};
    const QVariant m_defaultVariant{};

   public:
    virtual void update(data::Thread*) = 0;
    virtual QWidget* getVisualisation() = 0;

    virtual QString getName() const = 0;

    virtual ~StatisticProvider() {}

   protected:
    PropertyAccessor registerProperty(const QString& name,
                                      const QVariant& defaultValue);

    virtual void onPropertyUpdated(const QString& object, const QString& name) {
    }

   public:
    void setProperty(const QString& object,
                     const QString& name,
                     const QVariant& value);

    const QVariant& getProperty(const QString& object, const QString& name) {
        if (!m_properties.contains(object)) {
            return m_defaultVariant;
        }

        auto& objectProperties = m_properties[object];

        if (!objectProperties.contains(name)) {
            return m_defaultVariant;
        }

        return objectProperties[name];
    }

    const QMap<QString, QMap<QString, QVariant>> getProperties() const {
        return m_properties;
    }
};
