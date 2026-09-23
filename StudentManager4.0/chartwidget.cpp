#include "chartwidget.h"
#include "database.h"
#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>
#include <QtMath>

ChartWidget::ChartWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(680, 450);
    updateData();
}

void ChartWidget::updateData()
{
    m_ageDist = Database::instance()->getAgeDistribution();
    m_genderDist = Database::instance()->getGenderDistribution();
    m_totalCount = Database::instance()->getTotalCount();
    m_avgAge = Database::instance()->getAvgAge();
    update();
}

void ChartWidget::setDarkTheme(bool dark)
{
    m_darkTheme = dark;
    update();
}

void ChartWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    int w = width();
    int h = height();
    int margin = 20;

    // 左侧柱状图区域
    m_barChartRect = QRect(margin, margin, w * 0.55 - margin, h * 0.6 - margin);
    // 右侧饼图区域
    m_pieChartRect = QRect(w * 0.55 + margin, margin, w * 0.45 - margin * 2, h * 0.6 - margin);
    // 底部统计区域
    m_statsRect = QRect(margin, h * 0.6 + 10, w - margin * 2, h * 0.4 - margin * 2);
}

void ChartWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 背景
    QColor bgColor = m_darkTheme ? QColor("#1E293B") : QColor("#FFFFFF");
    painter.fillRect(rect(), bgColor);

    if (m_totalCount == 0) {
        painter.setPen(m_darkTheme ? Qt::white : Qt::gray);
        painter.setFont(QFont("Microsoft YaHei", 16));
        painter.drawText(rect(), Qt::AlignCenter, "暂无学生数据，请先添加学生记录");
        return;
    }

    drawBarChart(painter, m_barChartRect);
    drawPieChart(painter, m_pieChartRect);
    drawStatistics(painter, m_statsRect);
}

void ChartWidget::drawBarChart(QPainter &painter, const QRect &area)
{
    painter.save();
    painter.setPen(Qt::NoPen);

    QColor textColor = m_darkTheme ? QColor("#E2E8F0") : QColor("#1E293B");
    QColor barColor = QColor("#4A6CF7");
    QColor barColorHover = QColor("#3730A3");

    // 标题
    painter.setPen(textColor);
    painter.setFont(QFont("Microsoft YaHei", 14, QFont::Bold));
    painter.drawText(area.adjusted(0, 0, 0, -area.height() + 28), Qt::AlignLeft | Qt::AlignVCenter, "年龄分布");

    // 计算绘图区域
    int leftMargin = 50;
    int bottomMargin = 40;
    int topMargin = 40;
    int chartWidth = area.width() - leftMargin - 20;
    int chartHeight = area.height() - topMargin - bottomMargin;

    QRect chartRect(area.x() + leftMargin, area.y() + topMargin, chartWidth, chartHeight);

    if (m_ageDist.isEmpty()) {
        painter.setPen(textColor);
        painter.drawText(chartRect, Qt::AlignCenter, "无数据");
        painter.restore();
        return;
    }

    // 找到最大人数
    int maxCount = 0;
    for (int v : m_ageDist) {
        maxCount = qMax(maxCount, v);
    }
    if (maxCount == 0) maxCount = 1;

    QList<int> ages = m_ageDist.keys();
    std::sort(ages.begin(), ages.end());

    int barCount = ages.size();
    double barSpacing = 10;
    double barWidth = qMax(15.0, (double)chartWidth / barCount - barSpacing);
    double scaleY = (double)chartHeight / maxCount;

    // 绘制坐标轴
    painter.setPen(QPen(textColor, 1));
    painter.drawLine(chartRect.bottomLeft(), chartRect.bottomRight()); // X轴
    painter.drawLine(chartRect.bottomLeft(), chartRect.topLeft());    // Y轴

    // 绘制柱状图
    for (int i = 0; i < barCount; i++) {
        int age = ages[i];
        int count = m_ageDist[age];
        int barHeight = qMax(1, (int)(count * scaleY));

        double x = chartRect.x() + i * (barWidth + barSpacing) + barSpacing / 2;
        double y = chartRect.bottom() - barHeight;

        QRectF barRect(x, y, barWidth, barHeight);

        // 渐变色柱体
        QLinearGradient gradient(barRect.topLeft(), barRect.bottomLeft());
        gradient.setColorAt(0, barColor);
        gradient.setColorAt(1, barColorHover.darker(120));
        painter.setBrush(gradient);
        painter.drawRoundedRect(barRect, 3, 3);

        // 数值标签
        painter.setPen(textColor);
        painter.setFont(QFont("Microsoft YaHei", 10));
        painter.drawText(QRectF(x, y - 22, barWidth, 22), Qt::AlignCenter, QString::number(count));

        // 年龄标签
        painter.drawText(QRectF(x, chartRect.bottom() + 5, barWidth, 28), Qt::AlignCenter, QString::number(age));
    }

    painter.restore();
}

void ChartWidget::drawPieChart(QPainter &painter, const QRect &area)
{
    painter.save();

    QColor textColor = m_darkTheme ? QColor("#E2E8F0") : QColor("#1E293B");

    // 标题
    painter.setPen(textColor);
    painter.setFont(QFont("Microsoft YaHei", 14, QFont::Bold));
    painter.drawText(area.adjusted(0, 0, 0, -area.height() + 28), Qt::AlignLeft | Qt::AlignVCenter, "性别比例");

    if (m_genderDist.isEmpty() || m_totalCount == 0) {
        painter.setPen(textColor);
        painter.drawText(area.adjusted(0, 30, 0, 0), Qt::AlignCenter, "无数据");
        painter.restore();
        return;
    }

    // 计算饼图位置
    int pieSize = qMin(area.width() - 20, area.height() - 80);
    int pieX = area.center().x() - pieSize / 2;
    int pieY = area.y() + 50;
    QRect pieRect(pieX, pieY, pieSize, pieSize);

    // 颜色
    QList<QColor> colors = { QColor("#4A6CF7"), QColor("#F43F5E"), QColor("#10B981") };
    QStringList genders = {"男", "女", "其他"};

    int startAngle = 90 * 16; // 从顶部开始
    int colorIdx = 0;

    // 绘制饼图扇区
    for (const QString &gender : genders) {
        if (!m_genderDist.contains(gender) || m_genderDist[gender] == 0) continue;

        int count = m_genderDist[gender];
        int spanAngle = (int)(360.0 * count / m_totalCount * 16);

        QColor color = colors[colorIdx % colors.size()];
        painter.setBrush(color);
        painter.setPen(QPen(Qt::white, 2));
        painter.drawPie(pieRect, startAngle, spanAngle);

        double midAngle = (startAngle + spanAngle / 2) * M_PI / 180.0 / 16.0;
        double percent = 100.0 * count / m_totalCount;

        // 标签线
        int labelRadius = pieSize / 2 + 20;
        int cx = pieRect.center().x();
        int cy = pieRect.center().y();
        int labelX = cx + (int)(labelRadius * qCos(midAngle));
        int labelY = cy - (int)(labelRadius * qSin(midAngle));

        painter.drawLine(cx + (int)(pieSize / 2 * 0.6 * qCos(midAngle)),
                         cy - (int)(pieSize / 2 * 0.6 * qSin(midAngle)), labelX, labelY);

        painter.setPen(textColor);
        painter.setFont(QFont("Microsoft YaHei", 11));
        QString label = QString("%1: %2人 (%3%)").arg(gender).arg(count).arg(percent, 0, 'f', 1);

        int textAlign = (labelX > cx) ? Qt::AlignLeft : Qt::AlignRight;
        painter.drawText(QRect(labelX + 5, labelY - 10, 120, 20),
                         textAlign | Qt::AlignVCenter, label);

        startAngle += spanAngle;
        colorIdx++;
    }

    // 图例
    int legendY = pieRect.bottom() + 15;
    int legendX = area.x() + 20;
    colorIdx = 0;
    for (const QString &gender : genders) {
        if (!m_genderDist.contains(gender) || m_genderDist[gender] == 0) continue;
        int count = m_genderDist[gender];
        double percent = 100.0 * count / m_totalCount;

        QColor color = colors[colorIdx % colors.size()];
        painter.setBrush(color);
        painter.setPen(Qt::NoPen);
        painter.drawRect(legendX, legendY, 14, 14);

        painter.setPen(textColor);
        painter.setFont(QFont("Microsoft YaHei", 10));
        painter.drawText(legendX + 22, legendY, 220, 16, Qt::AlignLeft | Qt::AlignVCenter,
                        QString("%1: %2人 (%3%)").arg(gender).arg(count).arg(percent, 0, 'f', 1));

        legendY += 24;
        colorIdx++;
    }

    painter.restore();
}

void ChartWidget::drawStatistics(QPainter &painter, const QRect &area)
{
    painter.save();

    QColor textColor = m_darkTheme ? QColor("#E2E8F0") : QColor("#1E293B");
    QColor cardBg = m_darkTheme ? QColor("#1A2332") : QColor("#F1F5F9");

    // 标题
    painter.setPen(textColor);
    painter.setFont(QFont("Microsoft YaHei", 14, QFont::Bold));
    painter.drawText(area.adjusted(0, 0, 0, -area.height() + 28), Qt::AlignLeft | Qt::AlignVCenter, "统计概览");

    // 统计卡片
    int cardWidth = (area.width() - 40) / 4;
    int cardY = area.y() + 35;
    int cardHeight = area.height() - 45;

    struct StatItem {
        QString label;
        QString value;
        QColor valueColor;
    };

    QList<StatItem> items = {
        {"学生总数", QString::number(m_totalCount), QColor("#4A6CF7")},
        {"男生人数", QString::number(m_genderDist.value("男", 0)), QColor("#4A6CF7")},
        {"女生人数", QString::number(m_genderDist.value("女", 0)), QColor("#F43F5E")},
        {"平均年龄", QString::number(m_avgAge, 'f', 1), QColor("#10B981")}
    };

    for (int i = 0; i < items.size(); i++) {
        int cardX = area.x() + i * (cardWidth + 10);

        QRect cardRect(cardX, cardY, cardWidth, cardHeight);

        // 卡片背景
        painter.setPen(Qt::NoPen);
        painter.setBrush(cardBg);
        painter.drawRoundedRect(cardRect, 8, 8);

        // 数值
        painter.setPen(items[i].valueColor);
        painter.setFont(QFont("Microsoft YaHei", 28, QFont::Bold));
        painter.drawText(cardRect.adjusted(0, 0, 0, -22), Qt::AlignCenter, items[i].value);

        // 标签
        painter.setPen(textColor);
        painter.setFont(QFont("Microsoft YaHei", 12));
        painter.drawText(cardRect.adjusted(0, cardHeight - 30, 0, 0), Qt::AlignCenter, items[i].label);
    }

    painter.restore();
}
