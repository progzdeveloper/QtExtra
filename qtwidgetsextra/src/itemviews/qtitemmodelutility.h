#ifndef QTITEMVIEWUTILITY_H
#define QTITEMVIEWUTILITY_H

#include <QStack>
#include <QModelIndex>
#include <QAbstractProxyModel>

static inline QModelIndex sourceIndex(const QModelIndex& index) {
    const QAbstractProxyModel* proxy = qobject_cast<const QAbstractProxyModel*>(index.model());
    return  (proxy != Q_NULLPTR ? proxy->mapToSource(index) : index);
}

static inline QModelIndex sourceIndex(const QAbstractItemModel* model, int row, int column) {
    return sourceIndex(model->index(row, column));
}

static inline const QAbstractItemModel* sourceModel(const QAbstractItemModel* model)
{
    const QAbstractProxyModel* proxy = qobject_cast<const QAbstractProxyModel*>(model);
    while(proxy != Q_NULLPTR) {
        model = proxy->sourceModel();
        proxy = qobject_cast<const QAbstractProxyModel*>(model);
    }
    return model;
}

static inline QString formatPath(const QModelIndex& current, const QString& separator = "/", int role = Qt::DisplayRole)
{
    QModelIndex index = current;
    QString result;
    QStack<QString> stack;
    while(index.isValid()) {
        stack.push(index.data(role).toString());
        index = index.parent();
    }

    while(!stack.isEmpty()) {
        result += separator;
        result += stack.pop();
    }
    return result;
}

#endif // QTITEMVIEWUTILITY_H
