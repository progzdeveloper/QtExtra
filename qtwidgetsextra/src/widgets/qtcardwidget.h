#ifndef QTCARDWIDGET_H
#define QTCARDWIDGET_H

#include <QStyle>
#include <QTextOption>
#include <QGraphicsEffect>
#include <QWidget>
#include <QtWidgetsExtra>


class QTWIDGETSEXTRA_EXPORT QtStyleOptionChip
{
public:
    QTextOption textOptions;
    QFont font;
    QMargins margins;
    QRect rect;
    QSize buttonSize;
    QColor foreground, background;
    QStyle::State state; // reserved
    bool closeable;

    void paint(QPainter* painter, const QString& text) const;
    void paint(QPainter* painter, const QPixmap& pixmap) const;
    void paint(QPainter* painter, const QPixmap& pixmap, const QString& text) const;
};



class QTWIDGETSEXTRA_EXPORT QtGraphicsBageEffect :
        public QGraphicsEffect
{
    Q_OBJECT
public:
    explicit QtGraphicsBageEffect(QObject* parent = Q_NULLPTR);
    ~QtGraphicsBageEffect();

    void setMaximumSize(const QSize& size);
    QSize maximumSize() const;

    void setMargins(const QMargins& margins);
    QMargins margins() const;

    void setBackground(const QColor& color);
    QColor background() const;

    void setForeground(const QColor& color);
    QColor foreground() const;

    void setFont(const QFont& font);
    QFont font() const;

    int counter() const;
    QString text() const;

    QPixmap icon() const;

    void setValue(const QVariant& value);
    QVariant value() const;

    void setAlignment(Qt::Alignment align);
    Qt::Alignment alignment() const;

    void draw(QPainter* painter) Q_DECL_OVERRIDE;

public Q_SLOTS:
    void setCounter(int value);
    void setIcon(const QPixmap& icon);

private:
    QT_PIMPL(QtGraphicsBageEffect)
};

/*
class QtCardWidget :
        public QWidget
{
    Q_OBJECT
public:
    explicit QtCardWidget(QWidget *parent = Q_NULLPTR);
    ~QtCardWidget();

    QPixmap avatar() const;
    QString text() const;
    QString comment() const;

Q_SIGNALS:
    void counterChanged(int value);
    void avatarChanged(const QPixmap&);

public Q_SLOTS:
    void setAvatar(const QPixmap& pixmap);
    void setAvatar(const QImage& image);
    void setText(const QString& text);
    void setComment(const QString& text);
    void setCounter(int value);
    void increment();
    void decrement();
    void reset();

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;
    void changeEvent(QEvent *) Q_DECL_OVERRIDE;

private:
    QT_PIMPL(QtCardWidget)
};
*/


#endif // QTCARDWIDGET_H
