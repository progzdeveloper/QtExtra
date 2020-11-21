#ifndef QTHSVRADIALPICKER_H
#define QTHSVRADIALPICKER_H

#include <QWidget>
#include <QtWidgetsExtra>

class QTWIDGETSEXTRA_EXPORT QtHsvRadialPicker :
        public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_DISABLE_COPY(QtHsvRadialPicker)

public:
    explicit QtHsvRadialPicker(QWidget *parent = Q_NULLPTR);
    ~QtHsvRadialPicker();

    int value() const;

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;

    const QColor color() const;

public Q_SLOTS:
    void setValue(int v);
    void setColor(const QColor &c);

Q_SIGNALS:
    void valueChanged(int);
    void colorChanged(QColor);

    // QWidget interface
protected:
    void paintEvent(QPaintEvent*) Q_DECL_OVERRIDE;
    void showEvent(QShowEvent *e) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *me) Q_DECL_OVERRIDE;
    void changeEvent(QEvent *ce) Q_DECL_OVERRIDE;

protected:
    virtual void drawCrosshair(QPainter *painter, const QPoint &pt);

private:
    QT_PIMPL(QtHsvRadialPicker)


};

#endif // QTHSVRADIALPICKER_H
