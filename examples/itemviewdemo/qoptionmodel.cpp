#include "qoptionmodel.h"
#include <QApplication>
#include <QString>
#include <QTextStream>
#include <list>

struct QOption
{
    QString expr;
    QString text;
    QString value;
    bool implicit;
    bool required;

    QOption() :
        implicit(false), required(false)
    {
    }
};


class QOptionModelPrivate
{
    Q_DECLARE_TR_FUNCTIONS(QOptionModelPrivate)
public:

    typedef std::list<QOption> QOptionList;
    QOptionList options;
    QOptionModel* q_ptr;

    QOptionModelPrivate(QOptionModel* q) : q_ptr(q) {}

    inline QOption* decodePointer(const QModelIndex& index) const;
    inline QModelIndex encodePointer(int row, int column, QOptionList::const_iterator opt) const;

    inline bool edit(QOption *opt, int column, const QVariant &v);
    inline QVariant itemData(const QOption* opt, int column) const;
    inline QString display(const QOption* opt, int column) const;
    inline QVariant checked(const QOption* opt, int column) const;

    inline QString header(int section) const;
};

QOption *QOptionModelPrivate::decodePointer(const QModelIndex &index) const
{
    return (reinterpret_cast<QOptionList::value_type*>(index.internalPointer()));
}

QModelIndex QOptionModelPrivate::encodePointer(int row, int column, QOptionList::const_iterator opt) const
{
    return q_ptr->createIndex(row, column, const_cast<QOption*>(std::addressof(*opt)));
}

bool QOptionModelPrivate::edit(QOption *opt, int column, const QVariant& v)
{
    switch(column) {
    case QOptionModel::ColumnExpr:
        opt->expr = v.toString();
        opt->implicit = opt->expr.contains('=');
        opt->required = opt->expr.contains('*');
        return true;
    case QOptionModel::ColumnVal:
        opt->value = v.toString();
        opt->required = (opt->value.isEmpty());
        if (!opt->required)
            opt->expr.remove('*');
        return true;
    case QOptionModel::ColumnImp:
        opt->implicit = (v.toInt() == Qt::Checked);
        if (opt->implicit)
            opt->expr.append('=');
        else
            opt->expr.remove('=');
        return true;
    case QOptionModel::ColumnReq:
        opt->required = (v.toInt() == Qt::Checked);
        if (opt->required)
            opt->expr.prepend('*');
        else
            opt->expr.remove('*');
        return true;
    case QOptionModel::ColumnText:
        opt->text = v.toString();
        return true;
    }
    return false;
}

QVariant QOptionModelPrivate::itemData(const QOption *opt, int column) const
{
    switch(column) {
    case QOptionModel::ColumnExpr:
        return opt->expr;
    case QOptionModel::ColumnVal:
        return opt->value;
    case QOptionModel::ColumnImp:
        return opt->implicit;
    case QOptionModel::ColumnReq:
        return opt->required;
    case QOptionModel::ColumnText:
        return opt->text;
    }
    return QVariant();
}

QString QOptionModelPrivate::display(const QOption *opt, int column) const
{
    switch(column) {
    case QOptionModel::ColumnExpr:
        return opt->expr;
    case QOptionModel::ColumnVal:
        return opt->value;
    case QOptionModel::ColumnImp:
        return (opt->implicit ? tr("On") : tr("Off"));
    case QOptionModel::ColumnReq:
        return (opt->required ? tr("On") : tr("Off"));
    case QOptionModel::ColumnText:
        return opt->text;
    }
    return QString();
}

QVariant QOptionModelPrivate::checked(const QOption *opt, int column) const
{
    switch(column) {
    case QOptionModel::ColumnImp:
        return (opt->implicit ? Qt::Checked : Qt::Unchecked);
    case QOptionModel::ColumnReq:
        return (opt->required ? Qt::Checked : Qt::Unchecked);
    }
    return QVariant();
}

inline QString QOptionModelPrivate::header(int section) const
{
    switch(section)
    {
    case QOptionModel::ColumnExpr:
        return tr("Keys");
    case QOptionModel::ColumnVal:
        return tr("Value");
    case QOptionModel::ColumnImp:
        return tr("Implicit");
    case QOptionModel::ColumnReq:
        return tr("Required");
    case QOptionModel::ColumnText:
        return tr("Help Text");
    }
    return QString();
}





QOptionModel::QOptionModel(QObject *parent)
    : QAbstractTableModel(parent), d_ptr(new QOptionModelPrivate(this))
{
}

bool QOptionModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_D(QOptionModel);
    if ( row < -1 || row > static_cast<int>(d->options.size()) )
        return false;

    beginInsertRows(parent, row, row + count - 1);
    d->options.insert(std::next(d->options.begin(), row), count, QOption());
    endInsertRows();
    return true;
}

bool QOptionModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_D(QOptionModel);
    if (row < 0 || row + count > static_cast<int>(d->options.size()))
        return false;

    beginRemoveRows(parent, row, row + count - 1);
    auto first = std::next(d->options.begin(), row);
    auto last = std::next(d->options.begin(), row + count);
    d->options.erase(first, last);
    endRemoveRows();
    return true;
}

bool QOptionModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    Q_D(QOptionModel);
    if (!beginMoveRows(sourceParent, sourceRow, sourceRow + count - 1, destinationParent, destinationChild))
        return false;
    auto first = std::next(d->options.begin(), sourceRow);
    auto last = std::next(d->options.begin(), sourceRow + count);
    d->options.splice(std::next(d->options.begin(), destinationChild), d->options, first, last);
    endMoveRows();
    return true;
}

QVariant QOptionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_D(const QOptionModel);
    if (orientation == Qt::Horizontal) {
        switch (role) {
        case Qt::DisplayRole:
            return d->header(section);
            break;
        case Qt::CheckStateRole:
            if (section ==  ColumnImp || section == ColumnReq)
                return Qt::Checked;
            break;
        default:
            break;
        }
    }

    if (orientation == Qt::Vertical && role == Qt::CheckStateRole)
        return Qt::Checked;

    return QAbstractTableModel::headerData(section, orientation, role);

}

int QOptionModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const QOptionModel);
    if (parent.isValid())
        return 0;

    return d->options.size();
}

int QOptionModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return COLUMN_COUNT;
}

QVariant QOptionModel::data(const QModelIndex &index, int role) const
{
    Q_D(const QOptionModel);
    if (!index.isValid())
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Qt::ToolTipRole:
        return d->display(d->decodePointer(index), index.column());
    case Qt::CheckStateRole:
        return d->checked(d->decodePointer(index), index.column());
    default:
        break;
    }
    return QVariant();
}

bool QOptionModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_D(QOptionModel);

    if (!index.isValid())
        return false;

    int row = index.row();
    int column = index.column();
    if ((row < 0 || row >= static_cast<int>(d->options.size())) ||
        (column < 0 || column >= COLUMN_COUNT))
        return false;

    bool isBoolean = (column == ColumnImp || column == ColumnReq);
    if (data(index, role) == value)
        return false;

    QModelIndex sibling;
    switch(role)
    {
    case Qt::EditRole:
        if (isBoolean)
            return false;
        d->edit(d->decodePointer(index), column, value);
        break;
    case Qt::CheckStateRole:
        if (!isBoolean)
            return false;
        d->edit(d->decodePointer(index), column, value);
        sibling = createIndex(row, ColumnExpr);
        emit dataChanged(sibling, sibling, QVector<int>() << Qt::EditRole);
        break;
    }
    emit dataChanged(index, index, QVector<int>() << role);

    return true;
}

Qt::ItemFlags QOptionModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    switch(index.column())
    {
    case ColumnExpr:
    case ColumnVal:
    case ColumnText:
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    case ColumnImp:
    case ColumnReq:
        return (QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable);
    }
    return Qt::NoItemFlags;
}

QModelIndex QOptionModel::index(int row, int column, const QModelIndex&) const
{
    Q_D(const QOptionModel);
    if ((row >= 0 && row < static_cast<int>(d->options.size())) &&
        (column >= 0 && column < COLUMN_COUNT))
        return d->encodePointer(row, column, std::next(d->options.begin(), row));
    else
        return QModelIndex();
}


void QOptionModel::clear()
{
    Q_D(QOptionModel);
    beginRemoveRows(QModelIndex(), 0, rowCount()-1);
    d->options.clear();
    endRemoveRows();
}

QString QOptionModel::generateCppCode(const QString& className) const
{
    Q_D(const QOptionModel);

    QString result;
    QTextStream buffer(&result);

    buffer << "void initialize_" << className << "(stdx::" << className << "& cmdline)\n{" << endl;
    for (auto it = d->options.begin(); it != d->options.end(); ++it)
    {
        if (it->expr.isEmpty())
            continue;

        buffer << "    cmdline.emplace(\"" << it->expr << "\", ";
        if (!it->value.isEmpty())
            buffer << it->value << ", ";
        buffer << '\"' << it->text << "\");" << endl;
    }
    buffer << "}" << endl;
    return result;
}


