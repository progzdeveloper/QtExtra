#ifndef QTMULTIPROGRESSDIALOG_H
#define QTMULTIPROGRESSDIALOG_H

#include <QDialog>
#include <QtWidgetsExtra>

class QAbstractButton;

class QtMultiProgressDialog :
        public QDialog
{
    Q_OBJECT
    Q_PROPERTY(bool wasCanceled READ wasCanceled NOTIFY canceled)
    Q_PROPERTY(bool autoClose READ isAutoClose WRITE setAutoClose)

public:
    enum ProgressHint
    {
        PartialProgress = 0,
        TotalProgress = 1
    };

    explicit QtMultiProgressDialog(QWidget *parent = Q_NULLPTR, Qt::WindowFlags flags = 0);
    ~QtMultiProgressDialog();

    int progress(ProgressHint hint = PartialProgress) const;
    int minimum(ProgressHint hint = PartialProgress) const;
    int maximum(ProgressHint hint = PartialProgress) const;

    QString labelText(ProgressHint hint = PartialProgress) const;

    bool isAutoClose() const;
    bool wasCanceled() const;

    void setCancelButton(QAbstractButton* cancelButton);
    QAbstractButton* cancelButton() const;

public Q_SLOTS:
    void setProgress(int value, ProgressHint hint = PartialProgress);
    void setRange(int minimum, int maximum, ProgressHint hint = PartialProgress);
    void setMinimum(int minimum, ProgressHint hint = PartialProgress);
    void setMaximum(int maximum, ProgressHint hint = PartialProgress);

    void setLabelText(const QString& text, ProgressHint hint = PartialProgress);
    void setAutoClose(bool on = true);

    void cancel();
    void reset();

    void message(const QString& text);

private Q_SLOTS:
    void progressChanged(int value);
    void copyText();

Q_SIGNALS:
    void canceled();

private:
    QT_PIMPL(QtMultiProgressDialog)
};

#endif // QTMULTIPROGRESSDIALOG_H
