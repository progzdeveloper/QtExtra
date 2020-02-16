#include "qtresource.h"
#include <QtCore/QBuffer>
#include <QtCore/QTextStream>

QtResource::QtResource(const QString &file, const QLocale &locale) :
    QResource(file, locale)
{
}

QString QtResource::toString() const
{
    QBuffer buf;
    buffer(buf);
    buf.open(QBuffer::ReadOnly);
    return buf.readAll();
}

QStringList QtResource::toStringList() const
{
    QStringList strings;
    QBuffer buf;
    buffer(buf);
    buf.open(QBuffer::ReadOnly);
    QTextStream stream(&buf);
    QString line;
    while(!stream.atEnd()) {
        stream.readLineInto(&line);
        strings << line;
    }
    return strings;
}

void QtResource::buffer(QBuffer& buffer) const
{
    buffer.setData(reinterpret_cast<const char*>(this->data()), this->size());
}



