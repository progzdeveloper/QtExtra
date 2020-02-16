#include "progresswidget.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QBoxLayout>
#include <QThread>
#include <QApplication>

#include <QtMultiProgressDialog>

void  simulateProcessing();

ProgressWidget::ProgressWidget(QWidget *parent)
    : QWidget(parent)
{
    execButton = new QPushButton(tr("Execute..."), this);
    connect(execButton, SIGNAL(clicked()), SLOT(execute()));

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addStretch();
    layout->addWidget(execButton);

    QFile file(":/text/items.json");
    file.open(QFile::ReadOnly);
    QJsonDocument document = QJsonDocument::fromJson(file.readAll());
    json = document.object();

}

ProgressWidget::~ProgressWidget()
{
}

void ProgressWidget::execute()
{
    QtMultiProgressDialog* dialog = new QtMultiProgressDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowModality(Qt::WindowModal);
    dialog->show();

    dialog->setRange(0, json.size(), QtMultiProgressDialog::TotalProgress);

    int i = 1;
    for (auto objIt = json.begin(); objIt != json.end(); ++objIt, ++i) {
        dialog->setLabelText(tr("<b>Processing set:</b> '%1' (%2/%3)...")
                            .arg(objIt.key()).arg(i).arg(json.size()),
                            QtMultiProgressDialog::TotalProgress);
        dialog->message(tr("Processing set: '%1' (%2/%3)...").arg(objIt.key()).arg(i).arg(json.size()));
        QJsonArray array = (*objIt).toArray();
        dialog->setRange(0, array.size());
        for (auto it = array.begin(); it != array.end(); ++it) {
            if (dialog->wasCanceled())
                break;
            dialog->setLabelText(tr("<b>Processing item:</b> '%1'...").arg((*it).toString()));
            dialog->setProgress(std::distance(array.begin(), it) + 1);
            dialog->message(tr("Processing item: '%1'...").arg((*it).toString()));

            simulateProcessing();
        }
    }
    if (dialog->wasCanceled()) {
        dialog->close();
        return;
    }
    dialog->setProgress(json.size(), QtMultiProgressDialog::TotalProgress);
}


void simulateProcessing()
{
    for (int i = 0; i < 500; i++) {
        QThread::currentThread()->usleep(100 + qrand()%500);
        qApp->processEvents();
    }
}
