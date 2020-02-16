#include "qtaboutdialog.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QBoxLayout>

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtGui/QScreen>
#include <QtGui/QAbstractTextDocumentLayout>

class QtAboutDialogPrivate
{
    Q_DECLARE_TR_FUNCTIONS(QtAboutDialogPrivate)
public:
    QTextBrowser *versionBrowser;
    QTextBrowser *licenseBrowser;
    QTextBrowser *aboutBrowser;
    QDialogButtonBox *buttonBox;
    QTabWidget *infoTabs;
    QLabel *logoLabel;
    QLabel *copyrightLabel;

    QtAboutDialogPrivate();
    void initUi(QWidget *parent);

    void updateSize(QWidget* parent);
};

QtAboutDialogPrivate::QtAboutDialogPrivate()
{

}

void QtAboutDialogPrivate::initUi(QWidget *parent)
{
    versionBrowser = new QTextBrowser(parent);
    versionBrowser->setWordWrapMode(QTextOption::NoWrap);
    versionBrowser->setOpenExternalLinks(true);
    versionBrowser->setOpenLinks(true);

    licenseBrowser = new QTextBrowser(parent);
    licenseBrowser->setWordWrapMode(QTextOption::NoWrap);
    licenseBrowser->setOpenExternalLinks(true);
    licenseBrowser->setOpenLinks(true);

    aboutBrowser = new QTextBrowser(parent);
    aboutBrowser->setWordWrapMode(QTextOption::NoWrap);
    aboutBrowser->setOpenExternalLinks(true);
    aboutBrowser->setOpenLinks(true);

    infoTabs = new QTabWidget(parent);
    infoTabs->setDocumentMode(true);
    infoTabs->addTab(aboutBrowser, tr("About"));
    infoTabs->addTab(versionBrowser, tr("Version"));
    infoTabs->addTab(licenseBrowser, tr("License"));

    logoLabel = new QLabel(parent);
    copyrightLabel = new QLabel(parent);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, parent);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(logoLabel);
    layout->addWidget(infoTabs);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(copyrightLabel);
    buttonLayout->addStretch();
    buttonLayout->addWidget(buttonBox);

    QVBoxLayout *mainLayout = new QVBoxLayout(parent);
    mainLayout->addLayout(layout);
    mainLayout->addLayout(buttonLayout);
    //mainLayout->setContentsMargins(2, 2, 2, 2);
}

void QtAboutDialogPrivate::updateSize(QWidget * parent)
{
    //QScreen* screen = QApplication::primaryScreen();
    QAbstractTextDocumentLayout* layout = licenseBrowser->document()->documentLayout();
    QSize documentSize = layout->documentSize().toSize();
    parent->resize(parent->height(), logoLabel->width() + documentSize.width());
}




QtAboutDialog::QtAboutDialog(QWidget *parent) :
    QDialog(parent),
    d_ptr(new QtAboutDialogPrivate)
{
    d_ptr->initUi(this);
    connect(d_ptr->buttonBox, SIGNAL(accepted()), SLOT(accept()));
    connect(d_ptr->buttonBox, SIGNAL(rejected()), SLOT(reject()));
}

QtAboutDialog::QtAboutDialog(const QJsonDocument &json, QWidget *parent) :
    QDialog(parent),
    d_ptr(new QtAboutDialogPrivate)
{
    d_ptr->initUi(this);
    connect(d_ptr->buttonBox, SIGNAL(accepted()), SLOT(accept()));
    connect(d_ptr->buttonBox, SIGNAL(rejected()), SLOT(reject()));
    setJson(json);
}

QtAboutDialog::~QtAboutDialog()
{
}

void QtAboutDialog::setJson(const QJsonDocument &document)
{
    setJson(document.object());
}

void QtAboutDialog::setJson(const QJsonObject &json)
{
    Q_D(QtAboutDialog);
    d->aboutBrowser->setText(json["description"].toString());
    d->versionBrowser->setText(json["version"].toString());
    d->copyrightLabel->setText(json["copyright"].toString());
    QString pixmapPath = json["image"].toString();
    if (!pixmapPath.isEmpty()) {
        QPixmap pixmap;
        if (pixmap.load(pixmapPath))
            d->logoLabel->setPixmap(pixmap);
    }
    QString licence = json["license"].toString();
    QFileInfo fileInfo(licence);
    if (fileInfo.isFile()) {
        setLicenseFile(licence);
    } else {
        setLicenseText(licence);
    }
}

void QtAboutDialog::setAboutText(const QString& text)
{
    Q_D(QtAboutDialog);
    d->aboutBrowser->setPlainText(text);
}

void QtAboutDialog::setVersionInfo(const QString& version)
{
    Q_D(QtAboutDialog);
    d->versionBrowser->setPlainText(version);
}

void QtAboutDialog::setCopyrightText(const QString& text)
{
    Q_D(QtAboutDialog);
    d->copyrightLabel->setText(text);
}

void QtAboutDialog::setLicenseText(const QString &text)
{
    Q_D(QtAboutDialog);
    if (Qt::mightBeRichText(text))
        d->licenseBrowser->setHtml(text);
    else
        d->licenseBrowser->setPlainText(text);
}

void QtAboutDialog::setLicenseFile(const QString& fileName)
{
    QFile file(fileName);
    file.open(QFile::ReadOnly);
    setLicenseText(file.readAll());
}

void QtAboutDialog::setImage(const QPixmap& pixmap)
{
    Q_D(QtAboutDialog);
    d->logoLabel->setPixmap(pixmap);
}

void QtAboutDialog::showEvent(QShowEvent* e)
{
    //Q_D(QAboutDialog);
    QDialog::showEvent(e);
    //d->updateSize(this);
}

