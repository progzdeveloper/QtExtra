#ifndef QTEDITDIALOG_H
#define QTEDITDIALOG_H

#include <QDialog>
#include <QItemEditorCreator>

#include <QtWidgetsExtra>

class QItemEditorFactory;

class QTWIDGETSEXTRA_EXPORT QtEditDialog :
        public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(QtEditDialog)
public:
    explicit QtEditDialog(QWidget *parent = Q_NULLPTR);
    explicit QtEditDialog(QItemEditorFactory* factory, QWidget* parent = Q_NULLPTR);
    ~QtEditDialog();

    template<int _UserType, class _Editor>
    Q_INLINE_TEMPLATE void registerEditor(const QByteArray& propName) {
        this->registerEditor(_UserType, new QItemEditorCreator<_Editor>(propName));
    }

    void registerEditor(int userType, QItemEditorCreatorBase* creator);

    bool isChecked(const QString& objectName) const;

    void insert(const QString& title, const QString& objectName, const QVariant& value, bool checkable = false);

    QVariantHash values() const;
    QVariant value(const QString &id) const;

Q_SIGNALS:
    void valueChanged(const QString& id, const QVariant& value);

private Q_SLOTS:
    void editorValueChanged();

private:
    QT_PIMPL(QtEditDialog)
};


#endif // QTEDITDIALOG_H
