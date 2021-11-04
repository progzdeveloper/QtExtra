#ifndef QTMESSAGELOGMODEL_H
#define QTMESSAGELOGMODEL_H

#include <QAbstractTableModel>
#include <QDateTime>

#include <QtWidgetsExtra>

class QTWIDGETSEXTRA_EXPORT QtMessageLogModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Sections
    {
        SectionTimestamp = 0,
        SectionLevel     = 1,
        SectionCode      = 2,
        SectionCategory  = 3,
        SectionMessage   = 4,
        MaxSection = SectionMessage + 1
    };

    explicit QtMessageLogModel(QObject *parent = Q_NULLPTR);
    QtMessageLogModel(uint maxSize, QObject *parent = Q_NULLPTR);
    ~QtMessageLogModel();

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    void setRotationLimit(uint limit);
    uint rotationLimit() const;

    void message(int level, int code, const QString& category, const QString& message, const QDateTime& timestamp = QDateTime::currentDateTime());

    inline void message(int level, int code, const QString &category, const QString &message) {
        this->message(level, code, category, message, QDateTime::currentDateTime());
    }

public Q_SLOTS:
    void clear();

Q_SIGNALS:
    void overflowed();

private:
    QT_PIMPL(QtMessageLogModel)
};

#endif // QTMESSAGELOGMODEL_H
