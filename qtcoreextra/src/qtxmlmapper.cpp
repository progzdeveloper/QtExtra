#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QMetaObject>
#include <QMetaProperty>
#include <QVariant>
#include <QHash>

#include <QDebug>

#include "qtxmlmapper.h"


inline bool xmlWriteProperty(QXmlStreamWriter& xml, const char* propertyName, const QObject* object)
{
    QVariant value = object->property(propertyName);

    xml.writeStartElement(propertyName);
    xml.writeAttribute("type", value.typeName());
    xml.writeAttribute("value", value.toString());
    xml.writeEndElement(); // "property"
    return true;
}

inline bool xmlReadProperty(QXmlStreamReader& xml, QObject *object)
{
    QXmlStreamAttributes attributes = xml.attributes();
    QStringRef propertyName = xml.name();
    QStringRef typeName = attributes.value("type");
    QString strVal = attributes.value("value").toString();
    QVariant value(strVal);
    value.convert(QVariant::nameToType(typeName.toLatin1()));
    return object->setProperty(propertyName.toLatin1(), value);
}



QtXmlMapper::QtXmlMapper()
{
}

QtXmlMapper::~QtXmlMapper()
{
}

bool QtXmlMapper::write(QXmlStreamWriter &xml, const QObject* o) const
{
    if (!o)
        return false;

    const QMetaObject *metaObject = o->metaObject();
    xml.writeStartElement("properties");
    for (int i = 0; i < metaObject->propertyCount(); i++) {
        xmlWriteProperty(xml, metaObject->property(i).name(), o);
    }
    xml.writeEndElement(); // "properties"

    return true;
}

bool QtXmlMapper::read(QXmlStreamReader& xml, QObject* o)
{
    if (!o)
        return false;

    while(!xml.atEnd())
    {
        if (!xml.readNextStartElement())
            break;

        if (xml.name() == "properties") {
            while(xml.readNext())
            {
                if (xml.tokenType() == QXmlStreamReader::EndElement &&
                        xml.name() == "properties") {
                    break;
                }
                if (xml.tokenType() == QXmlStreamReader::StartElement) {
                    if (!xmlReadProperty(xml, o))
                        return false;
                }
            }
            break;
        }
    }
    return true;
}



