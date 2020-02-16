#include "dialog.h"

#include <QtJsonMapper>
#include <QJsonDocument>
#include <QJsonObject>

#include <QtXmlMapper>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <QTextStream>




QtContact::QtContact(QObject *parent) :
    QObject(parent), mAge(0), mMarried(false)
{
}

QtContact::~QtContact()
{
}

void QtContact::setFirstName(const QString &firstName)
{
    mFirstName = firstName;
}

QString QtContact::firstName() const
{
    return mFirstName;
}

void QtContact::setMiddleName(const QString &middleName)
{
    mMiddleName = middleName;
}

QString QtContact::middleName() const
{
    return mMiddleName;
}

void QtContact::setLastName(const QString &middleName)
{
    mLastName = middleName;
}

QString QtContact::lastName() const
{
    return mLastName;
}

void QtContact::setAge(int age)
{
    mAge = age;
}

int QtContact::age() const
{
    return mAge;
}

void QtContact::setBirthday(const QDate &birthday)
{
    mBirthday = birthday;
}

QDate QtContact::birthday() const
{
    return mBirthday;
}

void QtContact::setMarried(bool on)
{
    mMarried = on;
}

bool QtContact::isMarried() const
{
    return mMarried;
}

void QtContact::setRegisterDate(const QDateTime &dateTime)
{
    mRegDate = dateTime;
}

QDateTime QtContact::registerDate() const
{
    return mRegDate;
}



QtContactExtended::QtContactExtended(QObject *parent) :
    QtContact(parent), mRating(0)
{
}

QtContactExtended::~QtContactExtended()
{
}

void QtContactExtended::setRating(double rating)
{
    mRating = rating;
}

double QtContactExtended::rating() const
{
    return mRating;
}




inline QPushButton* createButton(const QString& text, QWidget* parent, const char* method)
{
    QPushButton* button = new QPushButton(text, parent);
    QObject::connect(button, SIGNAL(clicked()), parent, method);
    return button;
}



inline void toJson(QObject* obj, QPlainTextEdit* view)
{
    QJsonObject json;

    QtJsonMapper mapper;
    mapper.setClassFilter("Qt");
    mapper.serialize(json, obj);

    QJsonObject documentRoot;
    documentRoot["contact"] = json;
    view->setPlainText(QJsonDocument(documentRoot).toJson());
}

inline bool fromJson(QObject* obj, const QByteArray& jsonBytes, QWidget* parent = 0)
{
    QJsonParseError jsonError;
    QJsonDocument document = QJsonDocument::fromJson(jsonBytes, &jsonError);
    if (jsonError.error != QJsonParseError::NoError) {
        QMessageBox::critical(parent, QObject::tr("Format Error"),
                              QObject::tr("Failed to parse JSON: %1").arg(jsonError.errorString()));
        return false;
    }

    QJsonObject json = document.object()["contact"].toObject();
    QtJsonMapper mapper;
    mapper.setClassFilter("Qt");
    if (!mapper.validateObject<QtContact>(json)) {
        QMessageBox::critical(parent, QObject::tr("Format Error"),
                              QObject::tr("JSON shema validation failed: invalid file format"));
        return false;
    }
    return mapper.unserialize(json, obj);
}



inline void toXml(QObject* obj, QPlainTextEdit* view)
{
    QByteArray xmlBytes;
    QXmlStreamWriter xml(&xmlBytes);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeStartElement("contact");

    QtXmlMapper mapper;
    mapper.write(xml, obj);

    xml.writeEndElement(); // contact
    xml.writeEndDocument();

    view->setPlainText(xmlBytes);
}

inline bool fromXml(QObject* obj, const QByteArray& xmlBytes, QWidget* = 0)
{
    QtXmlMapper mapper;
    QXmlStreamReader xml(xmlBytes);
    while(!xml.atEnd()) {
        if (!xml.readNextStartElement())
            break;
        if (xml.name() == "contact") {
            return mapper.read(xml, obj);
        }
    }
    return false;
}


Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
{
    contact = new QtContactExtended(this);

    objectEditor = new QtPropertyWidget(this);
    objectEditor->enableNotifications(true);
    objectEditor->setObject(contact);
    connect(objectEditor, SIGNAL(propertyChanged(const QString&, const QVariant&)),
            this, SLOT(refresh(const QString&, const QVariant&)));

    textView = new QPlainTextEdit(this);
    textView->setReadOnly(true);
    textView->setFont(QFont("Courier New", 10));

    formatSelector = new QComboBox(this);
    formatSelector->addItem("JSON", (int)JsonFormat);
    formatSelector->addItem("XML", (int)XmlFormat);
    connect(formatSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(refresh()));

    openButton = createButton(tr("Open"), this, SLOT(load()));
    saveButton = createButton(tr("Save"), this, SLOT(save()));
    closeButton = createButton(tr("Close"), this, SLOT(close()));

    QHBoxLayout* mainLayout = new QHBoxLayout;
    mainLayout->addWidget(objectEditor);
    mainLayout->addWidget(textView);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(openButton);
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(formatSelector);
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(mainLayout);
    layout->addLayout(buttonLayout);

    refresh();
}

Dialog::~Dialog()
{
    delete contact;
}

void Dialog::save()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly)) {
        QMessageBox::critical(this, tr("File Error"),
                              tr("Failed to open the file '%1': %2").arg(fileName, file.errorString()));
        return;
    }

    QTextStream stream(&file);
    stream << textView->toPlainText();
}

void Dialog::load()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::critical(this, tr("File Error"),
                              tr("Failed to open the file '%1': %2").arg(fileName, file.errorString()));
        return;
    }
    QByteArray bytes = file.readAll();
    bool success = false;
    switch (formatSelector->currentData().toInt())
    {
    case JsonFormat:
        success = fromJson(contact, bytes, this);
        break;
    case XmlFormat:
        success = fromXml(contact, bytes, this);
        break;
    }

    if (success) {
        objectEditor->refresh();
        textView->setPlainText(bytes);
    } else {
        QMessageBox::critical(this, tr("Format Error"), tr("Failed to load file: invalid format"));
    }
}

void Dialog::refresh(const QString &, const QVariant &)
{
    refresh();
}

void Dialog::refresh()
{
    switch(formatSelector->currentData().toInt())
    {
    case JsonFormat:
        toJson(contact, textView);
        break;
    case XmlFormat:
        toXml(contact, textView);
        break;
    }
}




