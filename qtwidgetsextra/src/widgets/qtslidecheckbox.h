#ifndef QTSLIDECHECKBOX_H
#define QTSLIDECHECKBOX_H

#include <QtWidgetsExtra>
#include <QCheckBox>

class QTWIDGETSEXTRA_EXPORT QtSlideCheckBox :
        public QCheckBox
{
    Q_OBJECT
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(int duration READ duration WRITE setDuration)
public:
    explicit QtSlideCheckBox(QWidget* parent = Q_NULLPTR);
    explicit QtSlideCheckBox(Qt::Orientation orientation, QWidget* parent = Q_NULLPTR);
    ~QtSlideCheckBox();

    void setSpacing(int m);
    int spacing() const;

    void setAnimated(bool on);
    bool isAnimated() const;

    void setDuration(int msec);
    int duration() const;

    void setOrientation(Qt::Orientation orientation);
    Qt::Orientation orientation() const;

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;

Q_SIGNALS:
    void orientationChanged(Qt::Orientation);

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent* e) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent * e) Q_DECL_OVERRIDE;

    // QAbstractButton interface
protected:
    bool hitButton(const QPoint &pos) const Q_DECL_OVERRIDE;
    void nextCheckState() Q_DECL_OVERRIDE;

private Q_SLOTS:
    void animate(qreal value);

private:
    QT_PIMPL(QtSlideCheckBox)
};


#endif // QTSLIDECHECKBOX_H
