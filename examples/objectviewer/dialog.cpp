#include "dialog.h"
#include <QtWidgets>
#include <QApplication>
#include <QDebug>

#include <QtPropertyWidget>

#include <QtAttributeResource>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
{
    QtGadget* gadget = new QContact;

    ListWidget* widget = new ListWidget(this);

    QtAttributeResource* resource = new QtJsonAttributeResource;
    resource->read(":/metadata.json");

    browser = new QtPropertyWidget(this);
    browser->setResource(resource);
    //browser->setGadget(gadget, gadget->metaObject());
    browser->setObject(widget);
    browser->setSubmitPolicy(QtPropertyWidget::AutoSubmit);
    browser->setViewType(QtPropertyWidget::TreeView);


    editor = new QtPropertyWidget(this);
    editor->setResource(resource);
    editor->setSubmitPolicy(QtPropertyWidget::AutoSubmit);
    editor->setViewType(QtPropertyWidget::TreeView);
    editor->setFinal(true);
    editor->setObject(browser);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Close|QDialogButtonBox::Reset, Qt::Horizontal, this);
    connect(buttonBox, SIGNAL(clicked(QAbstractButton*)),
            SLOT(buttonClicked(QAbstractButton*)));

    QWidget* browserWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(browserWidget);
    layout->addWidget(editor);
    layout->addWidget(browser, 1);

    QSplitter* splitter = new QSplitter(this);
    splitter->addWidget(widget);
    splitter->addWidget(browserWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(splitter);
    mainLayout->addWidget(buttonBox);
}

Dialog::~Dialog()
{

}

void Dialog::resizeEvent(QResizeEvent *)
{
    browser->refresh();
}

void Dialog::buttonClicked(QAbstractButton *button)
{
    if (buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole) {
        browser->revert();
    }

    if (buttonBox->buttonRole(button) == QDialogButtonBox::RejectRole) {
        close();
    }
}


ListWidget::ListWidget(QWidget *parent) :
    QListWidget(parent)
{
    new QListWidgetItem(QIcon::fromTheme("drive-harddisk"), "Seagate Barracuda (1 Tb)", this);
    new QListWidgetItem(QIcon::fromTheme("drive-harddisk"), "Hitachi KV-2918 (500 Gb)", this);
    new QListWidgetItem(QIcon::fromTheme("drive-optical"), "ASUS CD/DVD-BROM", this);
    new QListWidgetItem(QIcon::fromTheme("drive-removable-media"), "Kingstone (32 Gb)", this);
}

void ListWidget::setFilter(const QString& filter)
{
    if (m_filter == filter)
        return;

    m_filter = filter;
    emit filterChanged(m_filter);
}

QString ListWidget::filter() const
{
    return m_filter;
}


QUrl ListWidget::url() const
{
    return m_url;
}

void ListWidget::setUrl(const QUrl& url)
{
    if (m_url == url)
        return;

    m_url = url;
    emit urlChanged(m_url);
}




QtGadget::QtGadget() :
    mId(QUuid::createUuid().toString())
{
}

QtGadget::~QtGadget()
{
}

void QtGadget::setIdentifier(const QString &name)
{
    mId = name;
}

QString QtGadget::identifier() const
{
    return mId;
}







QContact::QContact() : mRating(0), mAge(0), mMarried(true)
{
}

QContact::~QContact()
{
}

void QContact::setFirstName(const QString &firstName)
{
    std::string s = R"("Geeks":1)";
    mFirstName = firstName;
    debugPrint();
}

QString QContact::firstName() const
{
    return mFirstName;
}

void QContact::setLastName(const QString &lastName)
{
    mLastName = lastName;
    debugPrint();
}

QString QContact::lastName() const
{
    return mLastName;
}

void QContact::setAge(int age)
{
    mAge = age;
    debugPrint();
}

int QContact::age() const
{
    return mAge;
}

void QContact::setBirthday(const QDateTime &dt)
{
    mBirthday = dt;
    debugPrint();
}

QDateTime QContact::birthday() const
{
    return mBirthday;
}

void QContact::setRating(qreal rating)
{
    mRating = rating;
    debugPrint();
}

qreal QContact::rating() const
{
    return mRating;
}

void QContact::setMarried(bool on)
{
    mMarried = on;
    debugPrint();
}

bool QContact::isMarried() const
{
    return mMarried;
}

void QContact::debugPrint() const
{
    const QMetaObject* metaClass = metaObject();
    for (int i = 0; i < metaClass->propertyCount(); i++) {
        QMetaProperty property = metaClass->property(i);
        qDebug() << property.name() << ":" << qPrintable(property.readOnGadget(this).toString());
    }
}
