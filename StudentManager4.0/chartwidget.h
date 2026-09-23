#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include <QWidget>
#include <QMap>

class ChartWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ChartWidget(QWidget *parent = nullptr);

    void updateData();
    void setDarkTheme(bool dark);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void drawBarChart(QPainter &painter, const QRect &area);
    void drawPieChart(QPainter &painter, const QRect &area);
    void drawStatistics(QPainter &painter, const QRect &area);

    QMap<int, int> m_ageDist;        // 年龄分布
    QMap<QString, int> m_genderDist; // 性别分布
    int m_totalCount = 0;
    double m_avgAge = 0.0;
    bool m_darkTheme = false;

    // 绘图区域
    QRect m_barChartRect;
    QRect m_pieChartRect;
    QRect m_statsRect;
};

#endif // CHARTWIDGET_H
