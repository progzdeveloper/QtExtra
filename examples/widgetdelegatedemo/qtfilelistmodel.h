#ifndef QTFILELISTMODEL_H
#define QTFILELISTMODEL_H
#include <atomic>
#include <memory>
#include <QAbstractListModel>
#include <QSize>
#include <QRunnable>

// Pixmap loading multi-threaded task class,
// compatible with QThreadPool interface.
// We use multiple inheritance here,
// since we need a QRunnable interface
// along with ability to send signals
class PixmapTask :
        public QObject,
        public QRunnable
{
    Q_OBJECT

public:
    PixmapTask(const QString& path, const QSize &maxSize);
    ~PixmapTask();

    // QRunnable interface
public:
    virtual void run();

    void cancel();

Q_SIGNALS:
    void resultReady(const QString& path, const QPixmap&);

private:
    QString mPath;
    QSize mSize;
    std::atomic<bool> discarded;
};


//
// Example implementation of files list-like model
//
//
class QtFileListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum ExtendedRoles
    {
        FileSizeRole = Qt::UserRole + 1,
        FileStatusRole,
        FileTypeRole
    };

    explicit QtFileListModel(QObject *parent = Q_NULLPTR);
    ~QtFileListModel();

    void setFiles(const QStringList& paths);
    QStringList files() const;

    void reset();

    // QAbstractListModel interface
public:
    // Header:
    QVariant headerData(int section, Qt::Orientation, int = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

private Q_SLOTS:
    void onPixmapLoaded(const QString& path, const QPixmap& pixmap);

private:
    std::unique_ptr<class QtFileListModelPrivate> d;
};

#endif // QTFILELISTMODEL_H
