#include "qtcolorselector.h"

#include <QStylePainter>
#include <QStyleOptionFrame>
#include <QMouseEvent>
#include <QColorDialog>
#include <QApplication>
#include <QMimeData>

#include <QDragEnterEvent>
#include <QDrag>

/*!
 * \class QtColorSelector
 * \mainclass
 * \inmodule wwWidgets
 * \brief The QtColorSelector class provides a widget allowing to choose a foreground and background colour.
 *
 * \image qwwtwocolorindicator.png QtColorSelector
 *
 * \ingroup colorclasses
 */
/*!
 * \property QtColorSelector::fgColor
 * \brief    foreground colour
 */
/*!
 * \property QtColorSelector::bgColor
 * \brief    background colour
 */
/*!
 *  \fn      void QtColorSelector::fgChanged(const QColor &color)
 *  \brief   This signal is emitted when foreground colour changes to \a color.
 *  \sa      bgChanged()
 */
/*!
 *  \fn      void QtColorSelector::bgChanged(const QColor &color)
 *  \brief   This signal is emitted when background colour changes to \a color.
 *  \sa      fgChanged()
 */
/*!
 *  \fn      void QtColorSelector::fgClicked()
 *  \brief   This signal is emitted when the foreground button was clicked.
 *  \sa      bgClicked(), fgPressed()
 */
/*!
 *  \fn      void QtColorSelector::bgClicked()
 *  \brief   This signal is emitted when the background button was clicked.
 *  \sa      fgClicked(), bgPressed()
 */
/*!
 *  \fn      void QtColorSelector::fgPressed()
 *  \brief   This signal is emitted when the foreground button was pressed.
 *  \sa      bgPressed(), fgClicked()
 */
/*!
 *  \fn      void QtColorSelector::bgPressed()
 *  \brief   This signal is emitted when the background button was pressed.
 *  \sa      fgPressed(), bgClicked()
 */

class QtColorSelectorPrivate
{
public:
    QtColorSelector* q_ptr;
    QColor fg;
    QColor bg;
    QPoint pressPos;
    bool fgP;
    bool bgP;
    bool dragEnabled;

    QtColorSelectorPrivate(QtColorSelector *q) : q_ptr(q),
            fg(Qt::black), bg(Qt::white)
    {
        fgP = false;
        bgP = false;
        dragEnabled = false;
    }

    QRect foregroundRect() const {
        return QRect(0, 0, 2 * q_ptr->width() / 3, 2 * q_ptr->height() / 3);
    }
    QRect backgroundRect() const {
        return QRect(q_ptr->width() / 3, q_ptr->height() / 3, 2 * q_ptr->width() / 3, 2 * q_ptr->height() / 3);
    }
    QRect switchButtonRect() const {
        int x = foregroundRect().right() + 2;
        int y = 2;
        int w = backgroundRect().right() - foregroundRect().right() - 3;
        int h = backgroundRect().top() - 3;
        return QRect(x, y, w, h);
    }

    static QDrag* createDragObject(QWidget *source,
                                   const QColor &color,
                                   const QString &name);
};

/*!
 * Constructs a two color indicator with a given \a parent.
 */
QtColorSelector::QtColorSelector(QWidget *parent) :
    QWidget(parent),
    d_ptr(new QtColorSelectorPrivate(this))
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setAcceptDrops(true);
    setMouseTracking(true);
    setAttribute(Qt::WA_Hover);
    setDragEnabled(true); // ###
}

QtColorSelector::~QtColorSelector()
{
}

/*!
 *  Swaps foreground and background colors.
 */
void QtColorSelector::swapColors() {
    Q_D(QtColorSelector);
    if (d->fg == d->bg)
        return;

    qSwap(d->fg, d->bg);

    emit backgroundChanged(d->bg);
    emit foregroundChanged(d->fg);

    update();
}

/*!
 *  Sets the background color
 */
void QtColorSelector::setBackgroundColor(const QColor &b) {
    Q_D(QtColorSelector);
    if (b == d->bg)
        return;
    d->bg = b;
    emit backgroundChanged(b);
    update();
}

/*!
 *  Sets the foreground color
 */
void QtColorSelector::setForegroundColor(const QColor &f) {
    Q_D(QtColorSelector);
    if (f == d->fg)
        return;
    d->fg = f;
    emit foregroundChanged(f);
    update();
}



const QColor & QtColorSelector::backgroundColor() const {
    Q_D(const QtColorSelector);
    return d->bg;
}

const QColor & QtColorSelector::foregroundColor() const {
    Q_D(const QtColorSelector);
    return d->fg;
}

/*!
 * \internal
 */
QSize QtColorSelector::sizeHint() const {
    return QSize(32, 32).expandedTo(QApplication::globalStrut()*1.5);
}

/*!
 * \internal
 */
QSize QtColorSelector::minimumSizeHint() const {
    QStyleOptionButton option;
    option.initFrom(this);
    return QSize(option.iconSize).expandedTo(QApplication::globalStrut());
}

/*!
 * \brief Paints a single section of the indicator.
 *
 *        The method paints to the rectangle \a rect with \a color using \a painter.
 *
 *        Reimplement to modify how the indicator looks.
 */
void QtColorSelector::paintSection(QPainter * painter, const QRect & rect, const QColor & color)
{
    Q_D(QtColorSelector);

    QPoint pos = mapFromGlobal(QCursor::pos());

    QStyleOptionButton opt;
    opt.initFrom(this);
    opt.rect = rect;


    if ((rect.contains(QPoint(3,3)) && d->fgP) || (rect.contains(QPoint(width()-4, height()-4)) && d->bgP))
        opt.state |= QStyle::State_Sunken;
    else
        opt.state |= QStyle::State_Raised;

    if (rect==d->foregroundRect() && rect.contains(pos)) {
        opt.state |= QStyle::State_MouseOver;
    } else if (rect==d->backgroundRect() && rect.contains(pos) && !d->foregroundRect().contains(pos)) {
        opt.state |= QStyle::State_MouseOver;
    } else opt.state &= ~(QStyle::State_MouseOver);

    //style()->drawControl(QStyle::CE_PushButtonBevel, &opt, painter, this);
    style()->drawControl(QStyle::CE_FocusFrame, &opt, painter, this);

    QRect fillRect = style()->subElementRect(QStyle::SE_PushButtonContents, &opt, this);
    int offset = (opt.rect.height() - fillRect.height())/2;
    fillRect = opt.rect.adjusted(offset,offset,-offset,-offset);
    if (opt.state & QStyle::State_Sunken)
        fillRect.translate(style()->pixelMetric(QStyle::PM_ButtonShiftHorizontal),
                           style()->pixelMetric(QStyle::PM_ButtonShiftVertical));

    painter->setPen(Qt::NoPen);
    painter->setBrush(opt.state & QStyle::State_Enabled ? color : palette().color(QPalette::Disabled, QPalette::Window));

    // check if (0,0) hits the button - if yes -> drawRect, otherwise drawRoundedRect
    painter->setRenderHint(QPainter::Antialiasing);
    //painter->drawRoundedRect(fillRect, 3, 3);
    painter->drawRect(fillRect);
    //painter->drawEllipse(fillRect);
}

/*!
 * \internal
 */
void QtColorSelector::paintEvent(QPaintEvent *)
{
    Q_D(QtColorSelector);
    QPainter painter(this);
    paintSection(&painter, d->backgroundRect(), d->bg);
    paintSection(&painter, d->foregroundRect(), d->fg);
    QStyle* style = this->style();
    if (style == Q_NULLPTR)
        style = qApp->style();

    /*QIcon pix = style->standardIcon(QStyle::SP_BrowserReload);
    pix.paint(&painter, d->switchButtonRect());*/
}


/*!
 * \internal
 */
void QtColorSelector::mousePressEvent(QMouseEvent *ev) {
    Q_D(QtColorSelector);
    d->pressPos = ev->pos();
    if (ev->button() == Qt::LeftButton) {
        if (d->foregroundRect().contains(ev->pos())) {
            d->fgP = true;
            d->bgP = false;
            emit foregroundPressed();
            update();
        } else if (d->backgroundRect().contains(ev->pos())) {
            d->bgP = true;
            d->fgP = false;
            emit backgroundPressed();
            update();
        }
    }
}

/*!
 * \internal
 */
void QtColorSelector::mouseMoveEvent(QMouseEvent *ev) {
    Q_D(QtColorSelector);
    if (d->fgP || d->bgP) {
        if (dragEnabled() && (ev->pos() - d->pressPos).manhattanLength() >= QApplication::startDragDistance()) {
            QColor c = d->fgP ? foregroundColor() : backgroundColor();
            //ColorDrag *drag = new ColorDrag(this, c, c.name());
            //drag->exec();
            QDrag* drag = QtColorSelectorPrivate::createDragObject(this, c, c.name());
            drag->exec();
            d->fgP = d->bgP = false;
            update();
        }
    } else if (testAttribute(Qt::WA_Hover))
        update();
    QWidget::mouseMoveEvent(ev);
}

/*!
 * \internal
 */
void QtColorSelector::mouseReleaseEvent(QMouseEvent *ev) {
    Q_D(QtColorSelector);
    if (ev->button() == Qt::LeftButton)
    {
        /*
        // ### uncomment to enable react on clicking on swap button
        if (d->switchButtonRect().contains(ev->pos())) {
            swapColors();
            update();
            return;
        }
        */
        if (d->fgP && d->foregroundRect().contains(ev->pos())) {
            emit foregroundClicked();
            if (hasMouseTracking()) {
#if QT_VERSION >= 0x040500
                QColor c = QColorDialog::getColor(d->fg, this, tr("Select foreground color"), QColorDialog::ShowAlphaChannel);
#else
                QColor c = QColorDialog::getColor(d->fg, this);
#endif
                if (c.isValid()) {
                    setForegroundColor(c);
                }
            }
        } else if (d->bgP && d->backgroundRect().contains(ev->pos())) {
            emit backgroundClicked();
            if (hasMouseTracking()) {
#if QT_VERSION >= 0x040500
                QColor c = QColorDialog::getColor(d->bg, this, tr("Select background color"), QColorDialog::ShowAlphaChannel);
#else
                QColor c = QColorDialog::getColor(d->bg, this);
#endif
                if (c.isValid()) {
                    setBackgroundColor(c);
                }
            }
        }
        d->fgP = d->bgP = false;
        update();
    }
}

/*!
 * \internal
 */
void QtColorSelector::dragEnterEvent(QDragEnterEvent *ev) {
    if (ev->mimeData()->hasColor()) {
        QColor col(qvariant_cast<QColor>(ev->mimeData()->colorData()));
        if (col.isValid())
            ev->acceptProposedAction();
    } else if (ev->mimeData()->hasFormat("text/plain")) {
        QColor col;
        col.setNamedColor(ev->mimeData()->text());
        if (col.isValid())
            ev->acceptProposedAction();
    } else ev->ignore();
}

/*!
 * \internal
 */
void QtColorSelector::dragMoveEvent(QDragMoveEvent *ev) {
    Q_D(QtColorSelector);
//	bool acc = false;
    if (ev->mimeData()->hasColor()) {
//        QColor col(qvariant_cast<QColor>(ev->mimeData()->colorData()));
//        if (col.isValid())
//            acc = true;
    } else if (ev->mimeData()->hasFormat("text/plain")) {
//        QColor col;
//        col.setNamedColor(ev->mimeData()->text());
//        if (col.isValid())
//            acc = true;
    } else ev->ignore();
    if (d->foregroundRect().intersects(ev->answerRect())) {
        ev->acceptProposedAction();
        ev->accept(d->foregroundRect());
    } else if (d->backgroundRect().intersects(ev->answerRect())) {
        ev->acceptProposedAction();
//         ev->accept(d->backgroundRect());
    } else ev->ignore();
}

/*!
 * \internal
 */
void QtColorSelector::dropEvent(QDropEvent *ev) {
    Q_D(QtColorSelector);
    QColor c;

    if (ev->mimeData()->hasColor())
        c = qvariant_cast<QColor>(ev->mimeData()->colorData());
    else
        c.setNamedColor(ev->mimeData()->text());

    if (ev->source() == this) {
        swapColors();
    } else {
        if (d->foregroundRect().contains(ev->pos())) {
            setForegroundColor(c);
        } else if (d->backgroundRect().contains(ev->pos())) {
            setBackgroundColor(c);
        }
    }

    ev->setDropAction(Qt::CopyAction);
}

void QtColorSelector::wheelEvent(QWheelEvent*)
{
    swapColors();
}

/*!
 * \property QtColorSelector::dragEnabled
 * This property holds whether dragging from the widget is enabled.
 */
bool QtColorSelector::dragEnabled() const {
    Q_D(const QtColorSelector);
    return d->dragEnabled;
}

void QtColorSelector::setDragEnabled(bool v) {
    Q_D(QtColorSelector);
    d->dragEnabled = v;
}

QDrag *QtColorSelectorPrivate::createDragObject(QWidget *source,
                                                const QColor &color,
                                                const QString &name)
{
    int size = source->style()->pixelMetric(QStyle::PM_ButtonIconSize, 0, source);
    QPixmap pixmap(size, size);
    pixmap.fill(color);

    QMimeData *mimeData = new QMimeData;
    mimeData->setColorData(color);
    mimeData->setText(name);
    mimeData->setImageData(pixmap.toImage());

    QDrag* dragObject = new QDrag(source);
    dragObject->setMimeData(mimeData);
    dragObject->setPixmap(pixmap);
    return dragObject;
}
