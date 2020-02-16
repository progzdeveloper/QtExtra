#ifndef QTCOLORTRIANGLE_H
#define QTCOLORTRIANGLE_H

#include <QtGui/QImage>
#include <QtWidgets/QWidget>
#include <QtWidgetsExtra>

class QPointF;
struct Vertex;

class QTWIDGETSEXTRA_EXPORT QtColorTriangle : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
    QtColorTriangle(QWidget *parent = 0);
    ~QtColorTriangle();

    //QSize sizeHint() const;
    QSize minimumSizeHint() const;
    int heightForWidth(int w) const;

    void polish();
    QColor color() const;

Q_SIGNALS:
    void colorChanged(const QColor &col);

public Q_SLOTS:
    void setColor(const QColor &col);

protected:
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void keyPressEvent(QKeyEvent *e);
    void resizeEvent(QResizeEvent *);

private:
    QT_PIMPL(QtColorTriangle)
};

#endif
