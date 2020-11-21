#ifndef QTCOLORGRID_P_H
#define QTCOLORGRID_P_H

#include <QAbstractListModel>
#include <QStyledItemDelegate>
#include "../painting/qtcolorpalette.h"

class QtColorGridModel :
        public QAbstractListModel
{
    Q_OBJECT
public:
    explicit QtColorGridModel(QObject* parent = Q_NULLPTR);
    ~QtColorGridModel();

    void setPalette(const QtColorPalette& palette);
    const QtColorPalette& palette();

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    void clear();

private:
    QtColorPalette mPalette;
};


class QtColorGridDelegate :
        public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit QtColorGridDelegate(QObject* parent = Q_NULLPTR);
    ~QtColorGridDelegate();

    // QAbstractItemDelegate interface
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
};

#endif // QTCOLORGRID_P_H
