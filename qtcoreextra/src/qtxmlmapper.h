#pragma once
#include <QtCoreExtra>

class QObject;
class QXmlStreamWriter;
class QXmlStreamReader;

class QTCOREEXTRA_EXPORT QtXmlMapper
{
    Q_DISABLE_COPY(QtXmlMapper)
public:
    QtXmlMapper();
    virtual ~QtXmlMapper();

    virtual bool write(QXmlStreamWriter& xml, const QObject* o) const;
    virtual bool read(QXmlStreamReader &xml, QObject* o);
};
