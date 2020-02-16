#include "qtcolorcombobox.h"

#include <QItemDelegate>
#include <QAbstractItemView>
#include <QStandardItemModel>

#include <QColorDialog>
#include <QMessageBox>

#include <QLayout>
#include <QPushButton>

#include <QStylePainter>

#include <QEvent>
#include <QDragEnterEvent>

#include <QMimeData>

#include "../painting/qtcolorutils.h"


class QtColorComboBoxPrivate
{
public:
    QtColorComboBox* q_ptr;
    bool dialogEnabled;


    QtColorComboBoxPrivate(QtColorComboBox *q) :
        q_ptr(q), dialogEnabled(false) {
    }
};

/*!
 *  \class QtColorComboBox
 *  \brief The QtColorComboBox class provides a combobox that allows to pick colors.
 *  \ingroup colorclasses
 */
/*!
 *  \property   QtColorComboBox::colorDialogEnabled
 *  \brief  	Property holds information whether picking custom colors using a color dialog is enabled.
 */
/*!
 *  \fn QtColorComboBox::activated(const QColor &color)
 *  \brief Signal emitted when \a color is activated
 */


/*!
 * Constructs a color combo box with a given \a parent.
 *
 */
QtColorComboBox::QtColorComboBox(QWidget *parent)
        : QComboBox(parent), d_ptr(new QtColorComboBoxPrivate(this))
{
    connect(this, SIGNAL(activated(int)), this, SLOT(slotActivated(int)));

    view()->installEventFilter(this);
    setAcceptDrops(true);
}

/*!
 * \internal
 */
QtColorComboBox::~QtColorComboBox()
{
}

void QtColorComboBox::addColor(const QColor &color)
{
    addColor(color, standardColorName(color));
}

/*!
 * Adds the specified \a color with a \a name to the list of available colors.
 * \sa insertColor()
 */
void QtColorComboBox::addColor(const QColor & color, const QString & name) {
    insertColor(colorCount(), color, name);
}

/*!
 * \brief Sets \a color as the currently picked color.
 *
 * Color is added to the list of available colors if it's not there yet.
 *
 */
void QtColorComboBox::setCurrentColor(const QColor & color) {
    int i = findData(color, Qt::DecorationRole);
    if (i!=-1) {
        setCurrentIndex(i);
    } else {
        addColor(color);
        setCurrentIndex(count()-1);
    }
}


/*!
 *  Inserts the specified \a color with a \a name in the list of available
 *  colors under specified \a index
 *  \sa addColor()
 */
void QtColorComboBox::insertColor(int index, const QColor & color, const QString & name)
{
    insertItem(index, colorPixmap(color, iconSize()), name);
}

/*!
 * \property QtColorComboBox::currentColor
 * \brief   Currently chosen color.
 * \sa	colors, colorCount
 */
QColor QtColorComboBox::currentColor() const {
    return color(currentIndex());
}

/*!
 * \property QtColorComboBox::colorCount
 * \brief   Number of colors available.
 * \sa	colors, currentColor
 */
int QtColorComboBox::colorCount() const {
    return count();
}

/*!
 * \brief   Returns the color at position \a index
 * \sa	currentColor, colors
 */
QColor QtColorComboBox::color(int index) const {
    return qvariant_cast<QColor>(itemData(index, Qt::DecorationRole));
}

/*!
 * \brief   Fills the list of available colors with standard colors.
 * \sa      colors
 */
void QtColorComboBox::setStandardColors()
{
    clear();
    QStringList clist = QColor::colorNames();
    for(auto it = clist.begin(); it != clist.end(); ++it) {
        addColor(QColor(*it), *it);
    }
}


/*!
 * \brief   Returns whether the color dialog is enabled.
 */
bool QtColorComboBox::isColorDialogEnabled() const {
    Q_D(const QtColorComboBox);
    return d->dialogEnabled;
}

/*!
 * \brief   Enables or disables the color dialog.
 */
void QtColorComboBox::setColorDialogEnabled(bool enabled) {
    Q_D(QtColorComboBox);
    d->dialogEnabled = enabled;
}



/*!
 *  \internal
 *  Reimplemented from QComboBox.
 */
bool QtColorComboBox::eventFilter(QObject * watched, QEvent * e)
{
    if (watched == view()) {
        if (e->type() == QEvent::Show) {
            if (isColorDialogEnabled()) {
                addItem(tr("Select color..."));
                int index = count()-1;
                setItemData(index, Qt::AlignCenter, Qt::TextAlignmentRole);
                setItemData(index, palette().color(QPalette::Button), Qt::BackgroundRole);
                setItemData(index, tr("Choose a custom color"), Qt::ToolTipRole);

            }
            return false;
        }
        if (e->type() == QEvent::Hide) {
            if (isColorDialogEnabled())
                removeItem(count()-1);
            return false;
        }

    }
    return QComboBox::eventFilter(watched, e);
}



/*!
 *  \internal
 *  \reimp
 */
void QtColorComboBox::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasColor())
        event->acceptProposedAction();
    else if (event->mimeData()->hasText()) {
        QColor c(event->mimeData()->text());
        if (c.isValid())
            event->acceptProposedAction();
    }
}

/*!
 *  \internal
 *  \reimp
 */
void QtColorComboBox::dropEvent(QDropEvent *event)
{
    QColor c;
    if (event->mimeData()->hasColor()) {
        c = qvariant_cast<QColor>(event->mimeData()->colorData());
    } else if (event->mimeData()->hasText()) {
        c.setNamedColor(event->mimeData()->text());
    }
    setCurrentColor(c);
}


/*!
 * \property QtColorComboBox::colors
 * \brief list of colors known to the button.
 * \sa colorCount, currentColor
 */
QStringList QtColorComboBox::colors() const
{
    QStringList slist;
    if (count() < 1)
        return slist;
    int n = count();
    for (int i = 0; i < n; i++) {
        slist << itemData(i, Qt::DisplayRole).toString();
    }
    return slist;
}

/*!
 *  \brief Sets a list of colors encoded into strings as available colors
 *
 */
void QtColorComboBox::setColors(const QStringList &map)
{
    clear();
    QColor c;
    for (auto it = map.begin(); it != map.end(); ++it) {
        addColor(QColor(*it), *it);
    }
    slotActivated(0);
    update();
}


void QtColorComboBox::slotActivated(int i)
{
    if (count() < 1)
        return;

    if (isColorDialogEnabled() && i == count()-1) {
        slotPopupDialog();
    }

    QVariant v = itemData(i, Qt::DecorationRole);
    if (v.isValid()) {
        QColor c = qvariant_cast<QColor>(v);
        if (c.isValid())
            emit activated(c);
    }
}

void QtColorComboBox::slotPopupDialog()
{

#if QT_VERSION >= 0x040500
    QColor c = QColorDialog::getColor(currentColor(), this, tr("Choose color"), QColorDialog::ShowAlphaChannel);
#else
    QColor c = QColorDialog::getColor(currentColor(), this);
#endif
    if (c.isValid()) {
        int index = findData(c, Qt::DecorationRole);
        if (index == -1) {
            addColor(c, standardColorName(c));
            index = count() - 1;
        }
        setCurrentIndex(index);
    }
}
