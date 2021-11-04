#include <QDialogButtonBox>
#include <QVBoxLayout>
#include "qttablemodelexporterdialog.h"
#include "qttablemodelexporter.h"


class QtTableModelExporterDialogPrivate
{
public:
    QtTableModelExporterDialog *q_ptr;
    QDialogButtonBox * buttonBox;
    QWidget * controller;
    QtTableModelExporter *exporter;
    const char* member;
    void createUi();
};

void QtTableModelExporterDialogPrivate::createUi()
{
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Yes|QDialogButtonBox::Cancel, Qt::Horizontal, q_ptr);
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, q_ptr, &QtTableModelExporterDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, q_ptr, &QtTableModelExporterDialog::reject);

    controller = exporter->createEditor(q_ptr);

    QVBoxLayout *layout = new QVBoxLayout(q_ptr);
    layout->addWidget(controller);
    layout->addWidget(buttonBox);
}


QtTableModelExporterDialog::QtTableModelExporterDialog(QtTableModelExporter *exporter,
                                                       QWidget *parent /*= Q_NULLPTR*/ )
    : QDialog(parent), d_ptr(new QtTableModelExporterDialogPrivate)
{
    d_ptr->q_ptr = this;
    d_ptr->exporter = exporter;
    d_ptr->createUi();
}

QtTableModelExporterDialog::~QtTableModelExporterDialog()
{
    delete d_ptr;
}

