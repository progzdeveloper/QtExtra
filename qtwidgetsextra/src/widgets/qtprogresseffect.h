#ifndef QTPROGRESSEFFECT_H
#define QTPROGRESSEFFECT_H

#include <QGraphicsEffect>
#include <QtWidgetsExtra>

class QTWIDGETSEXTRA_EXPORT QtProgressEffect :
        public QGraphicsEffect
{
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)
    Q_PROPERTY(int minimum READ minimum WRITE setMinimum)
    Q_PROPERTY(int maximum READ maximum WRITE setMaximum)
    Q_PROPERTY(int progress READ progress WRITE setProgress)
    Q_PROPERTY(QString labelText READ labelText WRITE setLabelText)

public:
    explicit QtProgressEffect(QObject* parent = Q_NULLPTR);
    QtProgressEffect(QWidget* widget, QObject* parent = Q_NULLPTR);
    ~QtProgressEffect();

    qreal opacity() const;
    int minimum() const;
    int maximum() const;
    int progress() const;
    QString labelText() const;


public Q_SLOTS:
    void setOpacity(qreal opacity);
    void setMinimum(int minimum);
    void setMaximum(int maximum);
    void setRange(int minimum, int maximum);
    void setLabelText(const QString& text);
    void setProgress(int value);

Q_SIGNALS:
    void opacityChanged(qreal);

    // QGraphicsEffect interface
protected:
    void draw(QPainter *painter) Q_DECL_OVERRIDE;

    // QObject interface
protected:
    void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;

private:
    QT_PIMPL(QtProgressEffect)
};


#endif // QTPROGRESSEFFECT_H
