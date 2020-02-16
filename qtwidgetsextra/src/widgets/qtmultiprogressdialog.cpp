#include "qtmultiprogressdialog.h"
#include "qtmessagelogwidget.h"

#include <QGuiApplication>
#include <QClipboard>

#include <QAction>
#include <QLabel>
#include <QProgressBar>
#include <QCheckBox>
#include <QPushButton>
#include <QBoxLayout>



class QtMultiProgressDialogPrivate
{
    Q_DECLARE_TR_FUNCTIONS(QtMultiProgressDialogPrivate)
public:
    QtMultiProgressDialog* q_ptr;

    QLabel* totalLabel;
    QProgressBar* totalBar;
    QLabel* partialLabel;
    QProgressBar* partialBar;
    QPushButton* detailsButton;
    QtMessageLogWidget* messageBox;
    QCheckBox* autoCloseChecker;
    QScopedPointer<QAbstractButton> cancelButton;
    bool wasCanceled;

    QtMultiProgressDialogPrivate(QtMultiProgressDialog* q);

    void initUi();

    QProgressBar* progressBar(QtMultiProgressDialog::ProgressHint hint) const;
    QLabel* label(QtMultiProgressDialog::ProgressHint hint) const;
};

QtMultiProgressDialogPrivate::QtMultiProgressDialogPrivate(QtMultiProgressDialog *q) :
    q_ptr(q), wasCanceled(false)
{
}

void QtMultiProgressDialogPrivate::initUi()
{
    totalLabel = new QLabel(q_ptr);
    totalBar = new QProgressBar(q_ptr);
    QObject::connect(totalBar, SIGNAL(valueChanged(int)), q_ptr, SLOT(progressChanged(int)));

    partialLabel = new QLabel(q_ptr);
    partialBar = new QProgressBar(q_ptr);
    QObject::connect(partialBar, SIGNAL(valueChanged(int)), q_ptr, SLOT(progressChanged(int)));

    autoCloseChecker = new QCheckBox(tr("Close dialog after operation complete"), q_ptr);
    autoCloseChecker->setChecked(false);

    cancelButton.reset(new QPushButton(tr("Cancel"), q_ptr));
    QObject::connect(cancelButton.data(), SIGNAL(clicked()), q_ptr, SLOT(cancel()));



    messageBox = new QtMessageLogWidget(q_ptr);
    messageBox->setAlternatingRowColors(false);
    messageBox->setVisible(false);

    QAction* copyAct = new QAction(QIcon::fromTheme("edit-copy"), tr("Copy text..."), messageBox);
    QObject::connect(copyAct, SIGNAL(triggered()), q_ptr, SLOT(copyText()));
    messageBox->addAction(copyAct);
    messageBox->setContextMenuPolicy(Qt::ActionsContextMenu);

    detailsButton = new QPushButton(tr("Show Details..."), q_ptr);
    detailsButton->setCheckable(true);
    detailsButton->setChecked(false);
    detailsButton->setVisible(false);
    QObject::connect(detailsButton, SIGNAL(toggled(bool)), messageBox, SLOT(setVisible(bool)));
    QObject::connect(detailsButton, &QPushButton::toggled, q_ptr, &QWidget::adjustSize);

    QHBoxLayout* detailsLayout = new QHBoxLayout;
    detailsLayout->setMargin(0);
    detailsLayout->addWidget(detailsButton);
    detailsLayout->addStretch();

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(autoCloseChecker);
    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton.data());

    QVBoxLayout* mainLayout = new QVBoxLayout(q_ptr);
    mainLayout->addWidget(totalLabel);
    mainLayout->addWidget(totalBar);
    mainLayout->addWidget(partialLabel);
    mainLayout->addWidget(partialBar);
    mainLayout->addLayout(detailsLayout);
    mainLayout->addWidget(messageBox, 1);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
}

QProgressBar *QtMultiProgressDialogPrivate::progressBar(QtMultiProgressDialog::ProgressHint hint) const
{
    switch (hint) {
    case QtMultiProgressDialog::PartialProgress:
        return partialBar;
    case QtMultiProgressDialog::TotalProgress:
        return totalBar;
    default:
        break;
    }
    return partialBar;
}

QLabel *QtMultiProgressDialogPrivate::label(QtMultiProgressDialog::ProgressHint hint) const
{
    switch (hint) {
    case QtMultiProgressDialog::PartialProgress:
        return partialLabel;
    case QtMultiProgressDialog::TotalProgress:
        return totalLabel;
    default:
        break;
    }
    return partialLabel;
}



QtMultiProgressDialog::QtMultiProgressDialog(QWidget *parent, Qt::WindowFlags flags) :
    QDialog(parent, flags), d_ptr(new QtMultiProgressDialogPrivate(this))
{
    Q_D(QtMultiProgressDialog);
    d->initUi();
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

QtMultiProgressDialog::~QtMultiProgressDialog()
{
}

void QtMultiProgressDialog::setProgress(int value, QtMultiProgressDialog::ProgressHint hint)
{
    Q_D(QtMultiProgressDialog);
    d->progressBar(hint)->setValue(value);
}

int QtMultiProgressDialog::progress(QtMultiProgressDialog::ProgressHint hint) const
{
    Q_D(const QtMultiProgressDialog);
    return d->progressBar(hint)->value();
}

void QtMultiProgressDialog::setRange(int minimum, int maximum, QtMultiProgressDialog::ProgressHint hint)
{
    Q_D(QtMultiProgressDialog);
    d->progressBar(hint)->setRange(minimum, maximum);
}

void QtMultiProgressDialog::setMinimum(int minimum, QtMultiProgressDialog::ProgressHint hint)
{
    Q_D(QtMultiProgressDialog);
    d->progressBar(hint)->setMinimum(minimum);
}

int QtMultiProgressDialog::minimum(QtMultiProgressDialog::ProgressHint hint) const
{
    Q_D(const QtMultiProgressDialog);
    return d->progressBar(hint)->minimum();
}

void QtMultiProgressDialog::setMaximum(int maximum, QtMultiProgressDialog::ProgressHint hint)
{
    Q_D(QtMultiProgressDialog);
    d->progressBar(hint)->setMaximum(maximum);
}

int QtMultiProgressDialog::maximum(QtMultiProgressDialog::ProgressHint hint) const
{
    Q_D(const QtMultiProgressDialog);
    return d->progressBar(hint)->maximum();
}

void QtMultiProgressDialog::setLabelText(const QString &text, QtMultiProgressDialog::ProgressHint hint)
{
    Q_D(QtMultiProgressDialog);
    d->label(hint)->setText(text);
}

QString QtMultiProgressDialog::labelText(QtMultiProgressDialog::ProgressHint hint) const
{
    Q_D(const QtMultiProgressDialog);
    return d->label(hint)->text();
}

void QtMultiProgressDialog::setAutoClose(bool on)
{
    Q_D(QtMultiProgressDialog);
    d->autoCloseChecker->setChecked(on);
}

bool QtMultiProgressDialog::isAutoClose() const
{
    Q_D(const QtMultiProgressDialog);
    return d->autoCloseChecker->isChecked();
}

bool QtMultiProgressDialog::wasCanceled() const
{
    Q_D(const QtMultiProgressDialog);
    return d->wasCanceled;
}

void QtMultiProgressDialog::setCancelButton(QAbstractButton *cancelButton)
{
    Q_D(QtMultiProgressDialog);
    if (cancelButton == Q_NULLPTR) {
        d->cancelButton->hide();
    } else {
        d->cancelButton.reset(cancelButton);
    }
}

QAbstractButton *QtMultiProgressDialog::cancelButton() const
{
    Q_D(const QtMultiProgressDialog);
    return d->cancelButton.data();
}

void QtMultiProgressDialog::cancel()
{
    Q_D(QtMultiProgressDialog);
    d->wasCanceled = true;
    Q_EMIT canceled();
    d->messageBox->clear();
}

void QtMultiProgressDialog::reset()
{
    Q_D(QtMultiProgressDialog);
    d->wasCanceled = false;
    d->cancelButton->setEnabled(true);
    d->autoCloseChecker->setVisible(true);
    d->messageBox->clear();
    setProgress(0, TotalProgress);
    setProgress(0, PartialProgress);
}

void QtMultiProgressDialog::message(const QString &text)
{
    Q_D(QtMultiProgressDialog);
    if (!d->detailsButton->isVisible())
        d->detailsButton->setVisible(true);
    d->messageBox->message(text);
}

void QtMultiProgressDialog::progressChanged(int value)
{
    Q_D(QtMultiProgressDialog);
    QProgressBar* progressBar = qobject_cast<QProgressBar*>(sender());
    if (progressBar == d->partialBar) {
        if (value == d->partialBar->maximum()) {
            d->totalBar->setValue(d->totalBar->value() + 1);
        }
    }

    if (progressBar == d->totalBar) {
        if (value == d->totalBar->maximum()) {
            d->cancelButton->disconnect();
            d->cancelButton->setText(tr("Close"));
            QObject::connect(d->cancelButton.data(), SIGNAL(clicked()), SLOT(close()));
            if (d->autoCloseChecker->isChecked())
                close();
            else
                d->autoCloseChecker->setVisible(false);
        }
    }
}

void QtMultiProgressDialog::copyText()
{
    Q_D(QtMultiProgressDialog);
    QGuiApplication::clipboard()->setText(d->messageBox->text());
}



