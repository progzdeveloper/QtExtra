#include "qtmessagelogmodel.h"
#include <vector>

struct LogRecord
{
    QDateTime timestamp;
    QString category;
    QString message;
    int level;
    int code;

    LogRecord() :
        level(0), code(-1) {
    }

    LogRecord(int aLevel, int aCode, const QString& aCategory, const QString& aText, const QDateTime& aTimestamp) :
        timestamp(aTimestamp), category(aCategory), message(aText), level(aLevel), code(aCode) {
    }
};



class QtMessageLogModelPrivate
{
public:
    std::vector<LogRecord> recordCache;
    int current;
    int maxSize;

    QtMessageLogModelPrivate(int size) : current(0), maxSize(size) {
        recordCache.reserve(size);
    }

    bool validate(const QModelIndex& index) const;
    QVariant display(int row, int column) const;
    QVariant value(int row, int column) const;
    QVariant tooltip(int row, int column) const;
};

bool QtMessageLogModelPrivate::validate(const QModelIndex &index) const
{
    int row = index.row();
    if (row < 0 || row >= (int)recordCache.size())
        return false;

    int column = index.column();
    if (column < 0 || column > QtMessageLogModel::MaxSection)
        return false;
    return true;
}

QVariant QtMessageLogModelPrivate::display(int row, int column) const
{
    const LogRecord& r = recordCache[row];
    switch(column)
    {
    case QtMessageLogModel::SectionLevel:
        return r.level;
    case QtMessageLogModel::SectionCode:
        return r.code;
    case QtMessageLogModel::SectionTimestamp:
        return r.timestamp;
    case QtMessageLogModel::SectionCategory:
        return r.category;
    case QtMessageLogModel::SectionMessage:
        return r.message;
    }
    return QVariant();
}

QVariant QtMessageLogModelPrivate::value(int row, int column) const
{
    return display(row, column);
}

QVariant QtMessageLogModelPrivate::tooltip(int row, int column) const
{
    return display(row, column);
}



QtMessageLogModel::QtMessageLogModel(QObject *parent) :
    QAbstractTableModel(parent), d_ptr(new QtMessageLogModelPrivate(2048))
{
}

QtMessageLogModel::QtMessageLogModel(uint maxSize, QObject *parent):
    QAbstractTableModel(parent), d_ptr(new QtMessageLogModelPrivate(maxSize))
{
}

QtMessageLogModel::~QtMessageLogModel()
{
}

QVariant QtMessageLogModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical)
        return QAbstractTableModel::headerData(section, orientation, role);

    if (role == Qt::DisplayRole && (section >= 0 && section < 5))
    {
        switch(section)
        {
        case SectionLevel:
            return tr("Level");
        case SectionCode:
            return tr("Code");
        case SectionTimestamp:
            return tr("Timestamp");
        case SectionCategory:
            return tr("Category");
        case SectionMessage:
            return tr("Message");
        }
    }
    return QVariant();
}

int QtMessageLogModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    Q_D(const QtMessageLogModel);
    return d->recordCache.size();
}

int QtMessageLogModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return MaxSection;
}

QVariant QtMessageLogModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    Q_D(const QtMessageLogModel);
    if (!d->validate(index)) {
        return QVariant();
    }

    int r = index.row();
    int c = index.column();
    switch(role)
    {
    case Qt::DisplayRole:
        return d->display(r, c);
    case Qt::EditRole:
        return d->value(r, c);
    case Qt::ToolTipRole:
        return d->tooltip(r, c);
    default:
        break;
    }

    return QVariant();
}

void QtMessageLogModel::setRotationLimit(uint limit)
{
    Q_D(QtMessageLogModel);
    if (limit > 0 && limit > (uint)d->recordCache.size()) {
        d->maxSize = limit;
    }
}

uint QtMessageLogModel::rotationLimit() const
{
    Q_D(const QtMessageLogModel);
    return d->maxSize;
}

void QtMessageLogModel::message(int level, int code, const QString& category, const QString& message, const QDateTime& timestamp)
{
    Q_D(QtMessageLogModel);
    static const QModelIndex invalid;

    if (d->current < (int)d->recordCache.size()) {
        LogRecord& r = d->recordCache[d->current];
        r.level = level;
        r.code = code;
        r.category = category;
        r.message = message;
        r.timestamp = timestamp;
        Q_EMIT dataChanged(index(d->current, 0), index(d->current, MaxSection-1));
        ++d->current;
    }
    else {
        beginInsertRows(invalid, d->current, d->current);
        d->recordCache.emplace_back(level, code, category, message, timestamp);
        endInsertRows();
        d->current = d->recordCache.size();
    }

    if (d->current >= d->maxSize) {
        d->current = 0;
        Q_EMIT overflowed();
    }
}

void QtMessageLogModel::clear()
{
    Q_D(QtMessageLogModel);
    beginResetModel();
    d->recordCache.clear();
    d->current = 0;
    endResetModel();
}



