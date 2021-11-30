#include "qtfilelistmodel.h"
#include <QFileInfo>
#include <QImageReader>
#include <QApplication>
#include <QStyle>
#include <QThreadPool>
#include <QDebug>

struct RatioSuffix
{
    qint64 amount;
    char suffix;
};

static const RatioSuffix ratios[] = {
    { 1000, 'K' }, // Kilo
    { 1000000, 'M' }, // Mega
    { 1000000000, 'G' }, // Giga
    { 1000000000000LL, 'T' }, // Tera
    { 1000000000000000LL, 'P' }, // Peta
    { 1000000000000000000LL, 'E' } // Exa
};


class QtFileListModelPrivate
{
public:
    QFileInfoList list;
    QHash<QString, QPixmap> imagesCache;
    QPixmap fileImage;
    QtFileListModelPrivate() {}

    // check if file is media
    bool isMedia(const QFileInfo& fileInfo) const
    {
        const QString suffix = fileInfo.suffix();
        if (suffix.compare("GIF", Qt::CaseInsensitive) == 0)
            return true;
        return QImageReader::supportedImageFormats().contains(suffix.toUtf8());
    }

    // get a icon or an image
    QVariant fileIcon(const QFileInfo& fileInfo) const
    {
        auto it = imagesCache.constFind(fileInfo.absoluteFilePath());
        if (it != imagesCache.end())
            return *it;
        return fileImage;
    }

    // get a file name
    QVariant fileName(const QFileInfo& fileInfo) const
    {
        return fileInfo.fileName();
    }

    // get a file size
    QVariant fileSize(const QFileInfo& fileInfo) const
    {
        return formatSize(fileInfo.size()) + 'b';
    }

    // format size
    QString formatSize(quint64 size) const
    {
        int index = sizeof(ratios) / sizeof(ratios[0]) - 1;
        for (; index >= 0; --index) {
            double value = (double)size / ratios[index].amount;
            if (value >= 1.0) {
                return QString::number(value, 'f', 1) + ratios[index].suffix;
            }
        }
        return QString::number(size);
    }

    // cancel multi-threaded loading
    void cancelLoading()
    {
        QThreadPool::globalInstance()->clear(); // remove all unstarted tasks from the queue
        QThreadPool::globalInstance()->waitForDone(); // wait while other tasks will finish
    }
};


QtFileListModel::QtFileListModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(std::make_unique<QtFileListModelPrivate>())
{
    if (d->fileImage.isNull())
        d->fileImage = qApp->style()->standardPixmap(QStyle::SP_FileIcon);
}

QtFileListModel::~QtFileListModel()
{
    d->cancelLoading();
    d->list.clear();
    d->imagesCache.clear();
}

void QtFileListModel::setFiles(const QStringList &paths)
{
    beginResetModel();

    // reset everything
    d->cancelLoading();
    d->list.clear();
    d->imagesCache.clear();

    // create a QFileInfo list
    for (const auto& path : paths)
    {
        const QFileInfo fileInfo(path);
        d->list += fileInfo;
    }
    endResetModel(); // signal that main work is done

    QThreadPool::globalInstance()->setMaxThreadCount(QThread::idealThreadCount());

    // now start a heavy work of loading media files
    int row = 0;
    for (const auto& info : d->list)
    {
        if (!d->isMedia(info))
            continue; // discard any non-media files

        // create task and start loading pixmap
        PixmapTask* task = new PixmapTask(info.absoluteFilePath(), QSize(512, 512));
        task->setAutoDelete(true);
        connect(task, &PixmapTask::resultReady, this, &QtFileListModel::onPixmapLoaded, Qt::QueuedConnection);
        QThreadPool::globalInstance()->start(task);

        ++row;
    }
}

QStringList QtFileListModel::files() const
{
    // return list of file paths
    QStringList results;
    results.reserve(d->list.size());
    for (const auto& fileInfo : d->list)
        results += fileInfo.absoluteFilePath();
    return results;
}

void QtFileListModel::reset()
{
    beginResetModel();
    d->cancelLoading(); // cancel everything
    d->list.clear(); // clear list
    d->imagesCache.clear(); // clear images cache
    endResetModel();
}

QVariant QtFileListModel::headerData(int, Qt::Orientation, int) const
{
    // we don't waste our time implementing
    // this in such simple example, but in real
    // world we will prefer to implement this somehow
    return QVariant();
}

int QtFileListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    // number of rows is length of out QFileInfo list
    return d->list.size();
}

QVariant QtFileListModel::data(const QModelIndex &index, int role) const
{
    // simplest version of data() method overriding
    if (!index.isValid() || index.column() > 0)
        return QVariant();

    int row = index.row();
    if (row < 0 && row >= d->list.size())
        return QVariant();

    const QFileInfo& fileInfo = d->list[row];
    switch(role)
    {
    case Qt::DecorationRole: // decoration - get an icon
        return d->fileIcon(fileInfo);
    case Qt::DisplayRole: // display - file name
        return d->fileName(fileInfo);
    case FileSizeRole: // custom role: size of file
        return d->fileSize(fileInfo);
    case FileStatusRole: // custom role: loading status (if true - already loaded, otherwise - in progress)
        return (d->isMedia(fileInfo) ? !d->imagesCache.contains(fileInfo.absoluteFilePath()) : false);
    case FileTypeRole: // custom role: type of file (media/plain)
        return d->isMedia(fileInfo);
    default:
        break;
    }

    return QVariant();
}

bool QtFileListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    // simplest implementation of insertRows() method overriding
    beginInsertRows(parent, row, row + count - 1);

    for (int i = 0; i < count; ++i)
        d->list.insert(row, QFileInfo());

    endInsertRows();

    return true;
}

bool QtFileListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    // simplest implementation of removeRows() method overriding
    beginRemoveRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i)
    {
        const QFileInfo& fileInfo = d->list[row];
        d->imagesCache.remove(fileInfo.absoluteFilePath());
        d->list.removeAt(row);
    }
    endRemoveRows();
    return true;
}

void QtFileListModel::onPixmapLoaded(const QString &path, const QPixmap &pixmap)
{
    // pixmap loading task is finished:
    // update our images cache

    d->imagesCache[path] = pixmap;

    int row = 0;
    for (const auto info : d->list)
    {
        if (info.absoluteFilePath() == path)
        {
            QModelIndex idx = index(row);
            // notify that our internal data changed, since
            // it's important for attached proxy models
            // and views to update their internal state
            // and repaint view accordingly
            Q_EMIT dataChanged(idx, idx);
        }
        ++row;
    }
}


PixmapTask::PixmapTask(const QString &path, const QSize& maxSize)
    : mPath(path)
    , mSize(maxSize)
    , discarded(false)
{
}

PixmapTask::~PixmapTask()
{
}

void PixmapTask::run()
{
    // if images are small the loading process will be too fast
    // so we add fake delay - that allows us to see that loading
    // placeholder animation is work as expected. If you don't
    // intersting in this behavour - comment the next line out
    QThread::currentThread()->msleep(3000);

    // create the QImageLoader - and read image from disk
    QImageReader imageReader(mPath);
    QPixmap pixmap = QPixmap::fromImageReader(&imageReader);
    if (!discarded) // emit resulting pixmap
        Q_EMIT resultReady(mPath, pixmap.scaled(mSize, Qt::KeepAspectRatio));
}

void PixmapTask::cancel()
{
    discarded = true;
}
