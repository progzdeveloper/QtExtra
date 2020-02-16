#pragma once
#include <QWidget>
#include <QItemDelegate>

#include <QtWidgetsExtra>

class QCompleter;
class QLineEdit;
class QToolButton;

class QTWIDGETSEXTRA_EXPORT QtLineBoxEdit :
        public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_DISABLE_COPY(QtLineBoxEdit)
public:
    explicit QtLineBoxEdit(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~QtLineBoxEdit();
    QString text() const;
    QString placeholderText() const;
    bool isReadOnly() const;

    void setCompleter(QCompleter *c);
    QCompleter *completer() const;

public Q_SLOTS:
    void setReadOnly(bool on = true);
    void setPlaceholderText(const QString& text);
    void setText(const QString& text);
    void clear();

Q_SIGNALS:
    void textChanged(const QString& text);
    void clicked();
    void editingFinished();

private:
    QLineEdit *edit;
    QToolButton *button;
};




class QTWIDGETSEXTRA_EXPORT QtLineBoxDelegate :
        public QItemDelegate
{
    Q_OBJECT
    Q_DISABLE_COPY(QtLineBoxDelegate)
public:
    explicit QtLineBoxDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;

    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const;

};


