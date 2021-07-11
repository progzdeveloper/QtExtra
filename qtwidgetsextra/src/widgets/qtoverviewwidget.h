#ifndef QTOVERVIEWWIDGET_H
#define QTOVERVIEWWIDGET_H

#include <QWidget>
#include <QtWidgetsExtra>

class QAbstractScrollArea;

class QTWIDGETSEXTRA_EXPORT QtOverviewWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool updatable READ isUpdatable WRITE setUpdatable)
    Q_PROPERTY(int updateInterval READ updateInterval WRITE setUpdateInterval NOTIFY updateIntervalChanged)
public:
    explicit QtOverviewWidget(QWidget *parent = Q_NULLPTR, Qt::WindowFlags flags = Qt::WindowFlags(0));
    ~QtOverviewWidget();

    void setArea(QAbstractScrollArea* area);
    QAbstractScrollArea* area() const;

    void setUpdatable(bool on);
    bool isUpdatable();

    void setUpdateInterval(int msec);
    int updateInterval() const;

    // QObject interface
protected:
    bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;

    void showEvent(QShowEvent* event) Q_DECL_OVERRIDE;
    void hideEvent(QHideEvent *event) Q_DECL_OVERRIDE;

    void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;

    void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

protected:
    // drawing content rect can be customized to be style aware
    virtual void drawContentRect(QPainter* painter, const QRect& r);

private Q_SLOTS:
    void updatePixmap();
    void updateContentRect();
    void refresh();

Q_SIGNALS:
    void updateIntervalChanged(int);

private:
    QT_PIMPL(QtOverviewWidget)
};

#endif // QTOVERVIEWWIDGET_H
