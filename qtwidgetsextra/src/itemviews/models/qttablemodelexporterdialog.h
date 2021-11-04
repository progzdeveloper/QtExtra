#ifndef QTTABLEMODELEXPORTERDIALOG_H
#define QTTABLEMODELEXPORTERDIALOG_H

#include <QtWidgetsExtra>
#include <QDialog>

class QtTableModelExporter;

class QtTableModelExporterDialogPrivate;

class QTWIDGETSEXTRA_EXPORT QtTableModelExporterDialog :
        public QDialog
{
    Q_OBJECT
public:
    explicit QtTableModelExporterDialog(QtTableModelExporter *exporter,
                                        QWidget *parent = Q_NULLPTR);
    ~QtTableModelExporterDialog();

private:
    class QtTableModelExporterDialogPrivate *d_ptr;
    Q_DISABLE_COPY(QtTableModelExporterDialog)
    Q_DECLARE_PRIVATE(QtTableModelExporterDialog)
};

#endif
