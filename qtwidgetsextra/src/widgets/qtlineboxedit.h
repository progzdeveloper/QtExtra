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
    explicit QtLineBoxEdit(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = 0);
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
    explicit QtLineBoxDelegate(QObject *parent = Q_NULLPTR);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const Q_DECL_OVERRIDE;

    void setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;

    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const Q_DECL_OVERRIDE;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const Q_DECL_OVERRIDE;

};


