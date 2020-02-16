#include "qtlineboxedit.h"
#include <QLineEdit>
#include <QToolButton>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QApplication>


QtLineBoxEdit::QtLineBoxEdit(QWidget *parent, Qt::WindowFlags flags) : QWidget(parent, flags)
{
    edit = new QLineEdit(this);
    button = new QToolButton(this);
    button->setText(tr("..."));

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(edit);
    layout->addWidget(button);
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);

    connect(edit, SIGNAL(textChanged(const QString&)), this, SIGNAL(textChanged(const QString&)));
    connect(edit, SIGNAL(editingFinished()), this, SIGNAL(editingFinished()));
    connect(button, SIGNAL(clicked()), this, SIGNAL(clicked()));

    setFocusPolicy(Qt::StrongFocus);
}

QtLineBoxEdit::~QtLineBoxEdit()
{
}

void QtLineBoxEdit::setCompleter(QCompleter *c)
{
    edit->setCompleter(c);
}

QCompleter *QtLineBoxEdit::completer() const
{
    return edit->completer();
}

void QtLineBoxEdit::setPlaceholderText(const QString& text)
{
    edit->setPlaceholderText(text);
}

QString QtLineBoxEdit::placeholderText() const
{
    return edit->placeholderText();
}

void QtLineBoxEdit::setReadOnly(bool on)
{
    edit->setReadOnly(on);
}

bool QtLineBoxEdit::isReadOnly() const
{
    return edit->isReadOnly();
}

void QtLineBoxEdit::setText(const QString& text)
{
    edit->setText(text);
}

QString QtLineBoxEdit::text() const
{
    return edit->text();
}

void QtLineBoxEdit::clear()
{
    edit->clear();
}






QtLineBoxDelegate::QtLineBoxDelegate(QObject *parent) : QItemDelegate(parent)
{
}

QWidget *QtLineBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */,
                                         const QModelIndex &/* index */) const
{
    QtLineBoxEdit *editor = new QtLineBoxEdit(parent, Qt::FramelessWindowHint);
    editor->setReadOnly(true);
    connect(editor, SIGNAL(clicked()), SLOT(path()));
    editor->setFocus();
    return editor;
}

void QtLineBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString path = index.model()->data(index, Qt::EditRole).toString();
    QtLineBoxEdit *lineBox = static_cast<QtLineBoxEdit*>(editor);
    lineBox->setText(path);
}

void QtLineBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                     const QModelIndex &index) const
{
    QtLineBoxEdit *lineBox = static_cast<QtLineBoxEdit*>(editor);
    model->setData(index, lineBox->text(), Qt::EditRole);
}

void QtLineBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, 
                                             const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}




