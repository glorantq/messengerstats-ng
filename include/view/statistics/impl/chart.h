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

    PropertyAccessor m_showLegend{};
    PropertyAccessor m_animationFlags{};
    PropertyAccessor m_theme{};

    QList<QPair<QString, long long>> m_dataSet{};

    // StatisticProvider interface
   public:
    ChartStatisticProvider() : StatisticProvider() {
        m_showLegend = registerProperty(tr("Legend"), true);
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
        m_chartView->setContextMenuPolicy(
            Qt::ContextMenuPolicy::CustomContextMenu);

        QObject::connect(m_chartView, &QChartView::customContextMenuRequested,
                         [&](const QPoint& point) {
                             this->onChartContextMenuRequested(point);
                         });
    }

    ~ChartStatisticProvider() { delete m_chartView; }

    QWidget* getVisualisation() override { return m_chartView; }
    QString getName() const override { return tr("Chart"); }

   private slots:
    void onChartContextMenuRequested(const QPoint& point) {
        QMenu menu;

        QAction* saveImageAction =
            new QAction(QIcon("://resources/icon/silk/disk_download.png"),
                        tr("Save as image"));

        QObject::connect(saveImageAction, &QAction::triggered, [&]() {
            QPixmap image = m_chartView->grab();

            QString selectedPath = QFileDialog::getSaveFileName(
                m_chartView, tr("Save image"), QDir::homePath(), "*.png");

            if (!selectedPath.isEmpty()) {
                QFile file(selectedPath);
                file.open(QIODevice::WriteOnly | QIODevice::Truncate);
                image.save(&file, "PNG");
                file.close();
            }
        });

        menu.addAction(saveImageAction);

        menu.exec(m_chartView->viewport()->mapToGlobal(point));
    }

   protected:
    void onPropertyUpdated(const QString& object, const QString& name) override;
};
