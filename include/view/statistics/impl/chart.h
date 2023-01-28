#pragma once

#include <QCoreApplication>
#include <QWidget>
#include <QtCharts>

#include "view/statistics/statisticprovider.h"

class ChartStatisticProvider : public virtual StatisticProvider {
    Q_DECLARE_TR_FUNCTIONS(ChartStatisticProvider)

   protected:
    QChart* m_chart{};
    QChartView* m_chartView{};

    QString m_valueLegend{};

    PropertyAccessor m_showValueAxis{};
    PropertyAccessor m_showLegend{};
    PropertyAccessor m_animationFlags{};
    PropertyAccessor m_theme{};

    QList<QPair<QString, long long>> m_dataSet{};

    // StatisticProvider interface
   public:
    ChartStatisticProvider() : StatisticProvider() {
        m_showValueAxis = registerProperty(tr("Value axis"), true);
        m_showLegend = registerProperty(tr("Legend"), false);
        m_animationFlags = registerProperty(
            tr("Animations"), QVariant::fromValue<QChart::AnimationOptions>(
                                  QChart::AnimationOptions(
                                      QChart::AnimationOption::AllAnimations)));
        m_theme = registerProperty(
            tr("Theme"),
            QVariant::fromValue<QChart::ChartTheme>(QChart::ChartThemeLight));

        m_chart = new QChart();
        m_chart->setAnimationOptions(
            m_animationFlags->value<QChart::AnimationOptions>());
        m_chart->setTheme(m_theme->value<QChart::ChartTheme>());

        m_chartView = new QChartView(m_chart);
        m_chartView->setRenderHint(QPainter::RenderHint::Antialiasing);
    }

    ~ChartStatisticProvider() {
        delete m_chartView;
        delete m_chart;
    }

    QWidget* getVisualisation() override { return m_chartView; }
    QString getName() const override { return tr("Chart"); }

   protected:
    void onPropertyUpdated(const QString& object, const QString& name) override;
};
