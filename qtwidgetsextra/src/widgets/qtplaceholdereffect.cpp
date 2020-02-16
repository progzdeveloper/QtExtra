#include "qtplaceholdereffect.h"

#include <QApplication>
#include <QEvent>
#include <QPainter>
#include <QTextOption>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

#include <QPointer>
#include <QAbstractItemModel>
#include <QAbstractItemView>

#include <QDebug>

class QtPlaceholderEffectPrivate
{
public:
    QTextDocument document;
    QTextOption textOptions;
    QString placeholderText;
    QPointF displacement;
    qreal opacity;
    QPointer<QAbstractItemModel> model;
    QWidget* viewport;
    int timerId;

    QtPlaceholderEffectPrivate(QGraphicsEffect* effect) :
        document(effect), opacity(1.0), timerId(-1)
    {
        textOptions.setAlignment(Qt::AlignCenter);
        textOptions.setWrapMode(QTextOption::WordWrap);
        document.setDefaultTextOption(textOptions);
    }
    QtPlaceholderEffectPrivate(QGraphicsEffect* effect, QAbstractItemModel *m) :
        document(effect), opacity(1.0), model(m)
    {
        textOptions.setAlignment(Qt::AlignCenter);
        textOptions.setWrapMode(QTextOption::WordWrap);
        document.setDefaultTextOption(textOptions);
    }

    QPointF offsetPoint(const QRectF &rect) const;
    void drawText(QPainter* painter, const QRectF& rect);

};


QPointF QtPlaceholderEffectPrivate::offsetPoint(const QRectF& rect) const
{
    QPointF pos;

    //document.setTextWidth(rect.width());
    QSizeF size = document.size();
    int align = textOptions.alignment();
    if (align & Qt::AlignLeft) {
        pos.setY(0);
    }
    if (align & Qt::AlignRight) {
        pos.setX(qMax(rect.width() - size.width(), qreal(0)));
    }
    if (align & Qt::AlignTop) {
        pos.setX(0);
    }
    if (align & Qt::AlignBottom) {
        pos.setX(qMax(rect.height() - size.height(), qreal(0)));
    }
    if (align & Qt::AlignHCenter) {
        pos.setX(qMax((rect.width() - size.width())/2, qreal(0)));
    }
    if (align & Qt::AlignVCenter) {
        pos.setY(qMax((rect.height() - size.height())/2, qreal(0)));
    }
    return pos;
}

void QtPlaceholderEffectPrivate::drawText(QPainter *painter, const QRectF &rect)
{
    displacement = offsetPoint(rect);
    painter->save();
    painter->setOpacity(opacity);
    painter->translate(displacement);
    document.drawContents(painter, rect);
    painter->restore();
}



QtPlaceholderEffect::QtPlaceholderEffect(QAbstractItemModel *model, const QString &text, QObject *parent) :
    QGraphicsEffect(parent), d_ptr(new QtPlaceholderEffectPrivate(this, model))
{
    setPlaceholderText(text);
}

QtPlaceholderEffect::QtPlaceholderEffect(QAbstractItemView *view, const QString &text, QObject *parent) :
    QGraphicsEffect(parent), d_ptr(new QtPlaceholderEffectPrivate(this))
{
    Q_D(QtPlaceholderEffect);
    if (view) {
        d->model = view->model();
        d->viewport = view->viewport();
        d->viewport->setGraphicsEffect(this);
        d->viewport->installEventFilter(this);
    }
    setPlaceholderText(text);
}

QtPlaceholderEffect::~QtPlaceholderEffect()
{
}

void QtPlaceholderEffect::setOpacity(qreal opacity)
{
    Q_D(QtPlaceholderEffect);
    if (d->opacity != opacity) {
        d->opacity = opacity;
        Q_EMIT opacityChanged(d->opacity);
    }
}

qreal QtPlaceholderEffect::opacity() const
{
    Q_D(const QtPlaceholderEffect);
    return d->opacity;
}

void QtPlaceholderEffect::setModel(QAbstractItemModel *model)
{
    Q_D(QtPlaceholderEffect);
    if (d->model != model) {
        d->model = model;
        update();
    }
}

QAbstractItemModel *QtPlaceholderEffect::model() const
{
    Q_D(const QtPlaceholderEffect);
    return d->model;
}

void QtPlaceholderEffect::setPlaceholderText(const QString &text, int timeout)
{
    Q_D(QtPlaceholderEffect);
    if (d->placeholderText != text)
    {
        d->placeholderText = text;
        if (Qt::mightBeRichText(d->placeholderText)) {
            d->document.setHtml(d->placeholderText);
        } else {
            d->document.setPlainText(d->placeholderText);
        }
        update();
    }

    if (timeout != 0) {
        d->timerId = startTimer(timeout);
    }
}

QString QtPlaceholderEffect::placeholderText() const
{
    Q_D(const QtPlaceholderEffect);
    return d->placeholderText;
}

void QtPlaceholderEffect::setAlignment(Qt::Alignment align)
{
    Q_D(QtPlaceholderEffect);
    d->textOptions.setAlignment(align);
    d->document.setDefaultTextOption(d->textOptions);
    update();
}

Qt::Alignment QtPlaceholderEffect::alignment() const
{
    Q_D(const QtPlaceholderEffect);
    return d->textOptions.alignment();
}

void QtPlaceholderEffect::draw(QPainter *painter)
{
    Q_D(QtPlaceholderEffect);

    painter->save();
    drawSource(painter);
    painter->restore();

    if (d->model &&
       (d->model->rowCount() == 0 || d->model->columnCount() == 0)) {
        d->drawText(painter, boundingRect());
    }
}

void QtPlaceholderEffect::sourceChanged(ChangeFlags flags)
{
    Q_D(QtPlaceholderEffect);
    QGraphicsEffect::sourceChanged(flags);
    d->displacement = d->offsetPoint(boundingRect());
}

bool QtPlaceholderEffect::eventFilter(QObject *watched, QEvent *event)
{
    Q_D(QtPlaceholderEffect);
    if (watched == d->viewport && event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        QPoint pos = mouseEvent->pos() - d->displacement.toPoint();
        int position = d->document.documentLayout()->hitTest(pos, Qt::FuzzyHit);
        QTextCursor cursor(&d->document);
        cursor.setPosition(position);
        cursor.select(QTextCursor::WordUnderCursor);
        QTextCharFormat fmt = cursor.charFormat();
        if (fmt.isAnchor()) {
            Q_EMIT linkActivated(fmt.anchorHref());
        }
    }
    return QObject::eventFilter(watched, event);
}

void QtPlaceholderEffect::timerEvent(QTimerEvent *event)
{
    Q_D(QtPlaceholderEffect);
    if (event->timerId() == d->timerId) {
        killTimer(d->timerId);
        d->timerId = -1;
        d->placeholderText.clear();
        d->document.clear();
        update();
    }
}

