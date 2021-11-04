#ifndef QTPLACEHOLDEREFFECT_H
#define QTPLACEHOLDEREFFECT_H

#include <QGraphicsEffect>
#include <QtWidgetsExtra>

class QAbstractItemModel;
class QAbstractItemView;

class QTWIDGETSEXTRA_EXPORT QtPlaceholderEffect :
        public QGraphicsEffect
{
    Q_OBJECT
    Q_PROPERTY(QString placeholderText READ placeholderText WRITE setPlaceholderText)
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)
public:
    QtPlaceholderEffect(QAbstractItemModel* model, const QString& text = QString(), QObject* parent = Q_NULLPTR);
    QtPlaceholderEffect(QAbstractItemView* view, const QString& text = QString(), QObject* parent = Q_NULLPTR);
    ~QtPlaceholderEffect();

    void setOpacity(qreal opacity);
    qreal opacity() const;

    void setModel(QAbstractItemModel* model);
    QAbstractItemModel* model() const;

    void setPlaceholderText(const QString& text, int timeout = 0);
    QString placeholderText() const;

    void setAlignment(Qt::Alignment align);
    Qt::Alignment alignment() const;

    // QGraphicsEffect interface
protected:
    void draw(QPainter *painter) Q_DECL_OVERRIDE;
    void sourceChanged(ChangeFlags flags) Q_DECL_OVERRIDE;

    // QObject interface
protected:
    bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;
    void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void opacityChanged(qreal);
    void linkActivated(const QString&);

private:
    QT_PIMPL(QtPlaceholderEffect)
};
#endif // QTPLACEHOLDEREFFECT_H
