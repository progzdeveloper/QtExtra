#pragma once
#include <QtLineBoxEdit>

#include <QtWidgetsExtra>


class QTWIDGETSEXTRA_EXPORT QtPathLineEdit :
        public QtLineBoxEdit
{
    Q_OBJECT
    Q_DISABLE_COPY(QtPathLineEdit)
public:
    enum PathType
    {
        DirPath = 0,
        FileOpenPath = 1,
        FileSavePath = 2
    };

    explicit QtPathLineEdit(QWidget *parent = Q_NULLPTR, Qt::WindowFlags flags = 0);
    ~QtPathLineEdit(void);

    void setFilter(const QStringList& filter);
    QStringList filter() const;

    void setType(PathType type);
    PathType type() const;

private Q_SLOTS:
    void setPath();

private:
    QT_PIMPL(QtPathLineEdit)
};




class QTWIDGETSEXTRA_EXPORT QtPathLineDelegate :
        public QItemDelegate
{
    Q_OBJECT
    Q_DISABLE_COPY(QtPathLineDelegate)
public:
    explicit QtPathLineDelegate(QObject *parent = Q_NULLPTR);
    ~QtPathLineDelegate();

    void setFilter(const QStringList& filter);
    QStringList filter() const;

    void setType(QtPathLineEdit::PathType type);
    QtPathLineEdit::PathType type() const;

    void setItemRole(int role);
    int itemRole() const;

    void setTypeColumn(int column);
    int typeColumn() const;

    void setCompletionEnabled(bool on);
    bool isCompletionEnabled() const;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const Q_DECL_OVERRIDE;

    void setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;

    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const Q_DECL_OVERRIDE;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const Q_DECL_OVERRIDE;

private Q_SLOTS:
    void setPath();

private:
    QT_PIMPL(QtPathLineDelegate)
};


