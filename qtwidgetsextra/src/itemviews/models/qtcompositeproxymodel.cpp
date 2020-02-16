#include "qtcompositeproxymodel.h"
#include "qtitemfilter.h"
#include <QHash>
#include <QList>
#include <QLinearGradient>
#include <QBrush>
#include <QFontMetricsF>
#include <QDebug>

static inline quint64 hashCode(quint32 index, int role) {
    return (quint64(role) << 32 | quint64(index));
}

class QtCompositeProxyModelPrivate
{
public:
    typedef QList<QtItemMapper*> MapperList;
    QHash<quint64, MapperList> mappings;
    QHash<int, QtCompositeProxyModel::TraverseOptions> traversings;
    QString delimiter;
    QtCompositeProxyModel::BackgroundCombiation combinationMode;

    QtCompositeProxyModelPrivate();
    inline QVariant formatText(QtProxyModelIndex& index, const MapperList& mappers, int mode) const;
    inline QVariant formatColor(QtProxyModelIndex& index, const MapperList& mappers, int mode) const;
    inline QVariant foreground(QtProxyModelIndex& index, const MapperList& mappers, int mode) const;
    inline QVariant decoration(QtProxyModelIndex& index, const MapperList& mappers, int mode) const;
    inline QVariant textAlign(QtProxyModelIndex& index, const MapperList& mappers, int mode) const;
    inline QVariant font(QtProxyModelIndex& index, const MapperList& mappers, int mode) const;
    inline QVariant userData(QtProxyModelIndex& index, const MapperList& mappers, int mode) const;
};


QtCompositeProxyModelPrivate::QtCompositeProxyModelPrivate() :
    delimiter("\n"),
    combinationMode(QtCompositeProxyModel::Gradient)
{
}


template<class _Iterator>
static inline QString createFormattedText(QtProxyModelIndex &index, _Iterator first, _Iterator last, const QString& delimiter, bool propagate)
{
    QString result;
    QVariant value;
    for (; first != last; ++first) {
        value = (*first)->map(index);
        if (propagate) {
            index.setData(index.role(), value);
        }
        else {
            result += value.toString();
            result += delimiter;
        }
    }
    return result;
}

inline QVariant QtCompositeProxyModelPrivate::formatText(QtProxyModelIndex &index, const MapperList &mappers, int mode) const
{
    switch (mode) {
    case QtCompositeProxyModel::TopMost:
        return mappers.front()->map(index);
    case QtCompositeProxyModel::Forward:
        return createFormattedText(index, mappers.begin(), mappers.end(), delimiter, false);
    case QtCompositeProxyModel::Backward:
        return createFormattedText(index, mappers.rbegin(), mappers.rend(), delimiter, false);
    case QtCompositeProxyModel::PropagateForward:
        return createFormattedText(index, mappers.begin(), mappers.end(), delimiter, true);
    case QtCompositeProxyModel::PropagateBackward:
        return createFormattedText(index, mappers.rbegin(), mappers.rend(), delimiter, true);
    default: break;
    }
    return QVariant();
}


template<class _Iterator>
static inline QVariant createGradientBrush(QtProxyModelIndex &index, _Iterator first, _Iterator last, bool propagate)
{
    QVariant v;
    QVariant value = index.data();

    QVariantList results;
    for (; first != last; ++first)
    {
        v = (*first)->map(index);
        if (v.isValid() && v != value)
            results << v;

        if (propagate)
            index.setData(index.role(), v);
    }

    if (results.size() == 1) {
        return (results.front());
    }

    if (results.isEmpty())
        return index.data();

    QLinearGradient gradient(0, 0, 1, 0);
    gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    const qreal factor = 1.0 / (results.size() - 1);
    qreal pos = 0.0;
    for (auto it = results.begin(); it != results.end(); ++it, pos += factor) {
        gradient.setColorAt(pos, it->value<QColor>());
    }
    return QBrush(gradient);
}

inline QVariant QtCompositeProxyModelPrivate::formatColor(QtProxyModelIndex &index, const MapperList &mappers, int mode) const
{
    switch (mode) {
    case QtCompositeProxyModel::TopMost:
        for (auto it = mappers.begin(); it != mappers.end(); ++it) {
            QVariant value = (*it)->map(index);
            if (value.isValid() && value != index.data())
                return value;
        }
        return index.data();
    case QtCompositeProxyModel::Forward:
        return createGradientBrush(index, mappers.begin(), mappers.end(), false);
    case QtCompositeProxyModel::Backward:
        return createGradientBrush(index, mappers.rbegin(), mappers.rend(), false);
    case QtCompositeProxyModel::PropagateForward:
        return createGradientBrush(index, mappers.begin(), mappers.end(), true);
    case QtCompositeProxyModel::PropagateBackward:
        return createGradientBrush(index, mappers.rbegin(), mappers.rend(), true);
    default: break;
    }
    return QVariant();
}

inline QVariant QtCompositeProxyModelPrivate::foreground(QtProxyModelIndex &index, const MapperList &mappers, int mode) const
{
    Q_UNUSED(mode);
    QVariant v = index.data();
    for (auto it = mappers.begin(); it != mappers.end(); ++it) {
        QVariant value = (*it)->map(index);
        if (value.isValid() && value != v)
            return value;
    }
    return v;
}

inline QVariant QtCompositeProxyModelPrivate::decoration(QtProxyModelIndex &index, const MapperList &mappers, int mode) const
{
    Q_UNUSED(mode);
    QVariant v = index.data();
    for (auto it = mappers.begin(); it != mappers.end(); ++it) {
        QVariant value = (*it)->map(index);
        if (value.isValid() && value != v)
            return value;
    }
    return v;
}

inline QVariant QtCompositeProxyModelPrivate::textAlign(QtProxyModelIndex &index, const MapperList &mappers, int mode) const
{
    return userData(index, mappers, mode);
}

inline QVariant QtCompositeProxyModelPrivate::font(QtProxyModelIndex &index, const MapperList &mappers, int mode) const
{
    return userData(index, mappers, mode);
}

inline QVariant QtCompositeProxyModelPrivate::userData(QtProxyModelIndex &index, const MapperList &mappers, int mode) const
{
    QVariant result = index.data();
    switch (mode) {
    case QtCompositeProxyModel::TopMost:
        for (auto it = mappers.begin(); it != mappers.end(); ++it) {
            QVariant value = (*it)->map(index);
            if (value.isValid() && value != result) {
                result = value;
                break;
            }
        }
        break;
    case QtCompositeProxyModel::Forward:
        return mappers.front()->map(index);
    case QtCompositeProxyModel::Backward:
        return mappers.back()->map(index);
    case QtCompositeProxyModel::PropagateForward:
        for (auto it = mappers.begin(); it != mappers.end(); ++it) {
            result = (*it)->map(index);
            index.setData(index.role(), result);
        }
        break;
    case QtCompositeProxyModel::PropagateBackward:
        for (auto it = mappers.rbegin(); it != mappers.rend(); ++it) {
            result = (*it)->map(index);
            index.setData(index.role(), result);
        }
        break;
    default:
        break;
    }
    return result;
}






QtCompositeProxyModel::QtCompositeProxyModel(QObject *parent) :
    QIdentityProxyModel(parent), d_ptr(new QtCompositeProxyModelPrivate)
{
    setTraverseOptions(Qt::DisplayRole, TopMost);
    setTraverseOptions(Qt::BackgroundRole, Forward);
    setTraverseOptions(Qt::ForegroundRole, TopMost);
    setTraverseOptions(Qt::DecorationRole, TopMost);
    setTraverseOptions(Qt::TextAlignmentRole, TopMost);
    setTraverseOptions(Qt::FontRole, TopMost);
    setTraverseOptions(Qt::UserRole, TopMost);
}

QtCompositeProxyModel::~QtCompositeProxyModel()
{
}

void QtCompositeProxyModel::setTraverseOptions(int role, QtCompositeProxyModel::TraverseOptions opts)
{
    Q_D(QtCompositeProxyModel);
    d->traversings[role] = opts;
}

QtCompositeProxyModel::TraverseOptions QtCompositeProxyModel::traverseOptions(int role) const
{
    Q_D(const QtCompositeProxyModel);
    auto it = d->traversings.find(role);
    return (it == d->traversings.end() ? TopMost : (*it));
}

void QtCompositeProxyModel::attachMapping(int sourceColumn, int sourceRole, QtItemMapper *mapper)
{
    Q_D(QtCompositeProxyModel);
    if (!mapper)
        return;

    if (!mapper->isRoleSupported(sourceRole)) {
        qWarning() << mapper << "does not support" << sourceRole << "role";
        return;
    }

    // TODO: implement masked attaching to all columns
    /*if (sourceColumn < 0) {
        for (int i = 0; i < sourceModel()->columnCount(); i++)
            attachMapping(i, sourceRole, mapper);
        return;
    }*/

    quint64 key = hashCode(sourceColumn, sourceRole);
    auto it = d->mappings.find(key);
    if (it == d->mappings.end()) {
        d->mappings[key].push_back(mapper);

    } else {
        if (it->indexOf(mapper) == -1) {
            it->push_back(mapper);
        }
    }
}

int QtCompositeProxyModel::detachMapping(int sourceColumn, int sourceRole, QtItemMapper *mapper)
{
    Q_D(QtCompositeProxyModel);

    quint64 key = hashCode(sourceColumn, sourceRole);
    auto it = d->mappings.find(key);
    if (it == d->mappings.end())
        return 0;

    int index = it->indexOf(mapper);
    if (index < 0) {
        return 0;
    }

    it->removeAt(index);
    if (it->isEmpty()) {
        d->mappings.erase(it);
    }
    return 1;

}

int QtCompositeProxyModel::detachMappings(int sourceColumn, int sourceRole)
{
    Q_D(QtCompositeProxyModel);
    quint64 key = hashCode(sourceColumn, sourceRole);
    auto it = d->mappings.find(key);
    if (it == d->mappings.end())
        return 0;
    int n = it->size();
    it->clear();
    return n;
}

int QtCompositeProxyModel::detachMapping(QtItemMapper *mapper)
{
    Q_D(QtCompositeProxyModel);
    int count = 0;
    for (auto it = d->mappings.begin(); it != d->mappings.end(); ++it) {
        int index = it->indexOf(mapper);
        if (index >= 0) {
            it->removeAt(index);
            ++count;
        }
    }
    return count;
}

int QtCompositeProxyModel::countMappings(int sourceColumn, int sourceRole) const
{
    Q_D(const QtCompositeProxyModel);
    auto it = d->mappings.find(hashCode(sourceColumn, sourceRole));
    if (it == d->mappings.end())
        return 0;
    return it->size();
}

void QtCompositeProxyModel::clearMappings()
{
    Q_D(QtCompositeProxyModel);
    d->mappings.clear();
}

QVariant QtCompositeProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    Q_D(const QtCompositeProxyModel);

    if (!proxyIndex.isValid())
        return QIdentityProxyModel::data(proxyIndex, role);

    QtProxyModelIndex index( mapToSource(proxyIndex), role );
    int column = index.column();
    quint64 key = hashCode(column, role);
    auto it = d->mappings.find(key);
    if (it == d->mappings.end()) {
        return QIdentityProxyModel::data(proxyIndex, role);
    }

    switch (role) {
    case Qt::DisplayRole:
    case Qt::ToolTipRole:
    case Qt::StatusTipRole:
    case Qt::WhatsThisRole:
        return d->formatText(index, *it, traverseOptions(Qt::DisplayRole));
    case Qt::BackgroundRole:
        return d->formatColor(index, *it, traverseOptions(Qt::BackgroundRole));
    case Qt::ForegroundRole:
        return d->foreground(index, *it, traverseOptions(Qt::ForegroundRole));
    case Qt::DecorationRole:
        return d->decoration(index, *it, traverseOptions(Qt::DecorationRole));
    case Qt::TextAlignmentRole:
        return d->textAlign(index, *it, traverseOptions(Qt::TextAlignmentRole));
    case Qt::FontRole:
        return d->font(index, *it, traverseOptions(Qt::FontRole));
    default:
        return d->userData(index, *it, traverseOptions(Qt::UserRole));
    }
    return QVariant();
}

