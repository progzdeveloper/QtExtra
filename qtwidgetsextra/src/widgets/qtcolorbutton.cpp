#include "qtcolorbutton.h"
#include <QtCore/QHash>
#include <QtCore/QSet>
#include <QtCore/QCoreApplication>

#include <QtWidgets/QMenu>
#include <QtWidgets/QAction>
#include <QtWidgets/QWidgetAction>
#include <QtWidgets/QColorDialog>

#include "../painting/qtcolorutils.h"
#include "qtcolorgrid.h"

class QtColorButtonPrivate
{
    Q_DECLARE_TR_FUNCTIONS(QtColorButtonPrivate)
public:
    QtColorButton* q_ptr;
    QSet<QColor> colors;
    QColor color;
    QMenu* menu;
    QtColorButton::PopupStyle popupStyle;

    QtColorButtonPrivate(QtColorButton* q);
    void initUi();

    QAction* createAction(const QString& name);
    QAction* createAction(const QColor& c, const QString& name = QString());

    void createListMenu(const QStringList& names);
    void createListMenu(const QtColorSet& colors);
    void createListMenu(const QtColorSet& colors, const QStringList& names);

    void createGridMenu(const QStringList& names);
    void createGridMenu(const QtColorSet& colors);
    void createGridMenu(const QtColorSet& colors, const QStringList& names);

    void resetMenu();
};

QtColorButtonPrivate::QtColorButtonPrivate(QtColorButton * q) :
    q_ptr(q),
    menu(Q_NULLPTR),
    popupStyle(QtColorButton::GridPopup)
{
}

void QtColorButtonPrivate::initUi()
{
    q_ptr->setMenu(menu);
    q_ptr->setText(color.name());
    q_ptr->setIcon(colorPixmap(color, q_ptr->iconSize()));
    q_ptr->setPopupMode(QToolButton::MenuButtonPopup);
    q_ptr->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    QObject::connect(q_ptr, SIGNAL(clicked()), q_ptr, SLOT(pickColor()));
}

QAction* QtColorButtonPrivate::createAction(const QString &name)
{

    QAction* action = Q_NULLPTR;
    QColor c(name);
    if (c.isValid()) {
       action = new QAction(menu);
       action->setData(c);
       action->setIcon(colorPixmap(c, q_ptr->iconSize()));
       action->setText(name);

       QObject::connect(action, SIGNAL(triggered()), q_ptr, SLOT(colorSelected()));

       colors.insert(c);
    }
    return action;
}

QAction *QtColorButtonPrivate::createAction(const QColor &c, const QString &name)
{

    QAction* action = Q_NULLPTR;
    if (c.isValid()) {
       action = new QAction(menu);
       action->setData(c);
       action->setIcon(colorPixmap(c, q_ptr->iconSize()));
       action->setText(name.isEmpty() ? standardColorName(c) : name);

       QObject::connect(action, SIGNAL(triggered()), q_ptr, SLOT(colorSelected()));
       colors.insert(c);
    }
    return action;
}

void QtColorButtonPrivate::createListMenu(const QStringList& names)
{
    if (menu)
        delete menu;
    menu = new QMenu(q_ptr);
    for (auto it = names.begin(); it != names.end(); ++it) {
        QAction* action = createAction(*it);
        if (action)
            menu->addAction(action);
    }
}

void QtColorButtonPrivate::createListMenu(const QtColorSet& colors)
{
    if (menu)
        delete menu;
    menu = new QMenu(q_ptr);
    for (auto it = colors.begin(); it != colors.end(); ++it)
    {
        QAction* action = createAction(*it);
        if (action)
            menu->addAction(action);
    }
}

void QtColorButtonPrivate::createListMenu(const QtColorSet& colors, const QStringList& names)
{
    if (menu)
        delete menu;
    menu = new QMenu(q_ptr);
    auto nameIt = names.begin();
    for (auto it = colors.begin(); it != colors.end(); ++it, ++nameIt)
    {
        QAction* action = createAction(*it, *nameIt);
        if (action)
            menu->addAction(action);
    }
}

void QtColorButtonPrivate::createGridMenu(const QStringList &names)
{
    if (menu)
        delete menu;
    menu = new QMenu(q_ptr);
    QtColorGrid* grid = new QtColorGrid(menu);
    QtColorSet colors;
    for (auto it = names.begin(); it != names.end(); ++it) {
        colors << QColor(*it);
    }
    int width = 8 * grid->iconSize().width() + grid->iconSize().width() - 3;
    grid->setColors(colors);
    grid->setFixedWidth(width);
    QObject::connect(grid, SIGNAL(colorChanged(QColor)), q_ptr, SLOT(setColor(QColor)));
    QWidgetAction* action = new QWidgetAction(menu);
    action->setDefaultWidget(grid);
    menu->addAction(action);

    menu->setFixedWidth(width);
}

void QtColorButtonPrivate::createGridMenu(const QtColorSet &colors)
{
    if (menu)
        delete menu;
    menu = new QMenu(q_ptr);

    QtColorGrid* grid = new QtColorGrid(menu);
    int width = 8 * grid->iconSize().width() + grid->iconSize().width() - 3;
    grid->setColors(colors);
    grid->setFixedWidth(width);
    QObject::connect(grid, SIGNAL(colorChanged(QColor)), q_ptr, SLOT(setColor(QColor)));

    QWidgetAction* action = new QWidgetAction(menu);
    action->setDefaultWidget(grid);
    menu->addAction(action);

    menu->setFixedWidth(width);
}

void QtColorButtonPrivate::createGridMenu(const QtColorSet &colors, const QStringList &)
{
    if (menu)
        delete menu;
    menu = new QMenu(q_ptr);

    QtColorGrid* grid = new QtColorGrid(menu);
    int width = 8 * grid->iconSize().width() + grid->iconSize().width() - 3;
    grid->setColors(colors);
    grid->setFixedWidth(width);
    QObject::connect(grid, SIGNAL(colorChanged(QColor)), q_ptr, SLOT(setColor(QColor)));

    QWidgetAction* action = new QWidgetAction(menu);
    action->setDefaultWidget(grid);
    menu->addAction(action);

    menu->setFixedWidth(width);
}

void QtColorButtonPrivate::resetMenu()
{
    QtColorSet colorset(colors.size());
    qCopy(colors.begin(), colors.end(), colorset.begin());
    switch (popupStyle) {
    case QtColorButton::GridPopup:
        createGridMenu(colorset);
        menu->addAction(tr("Select color..."), q_ptr, SLOT(pickColor()));
        break;
    case QtColorButton::ListPopup:
        createListMenu(colorset);
        break;
    }
    q_ptr->setMenu(menu);
}


QtColorButton::QtColorButton(QWidget* parent) :
    QToolButton(parent), d_ptr(new QtColorButtonPrivate(this))
{
    Q_D(QtColorButton);
    QtColorSet colors;
    colors << Qt::white << Qt::black
           << Qt::red << Qt::darkRed
           << Qt::green << Qt::darkGreen
           << Qt::blue << Qt::darkBlue
           << Qt::cyan << Qt::darkCyan
           << Qt::magenta << Qt::darkMagenta
           << Qt::yellow << Qt::darkYellow
           << Qt::gray << Qt::darkGray << Qt::lightGray;

    for (auto it = colors.begin(); it != colors.end(); ++it)
        d->colors.insert(*it);

    d->resetMenu();
    d->initUi();
}

QtColorButton::QtColorButton(const QStringList& colorNames, QWidget* parent) :
    QToolButton(parent), d_ptr(new QtColorButtonPrivate(this))
{
    Q_D(QtColorButton);
    for (auto it = colorNames.begin(); it != colorNames.end(); ++it)
        d->colors.insert(QColor(*it));
    d->resetMenu();
    d->initUi();
}

QtColorButton::QtColorButton(const QtColorSet& colorSet, QWidget* parent) : 
    QToolButton(parent), d_ptr(new QtColorButtonPrivate(this))
{
    Q_D(QtColorButton);
    for (auto it = colorSet.begin(); it != colorSet.end(); ++it)
        d->colors.insert(*it);
    d->resetMenu();
    d->initUi();
}

QtColorButton::~QtColorButton()
{
}

void QtColorButton::colorSelected()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        setColor(action->data().value<QColor>().name());
    }
}

void QtColorButton::pickColor()
{
    Q_D(QtColorButton);
    QColor c = QColorDialog::getColor(d->color, this, tr("Select Color"), QColorDialog::ShowAlphaChannel);
    if (c.isValid()) {
        setColor(c);
        if (!d->colors.contains(c) && d->popupStyle != GridPopup) {
            d->menu->addAction(d->createAction(c));
        }
    }
}

void QtColorButton::setColor(const QColor & color)
{
    Q_D(QtColorButton);

    d->color = color;
    setText(standardColorName(d->color));
    setIcon(colorPixmap(d->color, iconSize()));
    if (!d->colors.contains(d->color)) {
        d->menu->addAction(d->createAction(d->color));
    }
    if (d->menu->isVisible())
        d->menu->hide();
    emit colorChanged(d->color);
}

QColor QtColorButton::color() const
{
    Q_D(const QtColorButton);
    return d->color;
}

void QtColorButton::setColors(const QtColorSet & colorSet)
{
    Q_D(QtColorButton);
    d->colors.clear();
    for (auto it = colorSet.begin(); it != colorSet.end(); ++it)
        d->colors.insert(*it);
    d->resetMenu();
}

QtColorSet QtColorButton::colors() const
{
    Q_D(const QtColorButton);
    QtColorSet colorSet;
    QList<QAction*> actions = d->menu->actions();
    for (auto it = actions.begin(); it != actions.end(); ++it) {
        colorSet << (*it)->data().value<QColor>();
    }
    return colorSet;
}

void QtColorButton::setPopupStyle(QtColorButton::PopupStyle style)
{
    Q_D(QtColorButton);
    if (d->popupStyle != style) {
        d->menu->hide();
        d->popupStyle = style;
        d->resetMenu();
        Q_EMIT popupStyleChanged(d->popupStyle);
    }
}

QtColorButton::PopupStyle QtColorButton::popupStyle() const
{
    Q_D(const QtColorButton);
    return d->popupStyle;
}



