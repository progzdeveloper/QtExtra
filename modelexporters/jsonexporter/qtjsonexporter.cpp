#include "qtjsonexporter.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include <QTextStream>
#include <QVariant>
#include <QDateTime>
#include <QRegExp>
#include <QRegularExpression>
#include <QKeySequence>
#include <QColor>
#include <QUrl>
#include <QUuid>
#include <QCoreApplication>
#include <QDialog>

#include <QtPropertyWidget>

QT_METAINFO_TR(QtTableModelJsonExporter)
{
    QT_TR_META(QtTableModelJsonExporter, "JSON Export"),
    QT_TR_META(QtTableModelJsonExporter, "Date format"), // Формат даты
    QT_TR_META(QtTableModelJsonExporter, "Time format"), // Формат времени
    QT_TR_META(QtTableModelJsonExporter, "Boolean alpha"), // Булево значение текстом
    QT_TR_META(QtTableModelJsonExporter, "JSON format") // Формат JSON
};


class QtTableModelJsonExporterPrivate
{
public:
    QtTableModelJsonExporter* q_ptr;
    bool boolalpha;
    QString dateFormat;
    QString timeFormat;
    QJsonDocument::JsonFormat jsonFormat;
    QJsonObject json;
    QJsonArray array;
    QtTableModelJsonExporterPrivate(QtTableModelExporter* q);

    QtTableModelJsonExporterPrivate(QtTableModelJsonExporter* q);
    inline void storeTableHeader(QJsonObject &json);
    inline QJsonValue jsonValue(const QVariant& v) const;
};

QtTableModelJsonExporterPrivate::QtTableModelJsonExporterPrivate(QtTableModelJsonExporter *q) :
    q_ptr(q)
{
    boolalpha = false;
    dateFormat = "dd-MM-yyyy";
    timeFormat = "hh.mm.ss";
}

void QtTableModelJsonExporterPrivate::storeTableHeader(QJsonObject &json)
{
    QJsonArray columns;
    QAbstractTableModel *m = q_ptr->model();
    for (int i = 0, n = m->columnCount(); i < n; ++i) {
        columns.append(QJsonValue::fromVariant(m->headerData(i, Qt::Horizontal, Qt::DisplayRole)));
    }
    json["header"] = columns;
}

QJsonValue QtTableModelJsonExporterPrivate::jsonValue(const QVariant &v) const
{
    switch (v.type()) {
    case QVariant::Date:
        return v.value<QDate>().toString(dateFormat);
    case QVariant::Time:
        return v.value<QTime>().toString(timeFormat);
    case QVariant::DateTime:
        return v.value<QDateTime>().toString(dateFormat + " " + timeFormat);
    case QVariant::ByteArray:
        return QString(v.value<QByteArray>().toBase64());
    case QVariant::RegExp:
        return v.value<QRegExp>().pattern();
    case QVariant::RegularExpression:
        return v.value<QRegularExpression>().pattern();
    case QVariant::Url:
        return v.value<QUrl>().toString();
    case QVariant::KeySequence:
        return v.value<QKeySequence>().toString();
    case QVariant::Color:
        return v.value<QColor>().name();
    case QVariant::Uuid:
        return v.value<QUuid>().toString();
    default:
        return QJsonValue::fromVariant(v);
    }
    return QJsonValue::fromVariant(v);
}



QtTableModelJsonExporter::QtTableModelJsonExporter(QAbstractTableModel* model) :
    QtTableModelExporter(model),
    d_ptr(new QtTableModelJsonExporterPrivate(this))
{
    setModel(model);
}

QtTableModelJsonExporter::~QtTableModelJsonExporter()
{
    delete d_ptr;
}

void QtTableModelJsonExporter::setBoolAlpha(bool on)
{
    Q_D(QtTableModelJsonExporter);
    d->boolalpha = on;
}

bool QtTableModelJsonExporter::isBoolAlpha() const
{
    Q_D(const QtTableModelJsonExporter);
    return d->boolalpha;
}

void QtTableModelJsonExporter::setDateFormat(const QString& ch)
{
    Q_D(QtTableModelJsonExporter);
    d->dateFormat = ch;
}

QString QtTableModelJsonExporter::dateFormat() const
{
    Q_D(const QtTableModelJsonExporter);
    return d->dateFormat;
}

void QtTableModelJsonExporter::setTimeFormat(const QString& ch)
{
    Q_D(QtTableModelJsonExporter);
    d->timeFormat = ch;
}

QString QtTableModelJsonExporter::timeFormat() const
{
    Q_D(const QtTableModelJsonExporter);
    return d->timeFormat;
}

void QtTableModelJsonExporter::setJsonFormat(QtTableModelJsonExporter::Format f)
{
    Q_D(QtTableModelJsonExporter);
    d->jsonFormat = static_cast<QJsonDocument::JsonFormat>(f);
}

QtTableModelJsonExporter::Format QtTableModelJsonExporter::jsonFormat() const
{
    Q_D(const QtTableModelJsonExporter);
    return static_cast<QtTableModelJsonExporter::Format>(d->jsonFormat);
}

QStringList QtTableModelJsonExporter::fileFilter() const
{
    return QStringList() << "JSON files(*.json)";
}

bool QtTableModelJsonExporter::exportModel(QIODevice *device)
{
    Q_D(QtTableModelJsonExporter);
    if (!beginExport(device))
        return false;

    QJsonDocument document;

    d->json["title"] = tableName();
    if (isHeaderStored()) {
        d->storeTableHeader(d->json);
    }

    storeIndex(QModelIndex());

    document.setObject(d->json);

    QTextStream stream;
    stream.setDevice(device);
    stream.setCodec(textCodec());

    stream << document.toJson(d->jsonFormat);

    endExport();

    d->json = QJsonObject();
    d->array = QJsonArray();
    return (stream.status() == QTextStream::Ok);
}

void QtTableModelJsonExporter::storeIndex(const QModelIndex &index)
{
    Q_D(QtTableModelJsonExporter);

    if (aborted())
        return;

    if (index.isValid()) {
        d->array.append(d->jsonValue(index.data(itemRole())));
        return;
    }

    QJsonArray modelData;
    const QAbstractTableModel *m = model();
    const int rowCount =  m->rowCount(index);
    const int columnCount =  m->columnCount(index);
    int step = 0;
    for (int r = 0; r < rowCount; ++r) {
        d->array = QJsonArray();
        for (int c = 0; c < columnCount; ++c) {
            storeIndex(m->index(r, c, index));
            setProgress(++step);
        }
        modelData.append(d->array);
    }
    d->json["items"] = modelData;
}

QWidget *QtTableModelJsonExporter::createEditor(QDialog *parent) const
{
    QtPropertyWidget* editor = new QtPropertyWidget(parent);
    editor->setObject(const_cast<QtTableModelJsonExporter*>(this));
    editor->setClassFilter(QStringLiteral("^(?!QObject$).*"));
    QObject::connect(parent, SIGNAL(accepted()), editor, SLOT(submit()));
    QObject::connect(parent, SIGNAL(rejected()), editor, SLOT(revert()));
    return editor;
}

