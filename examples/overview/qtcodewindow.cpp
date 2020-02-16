#include "qtcodewindow.h"
#include <QPlainTextEdit>

#include <QBoxLayout>
#include <QPushButton>

#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>

#include <QtOverviewWidget>

static inline void createCornerButton(QAbstractScrollArea* area, QtOverviewWidget* overview)
{
    QPushButton* cornerButton = new QPushButton(area);
    cornerButton->setCheckable(true);
    cornerButton->setChecked(overview->isVisible());
    QObject::connect(cornerButton, SIGNAL(toggled(bool)), overview, SLOT(setVisible(bool)));
    area->setCornerWidget(cornerButton);
}

QtCodeWindow::QtCodeWindow(QWidget *parent) : QWidget(parent)
{
    edit = new QPlainTextEdit(this);
    edit->setWordWrapMode(QTextOption::NoWrap);
    overview = new QtOverviewWidget(Q_NULLPTR, Qt::WindowStaysOnTopHint);
    overview->setArea(edit);
    overview->hide();

    createCornerButton(edit, overview);

    QHBoxLayout* layout = new QHBoxLayout(this);
    //layout->addWidget(overview);
    layout->addWidget(edit);
}

QtCodeWindow::~QtCodeWindow()
{
}

bool QtCodeWindow::load()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open text file"), QString(), "*.txt;;*.cpp;;*.hpp;;*.c;;*.h");
    if (fileName.isEmpty())
        return false;

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::critical(this, tr("File error"), tr("Failed to open the file '%1': %2").arg(fileName).arg(file.errorString()));
        return false;
    }

    edit->setPlainText(file.readAll());
    QFileInfo fileInfo(fileName);
    setWindowTitle(fileInfo.fileName());
    return true;
}
