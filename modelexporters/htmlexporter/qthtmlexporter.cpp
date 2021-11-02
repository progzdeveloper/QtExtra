#include <QCoreApplication>
#include <QtPropertyWidget>
#include <QDialog>
#include <QFont>
#include <QTextStream>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextTable>
#include <QTextTableFormat>

#include "qthtmlexporter.h"

QT_METAINFO_TR(QtTableModelHtmlExporter) {
    QT_TR_META(QtTableModelHtmlExporter, "HTML Export"),
    QT_TR_META(QtTableModelHtmlExporter, "Title font"),
    QT_TR_META(QtTableModelHtmlExporter, "Border"),
    QT_TR_META(QtTableModelHtmlExporter, "Margin"),
    QT_TR_META(QtTableModelHtmlExporter, "Padding"),
    QT_TR_META(QtTableModelHtmlExporter, "Background")
};


template<Qt::ItemDataRole _Role, class _Type>
static inline _Type indexData(const QModelIndex& index) {
    return index.data(_Role).value<_Type>();
}

template<Qt::ItemDataRole _Role, class _Type>
static inline _Type headerData(QAbstractItemModel* model, int section) {
    return model->headerData(section, Qt::Horizontal, _Role).value<_Type>();
}



class QtTableModelHtmlExporterPrivate
{
public:
    QtTableModelHtmlExporter *q_ptr;
    QTextStream stream;
    QTextDocument document;
    QTextCursor cursor;
    QTextTable* textTable;
    QTextTableFormat frameFormat;
    QFont titleFont;
    QColor backgroundColor;
    QColor headerBackground;

    QtTableModelHtmlExporterPrivate(QtTableModelHtmlExporter* q);
    inline void storeHeader();
    inline void storeItem(QTextTable* table, const QModelIndex &index) const;
    inline QTextCharFormat indexFormat(const QModelIndex &index) const;
};


QtTableModelHtmlExporterPrivate::QtTableModelHtmlExporterPrivate(QtTableModelHtmlExporter *q) :
    q_ptr(q), backgroundColor(Qt::white)
{
}

void QtTableModelHtmlExporterPrivate::storeHeader()
{
    QColor c;
    QTextCharFormat format;

    QAbstractTableModel *m = q_ptr->model();
    int nrows =  m->rowCount() + 1;
    int ncols =  m->columnCount();
    textTable = cursor.insertTable(nrows, ncols, frameFormat);

    for (int i = 0; i < ncols; ++i)
    {
        c = headerData<Qt::BackgroundRole, QColor>(m, i);
        format.setBackground(c.isValid() ? c : headerBackground);
        c = headerData<Qt::ForegroundRole, QColor>(m, i);
        format.setForeground(c.isValid() ? c : Qt::black);
        format.setFont(headerData<Qt::FontRole, QFont>(m, i));
        format.setToolTip(headerData<Qt::ToolTipRole, QString>(m, i));

        QTextTableCell cell = textTable->cellAt(0, i);
        cell.setFormat(format);
        QTextCursor cellCursor = cell.firstCursorPosition();
        cellCursor.insertText(headerData<Qt::DisplayRole, QString>(m, i));
        qApp->processEvents();
    }
}

void QtTableModelHtmlExporterPrivate::storeItem(QTextTable* table, const QModelIndex &index) const
{
    int offset = q_ptr->isHeaderStored();
    QTextTableCell cell = table->cellAt(index.row() + offset, index.column());
    cell.setFormat(indexFormat(index));
    QTextCursor cellCursor = cell.firstCursorPosition();
    cellCursor.insertText(index.data(q_ptr->itemRole()).toString());
    qApp->processEvents();
}

inline QTextCharFormat QtTableModelHtmlExporterPrivate::indexFormat(const QModelIndex &index) const
{
    QColor c;
    QTextCharFormat format;

    c = indexData<Qt::BackgroundRole, QColor>(index);
    format.setBackground(c.isValid() ? c : backgroundColor);

    c = indexData<Qt::ForegroundRole, QColor>(index);
    format.setForeground(c.isValid() ? c : Qt::black);

    format.setFont(indexData<Qt::FontRole, QFont>(index));
    format.setToolTip(indexData<Qt::ToolTipRole, QString>(index));

    return format;
}


QtTableModelHtmlExporter::QtTableModelHtmlExporter(QAbstractTableModel *model) :
    QtTableModelExporter(model),
    d_ptr(new QtTableModelHtmlExporterPrivate(this))
{
    setModel(model);
    connect(this, SIGNAL(headerStoredChanged(bool)), SLOT(onHeaderStored(bool)));
}

QtTableModelHtmlExporter::~QtTableModelHtmlExporter()
{
    delete d_ptr;
}

void QtTableModelHtmlExporter::setTitleFont(const QFont &font)
{
    Q_D(QtTableModelHtmlExporter);
    d->titleFont = font;
}

QFont QtTableModelHtmlExporter::titleFont() const
{
    Q_D(const QtTableModelHtmlExporter);
    return d->titleFont;
}

void QtTableModelHtmlExporter::setBorder(qreal value)
{
    Q_D(QtTableModelHtmlExporter);
    d->frameFormat.setBorder(value);
}

qreal QtTableModelHtmlExporter::border() const
{
    Q_D(const QtTableModelHtmlExporter);
    return d->frameFormat.border();
}

void QtTableModelHtmlExporter::setMargin(int value)
{
    Q_D(QtTableModelHtmlExporter);
    d->frameFormat.setMargin(value);
}

int QtTableModelHtmlExporter::margin() const
{
    Q_D(const QtTableModelHtmlExporter);
    return d->frameFormat.margin();
}

void QtTableModelHtmlExporter::setPadding(int value)
{
    Q_D(QtTableModelHtmlExporter);
    d->frameFormat.setPadding(value);
}

int QtTableModelHtmlExporter::padding() const
{
    Q_D(const QtTableModelHtmlExporter);
    return d->frameFormat.padding();
}

void QtTableModelHtmlExporter::setBackground(QColor c)
{
    Q_D(QtTableModelHtmlExporter);
    d->backgroundColor = c;
}

QColor QtTableModelHtmlExporter::background() const
{
    Q_D(const QtTableModelHtmlExporter);
    return d->backgroundColor;
}

void QtTableModelHtmlExporter::setHeaderBackground(QColor c)
{
    Q_D(QtTableModelHtmlExporter);
    d->headerBackground = c;
}

QColor QtTableModelHtmlExporter::headerBackground() const
{
    Q_D(const QtTableModelHtmlExporter);
    return d->headerBackground;
}

QStringList QtTableModelHtmlExporter::fileFilter() const
{
    return (QStringList() << "*.html" << "*.htm");
}

bool QtTableModelHtmlExporter::exportModel(QIODevice *device)
{
    Q_D(QtTableModelHtmlExporter);

    if (!beginExport(device))
        return false;

    d->textTable = Q_NULLPTR;
    d->document.clear();
    d->stream.setDevice(device);
    d->stream.setCodec(textCodec());

    d->cursor = QTextCursor(&d->document);

    if (!tableName().isEmpty()) {
        QTextCharFormat format;
        format.setFont(d->titleFont);
        d->cursor.insertText(tableName(), format);
    }

    if (isHeaderStored()) {
        d->storeHeader();
    }
    storeIndex();

    d->stream << d->document.toHtml();
    endExport();

    d->document.clear();
    return true;
}

void QtTableModelHtmlExporter::storeIndex(const QModelIndex &index)
{
    Q_D(QtTableModelHtmlExporter);

    if (aborted())
        return;

    if (index.isValid()) {
        d->storeItem(d->textTable, index);
        return;
    }

    if (d->stream.status() != QTextStream::Ok) {
        setErrorString(d->stream.device()->errorString());
        return;
    }

    const QAbstractTableModel *m = model();
    const int rowCount =  m->rowCount(index);
    const int columnCount =  m->columnCount(index);
    int step = 0;

    d->cursor.beginEditBlock();
    if (!d->textTable)
        d->textTable = d->cursor.insertTable(rowCount, columnCount, d->frameFormat);

    for (int r = 0; r < rowCount; ++r) {
        for (int c = 0; c < columnCount; ++c) {
            storeIndex(m->index(r, c, index));
            setProgress(++step);
        }
    }
    d->textTable = Q_NULLPTR;
    d->cursor.endEditBlock();
}

QWidget *QtTableModelHtmlExporter::createEditor(QDialog *parent) const
{
    QtPropertyWidget* editor = new QtPropertyWidget(parent);
    editor->setObject(const_cast<QtTableModelHtmlExporter*>(this));
    editor->setClassFilter("^(?!QObject$).*");
    QObject::connect(this, SIGNAL(headerStoredChanged(bool)), editor, SLOT(refresh()));
    QObject::connect(parent, SIGNAL(accepted()), editor, SLOT(submit()));
    QObject::connect(parent, SIGNAL(rejected()), editor, SLOT(revert()));
    return editor;
}


