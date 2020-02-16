#ifndef QTOVERVIEWWIDGET_H
#define QTOVERVIEWWIDGET_H

#include <QWidget>
#include <QtWidgetsExtra>

class QAbstractScrollArea;

class QTWIDGETSEXTRA_EXPORT QtOverviewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QtOverviewWidget(QWidget *parent = Q_NULLPTR, Qt::WindowFlags flags = 0);
    ~QtOverviewWidget();

    void setArea(QAbstractScrollArea* area);
    QAbstractScrollArea* area() const;

    // QObject interface
public:
    virtual bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;
    virtual void timerEvent(QTimerEvent*);

    virtual QSize maximumSize() const;

    // QWidget interface
protected:
    virtual void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    virtual void hideEvent(QHideEvent *event) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;
    virtual void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;


protected:
    // drawing content rect can be customized to be style aware
    virtual void drawContentRect(QPainter* painter, const QRect& r);

private Q_SLOTS:
    void updatePixmap();
    void refresh();

private:
    QT_PIMPL(QtOverviewWidget)
};

#endif // QTOVERVIEWWIDGET_H
