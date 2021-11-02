#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QMetaObject>
#include <QMetaProperty>
#include <QVariant>
#include <QHash>

#include <QDebug>

#include "qtxmlmapper.h"

namespace
{

inline bool xmlWriteProperty(QXmlStreamWriter& xml, const char* propertyName, const QObject* object)
{
    const QVariant value = object->property(propertyName);

    xml.writeStartElement(propertyName);
    xml.writeAttribute("type", value.typeName());
    xml.writeAttribute("value", value.toString());
    xml.writeEndElement(); // "property"
    return true;
}

inline bool xmlReadProperty(QXmlStreamReader& xml, QObject *object)
{
    const QXmlStreamAttributes attributes = xml.attributes();
    const QStringRef propertyName = xml.name();
    const QStringRef typeName = attributes.value("type");
    const QString strVal = attributes.value("value").toString();
    QVariant value(strVal);
    value.convert(QVariant::nameToType(typeName.toLatin1()));
    return object->setProperty(propertyName.toLatin1(), value);
}

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
    for (int i = 0, n = metaObject->propertyCount(); i < n; i++) {
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



