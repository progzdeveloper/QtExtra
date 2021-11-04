#ifndef QTTABLEMODELEXPORTERFACTORY_H
#define QTTABLEMODELEXPORTERFACTORY_H

#include <QtWidgetsExtra>
#include <QFactoryInterface>
#include <QString>
#include <QHash>
#include <QObject>
#include <QIcon>

class QAbstractTableModel;
class QtTableModelExporter;
class QtTableModelExporterPlugin;

class QTWIDGETSEXTRA_EXPORT QtTableModelExporterFactory :
        public QFactoryInterface
{
    Q_DISABLE_COPY(QtTableModelExporterFactory)
    QtTableModelExporterFactory();
public:

    ~QtTableModelExporterFactory();

    void registerExporter(QtTableModelExporterPlugin* creator);
    QtTableModelExporter* createExporter(const QString& exporter,
                                         QAbstractTableModel* model) const;

    QIcon exporterIcon(const QString& exporter) const;

    QStringList keys() const;

    static QtTableModelExporterFactory* instance();

private:
    typedef QHash<QString, QtTableModelExporterPlugin*> CreatorHash;
    CreatorHash m_creatorHash;
};

#endif
