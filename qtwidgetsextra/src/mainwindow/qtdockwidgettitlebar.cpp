#include <QApplication>
#include <QMenu>
#include <QAction>
#include <QStyle>
#include <QStyleOptionFocusRect>
#include <QIcon>
#include <QPixmap>
#include <QLabel>
#include <QToolButton>
#include <QBoxLayout>
#include <QSignalMapper>
#include <QStylePainter>
#include <QMap>
#include <QPalette>

#include "qtdockwidgettitlebar.h"
#include "qtdockwidget.h"

static const char * const qt_pin_xpm[] = {
    "10 10 2 1",
    "# c #000000",
    ". c None",
    "...#####..",
    "...#..##..",
    "...#..##..",
    "...#..##..",
    "...#..##..",
    "...#..##..",
    "..#######.",
    ".....#....",
    ".....#....",
    ".....#...."};

static const char * const qt_unpin_xpm[] = {
    "10 10 2 1",
    "# c #000000",
    ". c None",
    "..........",
    "...#......",
    "...#######",
    "...#.....#",
    "####.....#",
    "...#######",
    "...#######",
    "...#......",
    "..........",
    ".........."};

static const char * const qt_close_xpm[] = {
    "10 10 2 1",
    "# c #000000",
    ". c None",
    "..........",
    ".##....##.",
    "..##..##..",
    "...####...",
    "....##....",
    "...####...",
    "..##..##..",
    ".##....##.",
    "..........",
    ".........."};

static const char * const qt_normalizeup_xpm[] = {
    "10 10 2 1",
    "# c #000000",
    ". c None",
    "...######.",
    "...######.",
    "...#....#.",
    ".######.#.",
    ".######.#.",
    ".#....###.",
    ".#....#...",
    ".#....#...",
    ".######...",
    ".........."};


static const char * const qt_unshade_xpm[] = {
    "10 10 2 1",
    "# c #000000",
    ". c None",
    "..........",
    "..........",
    "..........",
    ".#######..",
    "..#####...",
    "...###....",
    "....#.....",
    "..........",
    "..........",
    ".........."};



enum QtDockWidgetButton
{
    UnshadeButton = 0,
    AutoHideButton = 1,
    UndockButton = 2,
    CloseButton = 3
};
typedef QMap<int, QToolButton*> QToolButtonMap;


class QtDockWidgetTitleBarPrivate
{
    Q_DECLARE_TR_FUNCTIONS(QtDockWidgetTitleBarPrivate)
public:
    QtDockWidgetTitleBar *q_ptr;
    QBoxLayout *layout;
    QSignalMapper *signalMapper;
    QLabel *label;
    QToolButtonMap buttons;
    QAction *autoHideAct;
    QAction *floatAct;
    QAction *closeAct;
    QPalette palette;

    inline void createPalette();
    inline QString buttonToolTip(QtDockWidgetButton type) const;
    inline QIcon buttonIcon(QtDockWidgetButton type, bool autoHide) const;
    inline QToolButton *createButton(QtDockWidgetButton type, bool autoHide) const;
    inline void updateButtons(bool autoHide);

    QMenu* createMenu(QtDockWidget *parent);
    void createButtons(QtDockWidget *parent);
    void createTitleBar(QtDockWidget *parent);
    void createLayout(QtDockWidget *parent);

    void enableButtons(bool on);
};

inline void QtDockWidgetTitleBarPrivate::createPalette()
{
    palette = q_ptr->palette();
    palette.setColor(QPalette::Active, QPalette::Highlight, QColor(110, 165, 255));
    q_ptr->setPalette(palette);
}

inline QIcon QtDockWidgetTitleBarPrivate::buttonIcon( QtDockWidgetButton type, bool autoHide ) const
{
    switch (type)
    {
    case AutoHideButton:
        return QPixmap(autoHide ? qt_unpin_xpm : qt_pin_xpm);
    case UndockButton:
        return QPixmap(qt_normalizeup_xpm);
    case CloseButton:
        return QPixmap(qt_close_xpm);
    case UnshadeButton:
        return QIcon();
    }
    return QIcon();
}

inline QString QtDockWidgetTitleBarPrivate::buttonToolTip( QtDockWidgetButton type ) const
{
    switch (type)
    {
    case AutoHideButton:
        return tr("Auto Hide");
    case UndockButton:
        return tr("Dock");
    case CloseButton:
        return tr("Close");
    case UnshadeButton:
        return tr("Window Position");
    }
    return QString();
}

QMenu* QtDockWidgetTitleBarPrivate::createMenu(QtDockWidget *parent)
{
    QMenu *menu = new QMenu(q_ptr);
    autoHideAct = menu->addAction(buttonToolTip(AutoHideButton), parent, SLOT(setAutoHide(bool)));
    autoHideAct->setCheckable(true);
    autoHideAct->setChecked(parent->isAutoHide());

    floatAct = menu->addAction(buttonToolTip(UndockButton), parent, SLOT(setFloating(bool)));
    floatAct->setCheckable(true);
    floatAct->setChecked(parent->isFloating());

    closeAct = menu->addAction(buttonToolTip(CloseButton), parent, SLOT(hide()));

    return menu;
}

void QtDockWidgetTitleBarPrivate::createButtons(QtDockWidget *parent)
{
    signalMapper = new QSignalMapper(q_ptr);
    for (int i = 0; i <= CloseButton; ++i) {
        buttons[i] = createButton(static_cast<QtDockWidgetButton>(i), parent->isAutoHide());
        signalMapper->setMapping(buttons[i], i);
    }
    QObject::connect(signalMapper, SIGNAL(mapped(int)), q_ptr, SLOT(clicked(int)));
    buttons[UnshadeButton]->setMenu(createMenu(parent));
    buttons[UnshadeButton]->setPopupMode(QToolButton::InstantPopup);
}

QToolButton *QtDockWidgetTitleBarPrivate::createButton(QtDockWidgetButton type, bool autoHide) const
{
    QToolButton *button = new QToolButton(q_ptr);
    button->setIcon(buttonIcon(type, autoHide));
    button->setToolTip(buttonToolTip(type));
    button->setAutoRaise(true);
    button->setFixedSize(16, 16);
    QObject::connect(button, SIGNAL(clicked()), signalMapper, SLOT(map()));
    return button;
}

void QtDockWidgetTitleBarPrivate::createLayout(QtDockWidget *parent)
{
    layout = new QBoxLayout(parent->features() & QDockWidget::DockWidgetVerticalTitleBar ? QBoxLayout::BottomToTop : QBoxLayout::LeftToRight, q_ptr);
    layout->addWidget(label);
    QToolButtonMap::iterator it = buttons.begin();
    for (; it != buttons.end(); ++it)
        layout->addWidget(*it);
    layout->setSpacing(0);
    layout->setMargin(1);
    layout->setContentsMargins(3, 1, 3, 1);
}

void QtDockWidgetTitleBarPrivate::createTitleBar(QtDockWidget *dw)
{
    Q_ASSERT(dw != 0);
    label = new QLabel(dw->windowTitle(), q_ptr);
    createButtons(dw);
    createLayout(dw);
    QObject::connect(dw, SIGNAL(featuresChanged(QDockWidget::DockWidgetFeatures)),
                     q_ptr, SLOT(changeFeatures(QDockWidget::DockWidgetFeatures)));
    QObject::connect(dw, SIGNAL(autoHideChanged(bool)),
                     q_ptr, SLOT(updateButtons(bool)));
}

void QtDockWidgetTitleBarPrivate::enableButtons( bool on )
{
    buttons[AutoHideButton]->setEnabled(on);
    buttons[UndockButton]->setEnabled(on);
    buttons[UnshadeButton]->setEnabled(on);
}

inline void QtDockWidgetTitleBarPrivate::updateButtons(bool autoHide)
{
    QtDockWidgetButton buttonType;
    QToolButtonMap::iterator it = buttons.begin();
    for (; it != buttons.end(); ++it) {
        buttonType = static_cast<QtDockWidgetButton>(it.key());
        (*it)->setIcon( buttonIcon(buttonType, autoHide) );
    }
}




QtDockWidgetTitleBar::QtDockWidgetTitleBar( QWidget *parent /*= 0*/, Qt::WindowFlags f /*= 0*/) 
    : QWidget(parent, f), d_ptr(new QtDockWidgetTitleBarPrivate)
{
    d_ptr->q_ptr = this;
    d_ptr->createTitleBar(qobject_cast<QtDockWidget*>(parent));
    d_ptr->createPalette();
    setMouseTracking(true);
}

QtDockWidgetTitleBar::~QtDockWidgetTitleBar()
{
    delete d_ptr;
}

void QtDockWidgetTitleBar::clicked( int button )
{
    switch(button)
    {
    case AutoHideButton:
        autoHide();
        break;
    case UndockButton:
        undock();
        break;
    case CloseButton:
        closeWidget();
        break;
    }
}

void QtDockWidgetTitleBar::updateButtons(bool autoHide)
{
    Q_D(QtDockWidgetTitleBar);
    d->updateButtons(autoHide);
}

void QtDockWidgetTitleBar::closeWidget()
{
    QtDockWidget *dw = qobject_cast<QtDockWidget*>(parentWidget());
    Q_ASSERT(dw != 0);
    dw->setAutoHide(false);
    dw->close();
}

void QtDockWidgetTitleBar::autoHide()
{
    QtDockWidget *dw = qobject_cast<QtDockWidget*>(parentWidget());
    Q_ASSERT(dw != 0);
    if (dw->isFloating())
        dw->setFloating(false);
    dw->setAutoHide(!dw->isAutoHide());
    if (dw->isAutoHide())
        dw->hide();
}

void QtDockWidgetTitleBar::undock()
{
    QtDockWidget *dw = qobject_cast<QtDockWidget*>(parentWidget());
    Q_ASSERT(dw != 0);
    dw->setFloating(!dw->isFloating());
    dw->setAutoHide(false);
}

void QtDockWidgetTitleBar::changeFeatures( QDockWidget::DockWidgetFeatures features )
{
    Q_UNUSED(features)
    Q_D(QtDockWidgetTitleBar);
    QDockWidget *dw = qobject_cast<QDockWidget*>(parentWidget());
    Q_ASSERT(dw != 0);
    d->enableButtons(dw->isFloating());
}

void QtDockWidgetTitleBar::paintEvent( QPaintEvent* )
{
    /*QStylePainter painter(this);
    QStyleOptionFocusRect option;
    option.initFrom(this);
    option.backgroundColor = palette().color(QPalette::ToolTipBase);
    painter.drawPrimitive (QStyle::PE_FrameFocusRect, option);
    QWidget::paintEvent(e);*/

    QStylePainter painter(this);
    QRect r = rect().adjusted(0, 0, -1, -1);
    painter.fillRect(r, palette().brush(isActiveWindow() ? QPalette::Active : QPalette::Inactive , QPalette::Highlight));
    painter.setPen(Qt::gray);
    painter.drawRect(r.adjusted(0, 0, -1, -1));
}

