#include <QDebug>
#include <QtWidgets>
#include <QWizard>
#include <QtSlideController>

#include <QtRangeNavigator>

#include "qconfigwizard.h"

//! [0] //! [1]
QWidget *createIntroPage()
{
    QWidget *page = new QWidget;
    page->setWindowTitle("Introduction");

    QLabel *label = new QLabel("This wizard will help you register your copy "
                               "of Super Product Two.");
    label->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    page->setLayout(layout);

    return page;
}
//! [0]

//! [2]
QWidget *createRegistrationPage()
//! [1] //! [3]
{
    //! [3]
    QWidget *page = new QWidget;
    page->setWindowTitle("Registration");

    QLineEdit *nameLineEdit = new QLineEdit;
    QLineEdit *emailLineEdit = new QLineEdit;

    QFormLayout *layout = new QFormLayout(page);
    layout->addRow(new QLabel("Please fill both fields."));
    layout->addRow("Name:", nameLineEdit);
    layout->addRow("Email address:", emailLineEdit);
    page->setLayout(layout);

    return page;
    //! [4]
}
//! [2] //! [4]

//! [5] //! [6]
QWidget *createConclusionPage()
//! [5] //! [7]
{
    //! [7]
    QWidget *page = new QWidget;
    page->setWindowTitle("Conclusion");

    QLabel *label = new QLabel("You are now successfully registered. Have a "
                               "nice day!");
    label->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    page->setLayout(layout);

    return page;
    //! [8]
}


void addPage(QTabWidget* tabs, QWidget* page)
{
    tabs->addTab(page, page->windowTitle());
}

QConfigWizard::QConfigWizard(QWidget* parent) : QDialog(parent)
{
    QTabWidget* tabs = new QTabWidget(this);
    tabs->setDocumentMode(true);

    addPage(tabs, createIntroPage());
    addPage(tabs, createRegistrationPage());
    addPage(tabs, createConclusionPage());

    navigator = new QtRangeNavigator(this);
    navigator->setRange(0, tabs->count());
    navigator->setText(tr("Page"));
    connect(navigator, SIGNAL(indexChanged(int)), tabs, SLOT(setCurrentIndex(int)));
    connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(changePage(int)));

    setWindowTitle("Trivial Wizard");

    QtSlideController* controller = new QtSlideController(this);
    controller->setWidget(tabs->findChild<QStackedWidget*>());
    controller->setDuration(400);
    controller->setSlideDirection(QtSlideController::SlideDown);
    controller->setEffects(QtSlideController::RollEffect);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(tabs);
    layout->addWidget(navigator);
    layout->setContentsMargins(0, 0, 0, 0);

}

void QConfigWizard::changePage(int page)
{
    QSignalBlocker blocker(navigator);
    navigator->setCurrent(page + 1);
}
