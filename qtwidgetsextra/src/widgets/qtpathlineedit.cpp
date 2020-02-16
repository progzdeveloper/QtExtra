#include "qtpathlineedit.h"
#include <QApplication>
#include <QFileDialog>
#include <QDir>
#include <QDirModel>
#include <QCompleter>
#include <QMutex>
#include <QMutexLocker>

class QtPathLineEditPrivate
{
public:
    QStringList filter;
    int type;
};


QtPathLineEdit::QtPathLineEdit(QWidget *parent, Qt::WindowFlags flags) 
    : QtLineBoxEdit(parent, flags), d_ptr(new QtPathLineEditPrivate)
{
    d_ptr->type = static_cast<int>(FileOpenPath);
    setReadOnly(false);
    connect(this, SIGNAL(clicked()), SLOT(setPath()));
    setFocusPolicy(Qt::StrongFocus);
}

QtPathLineEdit::~QtPathLineEdit(void)
{
}

void QtPathLineEdit::setFilter(const QStringList& filter)
{
    Q_D(QtPathLineEdit);
    d->filter = filter;
}

QStringList QtPathLineEdit::filter() const
{
    Q_D(const QtPathLineEdit);
    return d->filter;
}

void QtPathLineEdit::setType(PathType type)
{
    Q_D(QtPathLineEdit);
    d->type = type;
}

QtPathLineEdit::PathType QtPathLineEdit::type() const
{
    Q_D(const QtPathLineEdit);
    return static_cast<PathType>(d->type);
}


void QtPathLineEdit::setPath()
{
    Q_D(QtPathLineEdit);
    setFocus();
    QString path = text();
    if (path.isEmpty())
        path = qApp->property("lastDir").toString();

    switch(static_cast<PathType>(d->type))
    {
    case DirPath:
        path = QFileDialog::getExistingDirectory(this, tr("Select Directory"), path);
        break;
    case FileOpenPath:
        path = QFileDialog::getOpenFileName(this, tr("Select File"), path, d->filter.join(";;"));
        break;
    case FileSavePath:
        path = QFileDialog::getSaveFileName(this, tr("Select File"), path, d->filter.join(";;"));
        break;
    }
    if (!path.isEmpty()) {
        setText(path);
        qApp->setProperty("lastDir", path);
    }
}






class QtPathLineDelegatePrivate
{
public:
    QStringList pathFilter;
    QtPathLineEdit::PathType pathType;
    bool completionEnabled;
    int column, role;
    QModelIndex curIdx;
};



QtPathLineDelegate::QtPathLineDelegate(QObject *parent) :
    QItemDelegate(parent),
    d_ptr(new QtPathLineDelegatePrivate)
{
    Q_D(QtPathLineDelegate);
    d->pathFilter << tr("All files (*.*)");
    d->pathType = QtPathLineEdit::FileOpenPath;
    d->completionEnabled = true;
    d->column = 0;
    d->role = Qt::EditRole;
}

QtPathLineDelegate::~QtPathLineDelegate()
{
}

void QtPathLineDelegate::setItemRole(int role)
{
    Q_D(QtPathLineDelegate);
    d->role = role;
}

int QtPathLineDelegate::itemRole() const
{
    Q_D(const QtPathLineDelegate);
    return d->role;
}

void QtPathLineDelegate::setTypeColumn(int c)
{
    Q_D(QtPathLineDelegate);
    d->column = c;
}

int QtPathLineDelegate::typeColumn() const
{
    Q_D(const QtPathLineDelegate);
    return d->column;
}

void QtPathLineDelegate::setFilter(const QStringList& filter)
{
    Q_D(QtPathLineDelegate);
    d->pathFilter = filter;
}

QStringList QtPathLineDelegate::filter() const
{
    Q_D(const QtPathLineDelegate);
    return d->pathFilter;
}

void QtPathLineDelegate::setType(QtPathLineEdit::PathType type)
{
    Q_D(QtPathLineDelegate);
    d->pathType = type;
}

QtPathLineEdit::PathType QtPathLineDelegate::type() const
{
    Q_D(const QtPathLineDelegate);
    return d->pathType;
}

void QtPathLineDelegate::setCompletionEnabled(bool on)
{
    Q_D(QtPathLineDelegate);
    d->completionEnabled = on;
}

bool QtPathLineDelegate::isCompletionEnabled() const
{
    Q_D(const QtPathLineDelegate);
    return d->completionEnabled;
}

QWidget *QtPathLineDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */,
                                          const QModelIndex & index) const
{
    Q_D(const QtPathLineDelegate);
    const QAbstractItemModel * model = index.model();
    QModelIndex idx = model->index(index.row(), d->column);

    QtLineBoxEdit* pathEdit = new QtLineBoxEdit(parent, Qt::FramelessWindowHint);
    pathEdit->setProperty( "pathType", (d->column == -1 ? d->pathType : idx.data(d->role).toInt()) );

    if (d->completionEnabled)
    {
        QDir::Filters dirFilters =
                (d->pathType == QtPathLineEdit::DirPath ?
                     QDir::Dirs|QDir::NoDotAndDotDot        :
                     QDir::AllEntries|QDir::NoDotAndDotDot);

        QCompleter *completer = new QCompleter(pathEdit);
        completer->setModel(new QDirModel(QStringList(), dirFilters, QDir::NoSort, completer));
        pathEdit->setCompleter(completer);
    }
    connect(pathEdit, SIGNAL(clicked()), SLOT(setPath()));
    return pathEdit;
}

void QtPathLineDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    Q_D(const QtPathLineDelegate);
    QString path = index.model()->data(index, d->role).toString();
    QtPathLineEdit *pathEdit = static_cast<QtPathLineEdit*>(editor);
    pathEdit->setText(path);
}

void QtPathLineDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                      const QModelIndex &index) const
{
    QtLineBoxEdit *pathEdit = static_cast<QtLineBoxEdit*>(editor);
    model->setData(index, pathEdit->text(), Qt::EditRole);
}

void QtPathLineDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                              const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}


void QtPathLineDelegate::setPath()
{
    Q_D(QtPathLineDelegate);

    blockSignals(true);

    QtLineBoxEdit *edit = qobject_cast<QtLineBoxEdit*>(sender());
    QString path = edit->text();
    int type = (d->column == -1 ? d->pathType : edit->property("pathType").toInt());
    switch(static_cast<QtPathLineEdit::PathType>(type))
    {
    case QtPathLineEdit::DirPath:
        path = QFileDialog::getExistingDirectory(edit,
                                                 tr("Select Directory"),
                                                 path);
        break;
    case QtPathLineEdit::FileOpenPath:
        path = QFileDialog::getOpenFileName(edit,
                                            tr("Select File"),
                                            path,
                                            d->pathFilter.join(";;"));
        break;
    case QtPathLineEdit::FileSavePath:
        path = QFileDialog::getSaveFileName(edit,
                                            tr("Select File"),
                                            path,
                                            d->pathFilter.join(";;"));
        break;
    }

    if (path.isEmpty())
        return;


    if (edit)
        edit->setText(path);

    blockSignals(false);
}
