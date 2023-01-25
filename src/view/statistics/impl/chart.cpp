#include "view/statistics/impl/chart.h"

void ChartStatisticProvider::onPropertyUpdated(const QString& object,
                                               const QString& name) {
    QString ownName = ChartStatisticProvider::getName();
    if (object != ownName) {
        return;
    }

    if (name == m_showLegend.name()) {
        m_chart->legend()->setVisible(m_showLegend->toBool());
    } else if (name == m_showValueAxis.name()) {
        for (auto& axis : m_chart->axes()) {
            if (axis->inherits("QValueAxis")) {
                axis->setVisible(m_showValueAxis->toBool());
            }
        }
    } else if (name == m_animationFlags.name()) {
        m_chart->setAnimationOptions(
            m_animationFlags->value<QChart::AnimationOptions>());
    }
}
