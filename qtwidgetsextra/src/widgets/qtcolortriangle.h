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
    explicit QtColorTriangle(QWidget *parent = Q_NULLPTR);
    ~QtColorTriangle();

    //QSize sizeHint() const;
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    int heightForWidth(int w) const Q_DECL_OVERRIDE;

    void polish();
    QColor color() const;

Q_SIGNALS:
    void colorChanged(const QColor &col);

public Q_SLOTS:
    void setColor(const QColor &col);

protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *) Q_DECL_OVERRIDE;

private:
    QT_PIMPL(QtColorTriangle)
};

#endif
