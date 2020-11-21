#ifndef QTCOLORSELECTOR_H
#define QTCOLORSELECTOR_H

#include <QWidget>
#include <QtWidgetsExtra>

class QTWIDGETSEXTRA_EXPORT QtColorSelector : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor foregroundColor READ foregroundColor WRITE setForegroundColor)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
    Q_PROPERTY(bool dragEnabled READ dragEnabled WRITE setDragEnabled)
    Q_DISABLE_COPY(QtColorSelector)

public:
    explicit QtColorSelector(QWidget *parent = Q_NULLPTR);
    ~QtColorSelector();

    const QColor &foregroundColor() const;
    const QColor &backgroundColor() const;

    QSize sizeHint() const Q_DECL_OVERRIDE;
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;

    bool dragEnabled() const;
    void setDragEnabled(bool);

public Q_SLOTS:
    void setForegroundColor(const QColor &);
    void setBackgroundColor(const QColor &);

    void swapColors();

Q_SIGNALS:
    void foregroundChanged(const QColor &);
    void backgroundChanged(const QColor &);

    void foregroundClicked();
    void backgroundClicked();

    void foregroundPressed();
    void backgroundPressed();

    // QWidget interface
protected:
    void paintEvent(QPaintEvent*) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent*) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent*) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent*) Q_DECL_OVERRIDE;
    void dragEnterEvent(QDragEnterEvent*) Q_DECL_OVERRIDE;
    void dragMoveEvent(QDragMoveEvent*) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent*) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent*) Q_DECL_OVERRIDE;

protected:
    virtual void paintSection(QPainter *p, const QRect &rect, const QColor &color);

private:
    QT_PIMPL(QtColorSelector)
};

#endif // QTCOLORSELECTOR_H
