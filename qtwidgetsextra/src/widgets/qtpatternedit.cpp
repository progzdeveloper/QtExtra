#include "qtpatternedit.h"
#include <QtCore/QCoreApplication>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtGui/QIcon>
#include <QtGui/QTextLayout>
#include <QtWidgets/QMenu>

class QtPatternEditPrivate
{
    Q_DECLARE_TR_FUNCTIONS(QtPatternEditPrivate)
public:
    QtPatternEdit::Options options;
    QHash<int, QIcon> iconMap;
    QHash<int, QString> tooltipMap;
    QMenu* menu;
    QAction* optionsAction;

    QtPatternEditPrivate() : options(QtPatternEdit::NoOption) {}

    void initUi(QtPatternEdit *editor);
    void createMenu(QtPatternEdit *editor);
    void createAction(QtPatternEdit *editor, QMenu* menu,
                      QtPatternEdit::Option option, const QString& text);

    QString toolTipText() const;
};



void QtPatternEditPrivate::initUi(QtPatternEdit *editor)
{
    iconMap[QtPatternEdit::NoOption]       = QIcon(":/images/patterns-search.png");
    iconMap[QtPatternEdit::CaseSensitive]  = QIcon(":/images/patterns-aA.png");
    iconMap[QtPatternEdit::WholeWords]     = QIcon(":/images/patterns-HW.png");
    iconMap[QtPatternEdit::RegularExpr]    = QIcon(":/images/patterns-RX.png");
    iconMap[QtPatternEdit::CaseSensitive |
            QtPatternEdit::WholeWords]     = QIcon(":/images/patterns-aAHW.png");
    iconMap[QtPatternEdit::CaseSensitive |
            QtPatternEdit::RegularExpr]    = QIcon(":/images/patterns-aARX.png");
    iconMap[QtPatternEdit::WholeWords |
            QtPatternEdit::RegularExpr]    = QIcon(":/images/patterns-HWRX.png");
    iconMap[QtPatternEdit::CaseSensitive |
            QtPatternEdit::WholeWords    |
            QtPatternEdit::RegularExpr]    = QIcon(":/images/patterns-aAHWRX.png");

    tooltipMap[QtPatternEdit::CaseSensitive] = tr("Match Case Sensitive");
    tooltipMap[QtPatternEdit::WholeWords]    = tr("Match Whole Words");
    tooltipMap[QtPatternEdit::RegularExpr]   = tr("Match Regular Expression");


    optionsAction = editor->addAction(iconMap[options], QLineEdit::LeadingPosition);
    QObject::connect(optionsAction, SIGNAL(triggered()), editor, SLOT(showOptions()));
    createMenu(editor);
}

void QtPatternEditPrivate::createMenu(QtPatternEdit *editor)
{
    menu = new QMenu(editor);
    createAction(editor, menu, QtPatternEdit::CaseSensitive, tr("Case Sensitive"));
    createAction(editor, menu, QtPatternEdit::WholeWords, tr("Whole Words"));
    createAction(editor, menu, QtPatternEdit::RegularExpr, tr("Regular Expression"));
}

void QtPatternEditPrivate::createAction(QtPatternEdit* editor, QMenu *menu,
                                        QtPatternEdit::Option option, const QString &text)
{
    QAction* action = menu->addAction(text);
    action->setCheckable(true);
    action->setChecked(options.testFlag(option));
    action->setData(option);
    QObject::connect(action, SIGNAL(toggled(bool)), editor, SLOT(toggleOption(bool)));
}

QString QtPatternEditPrivate::toolTipText() const
{
    QString tooltip;
    for (int i = 1; i < (QtPatternEdit::RegularExpr+1); i <<= 1)
    {
        if (!options.testFlag(static_cast<QtPatternEdit::Option>(i)))
            continue;

        auto it = tooltipMap.find(i);
        if (it != tooltipMap.end()) {
            tooltip += *it;
            tooltip += '\n';
        }
    }
    if (!tooltip.isEmpty())
        tooltip.remove(tooltip.size()-1, 1);
    return tooltip;
}





QtPatternEdit::QtPatternEdit(QWidget *parent) :
    QLineEdit(parent), d_ptr(new QtPatternEditPrivate)
{
    d_ptr->initUi(this);
}

QtPatternEdit::QtPatternEdit(const QString &pattern, QWidget *parent) :
    QLineEdit(pattern, parent), d_ptr(new QtPatternEditPrivate)
{
    d_ptr->initUi(this);
}

QtPatternEdit::~QtPatternEdit()
{
}

void QtPatternEdit::setOptions(QtPatternEdit::Options opts)
{
    Q_D(QtPatternEdit);
    d->options = opts;
}

QtPatternEdit::Options QtPatternEdit::options() const
{
    Q_D(const QtPatternEdit);
    return d->options;
}

void QtPatternEdit::showOptions()
{
    Q_D(QtPatternEdit);
    d->menu->move(mapToGlobal(rect().bottomLeft()));
    d->menu->exec();
}

void QtPatternEdit::toggleOption(bool on)
{
    Q_D(QtPatternEdit);
    QAction* action = qobject_cast<QAction*>(sender());
    if (action == Q_NULLPTR)
        return;

    Option option = static_cast<Option>(action->data().toInt());
    d->options.setFlag(option, on);
    d->optionsAction->setIcon(d->iconMap[d->options]);
    d->optionsAction->setToolTip(d->toolTipText());
}


