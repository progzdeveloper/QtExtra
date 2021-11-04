#ifndef QTTABLEMODELEXPORTER_H
#define QTTABLEMODELEXPORTER_H

#include <QtWidgetsExtra>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QModelIndex>

class QDialog;
class QIODevice;
class QTextCodec;
class QAbstractTableModel;


#ifdef Q_CC_GNU
#define QT_EXT_DECL_USED __attribute__((used))
#else
#define QT_EXT_DECL_USED
#endif

#ifndef QT_METAINFO_TR
#define QT_METAINFO_TR(_ClassName) static const char * _ClassName##MetaInfoTr[] QT_EXT_DECL_USED =
#endif

#ifndef QT_TR_META
#define QT_TR_META(_ClassName, _Text) QT_TRANSLATE_NOOP(#_ClassName, _Text)
#endif


class QTWIDGETSEXTRA_EXPORT QtTableModelExporter :
        public QObject
{
    Q_OBJECT
    Q_CLASSINFO("QtTableModelExporter", "Common")

    Q_PROPERTY(QString tableName READ tableName WRITE setTableName)
    Q_CLASSINFO("tableName", "Header") // Заголовок

    Q_PROPERTY(bool storeHeader READ isHeaderStored WRITE setHeaderStored)
    Q_CLASSINFO("storeHeader", "Column names") // Записать столбцы

    friend class QtTableModelExporterDialog;

public:
    explicit QtTableModelExporter(QAbstractTableModel* model = Q_NULLPTR);

    virtual ~QtTableModelExporter(void);

    virtual QDialog* createDialog(QWidget* parent = Q_NULLPTR) const;

    void setModel(QAbstractTableModel* model);
    QAbstractTableModel* model() const;

    void setTextCodec(QTextCodec* codec);
    QTextCodec *textCodec() const;

    void setItemRole(int role = Qt::DisplayRole);
    int itemRole() const;

    void setHeaderStored(bool on = true);
    bool isHeaderStored() const;

    void setTableName(const QString& name);
    QString tableName() const;

    QString errorString() const;

    virtual QStringList fileFilter() const;

    virtual bool exportModel(QIODevice *device);

    virtual QWidget *createEditor(QDialog *parent) const = 0;

protected:
    virtual void storeIndex(const QModelIndex& index = QModelIndex()) = 0;

    void setErrorString(const QString& text);
    void setProgress(int step);
    void setProgressText(const QString& text);
    bool beginExport(QIODevice *device);
    void endExport();
    bool aborted() const;

    class QtTableModelExporterPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QtTableModelExporter)
    Q_DISABLE_COPY(QtTableModelExporter)
};

#endif
