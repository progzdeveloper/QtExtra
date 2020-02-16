#ifndef QTTEXTCODECMODEL_H
#define QTTEXTCODECMODEL_H

#include <QAbstractListModel>
#include <QtWidgetsExtra>

class QtTextCodecModelPrivate;
class QTWIDGETSEXTRA_EXPORT QtTextCodecModel :
        public QAbstractListModel
{
    Q_OBJECT

public:
    explicit QtTextCodecModel(QObject *parent = Q_NULLPTR);
    ~QtTextCodecModel();

    int rowCount ( const QModelIndex & parent = QModelIndex() ) const;

    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

private:
    Q_DECLARE_PRIVATE(QtTextCodecModel)
    QtTextCodecModelPrivate *d_ptr;
};

#endif
