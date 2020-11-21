#ifndef QTHSVRECTPICKER_H
#define QTHSVRECTPICKER_H

#include <QFrame>

#include <QtWidgetsExtra>

class QTWIDGETSEXTRA_EXPORT QtHsvRectPicker :
        public QFrame
{
    Q_OBJECT
    Q_PROPERTY(int minimumHue READ minimumHue WRITE setMinimumHue)
    Q_PROPERTY(int maximumHue READ maximumHue WRITE setMaximumHue)
    Q_PROPERTY(int minimumSat READ minimumSat WRITE setMinimumSat)
    Q_PROPERTY(int maximumSat READ maximumSat WRITE setMaximumSat)
    Q_PROPERTY(int value READ value WRITE setValue)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_DISABLE_COPY(QtHsvRectPicker)

public:
    explicit QtHsvRectPicker(QWidget *parent = Q_NULLPTR);
    ~QtHsvRectPicker();

    QSize sizeHint() const Q_DECL_OVERRIDE;
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;

    int value() const;
    QColor color() const;
    int minimumSat() const;
    int maximumSat() const;
    int minimumHue() const;
    int maximumHue() const;
    void setHueRange(int mi, int ma);
    void setSatRange(int mi, int ma);

public Q_SLOTS:
    void setValue(int v);
    void setMinimumSat(int v);
    void setMaximumSat(int v);
    void setMinimumHue(int h);
    void setMaximumHue(int h);
    void setColor(const QColor &c);

Q_SIGNALS:
    void valueChanged(int);
    void minimumHueChanged(int);
    void maximumHueChanged(int);
    void minimumSatChanged(int);
    void maximumSatChanged(int);
    void satRangeChanged(int, int);
    void hueRangeChanged(int, int);
    void colorChanged(const QColor&);

protected:
    virtual void drawCrosshair(QPainter *painter, const QPoint &pt);

    void paintEvent(QPaintEvent *pe) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *) Q_DECL_OVERRIDE;
private:
    QT_PIMPL(QtHsvRectPicker)
};

#endif // QTHSVRECTPICKER_H
