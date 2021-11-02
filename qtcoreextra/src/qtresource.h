#ifndef QTRESOURCE_H
#define QTRESOURCE_H
#include <QtCore/QResource>
#include <QtCore/QString>
#include <QtCore/QStringList>

#include <QtCoreExtra>


class QBuffer;

class QTCOREEXTRA_EXPORT QtResource :
        public QResource
{
public:
    QtResource(const QString &file = QString(), const QLocale &locale = QLocale());

    QString toString() const;
    QStringList toStringList() const;

    void buffer(QBuffer& buffer) const;
};

#endif // QTRESOURCE_H
