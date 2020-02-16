#include "qtmessagelogview.h"
#include "qtmessagelogmodel.h"

#include "../itemviews/delegates/qtrichtextitemdelegate.h"

#include <QSortFilterProxyModel>
#include <QTableView>
#include <QHeaderView>
#include <QBoxLayout>



static inline bool matched(const QVariant& v, const QVariant& what, Qt::MatchFlags flags)
{
    Qt::CaseSensitivity cs = flags & Qt::MatchCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
    QString text;
    // QVariant based matching
    if (flags == Qt::MatchExactly) {
        return (what == v);
    } else {
        if (text.isEmpty())
            text = what.toString();
        QString t = v.toString(); // lazy string initialization
        switch (flags) {
        case Qt::MatchRegExp:
            return (QRegExp(text, cs).exactMatch(t));
        case Qt::MatchWildcard:
            return (QRegExp(text, cs, QRegExp::Wildcard).exactMatch(t));
        case Qt::MatchStartsWith:
            return t.startsWith(text, cs);
        case Qt::MatchEndsWith:
            return t.endsWith(text, cs);
        case Qt::MatchFixedString:
            return (t.compare(text, cs) == 0);
        case Qt::MatchContains:
        default:
            return t.contains(text, cs);
        }
    }
    return false;
}



class QtMessageLogProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit QtMessageLogProxyModel(QObject* parent = Q_NULLPTR) :
        QSortFilterProxyModel(parent) {
    }

    virtual ~QtMessageLogProxyModel() {}

    void setFieldFilter(QtMessageLogView::Field field, const QVariant& value, Qt::MatchFlags flags = Qt::MatchExactly) {
        if (value.isValid())
            filters[fieldColumn(field)] = qMakePair(value, flags);
        else
            filters.remove(fieldColumn(field));
        invalidateFilter();
    }

    const QPair<QVariant, Qt::MatchFlags> fieldFilter(QtMessageLogView::Field field) const {
        auto it = filters.find(fieldColumn(field));
        return (it != filters.end() ? (*it) : qMakePair(QVariant(), Qt::MatchFlags(Qt::MatchExactly)));
    }

    void clearFilters(QtMessageLogView::Fields fields)
    {
        if (fields == QtMessageLogView::None)
            return;

        if (fields == QtMessageLogView::FieldMask) {
            filters.clear();
        } else {
            for (int i = 1, c = 0; i <= QtMessageLogView::Message; i <<= 1, ++c) {
                if (i & fields) {
                    filters.remove(c);
                }
            }
        }
        invalidateFilter();
    }

    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
    {
        for (auto it = filters.begin(); it != filters.end(); ++it) {
            QModelIndex index = sourceModel()->index(sourceRow, it.key(), sourceParent);
            if (!matched(index.data(filterRole()), it->first, it->second)) {
                return false;
            }
        }
        return true;
    }

    static inline int fieldColumn(QtMessageLogView::Field field)
    {
        if (field == QtMessageLogView::None)
            return -1;
        if (field == QtMessageLogView::FieldMask)
            return -1;

        QtMessageLogView::Fields::Int f = field;
        int i = 0;
        for (; f > 0; ++i) {
            f >>= 1;
        }
        return (i > 0 ? (i - 1) : i);
    }

private:
    QHash< int,     QPair<QVariant, Qt::MatchFlags> > filters;
};







class QtMessageLogViewPrivate
{
public:
    QHash<int, QVariant> defaults;
    QRegularExpression regExp;
    QtMessageLogView* q_ptr;
    QtMessageLogModel* model;
    QtMessageLogProxyModel* proxy;
    QTableView* view;

    QtMessageLogView::Fields fields;

    QtMessageLogViewPrivate(QtMessageLogView* q) :
        q_ptr(q) {
    }

    void initUi();
};

void QtMessageLogViewPrivate::initUi()
{
    view = new QTableView(q_ptr);
    model = new QtMessageLogModel(view);
    //proxy = new QSortFilterProxyModel(view);
    proxy = new QtMessageLogProxyModel(view);
    proxy->setSourceModel(model);
    proxy->setSortRole(Qt::DisplayRole);
    proxy->sort(QtMessageLogModel::SectionTimestamp, Qt::DescendingOrder);
    view->setModel(proxy);
    view->setSortingEnabled(true);
    view->setAlternatingRowColors(true);
    view->setSelectionBehavior(QTableView::SelectRows);
    view->setSelectionMode(QTableView::SingleSelection);
    view->horizontalHeader()->setStretchLastSection(true);
    view->horizontalHeader()->setSectionsMovable(true);
    view->verticalHeader()->setDefaultSectionSize(view->fontMetrics().height() + 4);
    view->verticalHeader()->hide();
    view->setItemDelegateForColumn(QtMessageLogModel::SectionMessage, new QtRichTextItemDelegate(view));

    QVBoxLayout* layout = new QVBoxLayout(q_ptr);
    layout->addWidget(view);
    layout->setMargin(0);
}

QtMessageLogView::QtMessageLogView(QWidget *parent) :
    QWidget(parent), d_ptr(new QtMessageLogViewPrivate(this))
{
    Q_D(QtMessageLogView);
    setDefaultValue(Level, 0);
    setDefaultValue(Code, -1);
    setDefaultValue(Category, tr("Uncategorized"));
    d->initUi();
}

QtMessageLogView::~QtMessageLogView()
{
}

QAbstractItemModel *QtMessageLogView::model() const
{
    Q_D(const QtMessageLogView);
    return d->proxy;
}


void QtMessageLogView::setRegExp(const QRegularExpression &re)
{
    Q_D(QtMessageLogView);
    d->regExp = re;
}

QRegularExpression QtMessageLogView::regExp() const
{
    Q_D(const QtMessageLogView);
    return d->regExp;
}

void QtMessageLogView::setRotationLimit(uint limit)
{
    Q_D(QtMessageLogView);
    d->model->setRotationLimit(limit);
}

uint QtMessageLogView::rotationLimit() const
{
    Q_D(const QtMessageLogView);
    return d->model->rotationLimit();
}

void QtMessageLogView::setVisibleFields(Fields f)
{
    Q_D(QtMessageLogView);
    if (d->fields == f)
        return;

    d->fields = f;
    for (int i = 1, section = 0; i <= Message; i<<=1, ++section) {
        d->view->horizontalHeader()->setSectionHidden(section, !(i & f));
    }
    Q_EMIT visibleFieldsChanged(f);
}

QtMessageLogView::Fields QtMessageLogView::visibleFields() const
{
    Q_D(const QtMessageLogView);
    return d->fields;
}

void QtMessageLogView::setDefaultValue(Field field, const QVariant &value)
{
    Q_D(QtMessageLogView);
    d->defaults[field] = value;
}

QVariant QtMessageLogView::defaultValue(Field field) const
{
    Q_D(const QtMessageLogView);
    return d->defaults[field];
}

void QtMessageLogView::sort()
{
    Q_D(QtMessageLogView);
    d->view->sortByColumn(QtMessageLogModel::SectionTimestamp, Qt::DescendingOrder);
}

void QtMessageLogView::setFieldFilter(Field f, const QVariant &value, Qt::MatchFlags flags)
{
    Q_D(QtMessageLogView);
    d->proxy->setFieldFilter(f, value, flags);
}

void QtMessageLogView::setCategoryFilter(const QString &pattern, Qt::MatchFlags flags)
{
    setFieldFilter(Category, pattern.isEmpty() ? QVariant() : QVariant(pattern), flags);
}

QString QtMessageLogView::categoryFilter() const
{
    return fieldFilter(Category).first.toString();
}

void QtMessageLogView::setMessageFilter(const QString &pattern, Qt::MatchFlags flags)
{
    setFieldFilter(Message, pattern.isEmpty() ? QVariant() : QVariant(pattern), flags);
}

QString QtMessageLogView::messageFilter() const
{
    return fieldFilter(Message).first.toString();
}

QPair<QVariant, Qt::MatchFlags> QtMessageLogView::fieldFilter(QtMessageLogView::Field f) const
{
    Q_D(const QtMessageLogView);
    return d->proxy->fieldFilter(f);
}

void QtMessageLogView::clearFilters(Fields fields)
{
    Q_D(QtMessageLogView);
    d->proxy->clearFilters(fields);
}

void QtMessageLogView::clear()
{
    Q_D(QtMessageLogView);
    d->model->clear();
}

void QtMessageLogView::message(const QDateTime &timestamp, int level, int code, const QString& category, const QString& text)
{
    Q_D(QtMessageLogView);
    d->model->message(level, code, category, text, timestamp);
}

void QtMessageLogView::message(int level, int code, const QString& category, const QString& text)
{
    this->message(QDateTime::currentDateTime(), level, code, category, text);
}

void QtMessageLogView::message(int level, int code, const QString &text)
{
    this->message(QDateTime::currentDateTime(),
                  level, code,
                  defaultValue(Category).toString(),
                  text);
}

void QtMessageLogView::message(int level, const QString &text)
{
    this->message(QDateTime::currentDateTime(),
                  level,
                  defaultValue(Code).toInt(),
                  defaultValue(Category).toString(),
                  text);
}

void QtMessageLogView::message(const QString &category, const QString &text)
{
    this->message(QDateTime::currentDateTime(),
                  defaultValue(Level).toInt(),
                  defaultValue(Code).toInt(),
                  category, text);
}

void QtMessageLogView::message(const QString &text)
{
    this->message(QDateTime::currentDateTime(),
                  defaultValue(Level).toInt(),
                  defaultValue(Code).toInt(),
                  defaultValue(Category).toString(),
                  text);
}

void QtMessageLogView::feed(const QString &line)
{
    Q_D(QtMessageLogView);
    QRegularExpressionMatch match = d->regExp.match(line);
    if (match.hasMatch()) {
        interpret(match);
    }
}

void QtMessageLogView::interpret(const QRegularExpressionMatch &match)
{
    if (match.lastCapturedIndex() != (QtMessageLogModel::MaxSection + 1))
        return;

    this->message(this->timestamp(match),
                  this->level(match),
                  this->code(match),
                  this->category(match),
                  this->text(match));
}

QDateTime QtMessageLogView::timestamp(const QRegularExpressionMatch &match) const
{
    return QDateTime::fromString(match.captured(QtMessageLogModel::SectionTimestamp + 1));
}

int QtMessageLogView::level(const QRegularExpressionMatch &match) const
{
    return match.capturedRef(QtMessageLogModel::SectionLevel + 1).toInt();
}

int QtMessageLogView::code(const QRegularExpressionMatch &match) const
{
    return match.capturedRef(QtMessageLogModel::SectionCode + 1).toInt();
}

QString QtMessageLogView::category(const QRegularExpressionMatch &match) const
{
    return match.captured(QtMessageLogModel::SectionCategory + 1);
}

QString QtMessageLogView::text(const QRegularExpressionMatch &match) const
{
    return match.captured(QtMessageLogModel::SectionMessage + 1);
}


#include "qtmessagelogview.moc"
