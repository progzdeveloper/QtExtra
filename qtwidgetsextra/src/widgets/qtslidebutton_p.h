#include "qtslidebutton.h"
#include <QRect>

class QPainter;
class QPropertyAnimation;


class QtSlideButtonPrivate :
        public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal position READ position WRITE setPosition)
public:
    QtSlideButtonPrivate(QtSlideButton* q);
    ~QtSlideButtonPrivate();

    qreal position() const;

    void drawSlider(QPainter *painter);
    void updateSliderRect(const QSize& size);

public slots:
    void animate(bool);
    void setPosition(qreal value);

private:
    QtSlideButton *q_ptr;
    qreal pos;
    QRectF sliderShape;
    QLinearGradient gradient;

public:
    QPropertyAnimation *animation;
};

