#pragma once
#include <QtTableModelExporter>
#include <QFont>
#include <QColor>

class QtTableModelHtmlExporter :
        public QtTableModelExporter
{
    Q_OBJECT
    Q_CLASSINFO("QtTableModelHtmlExporter", "HTML Export")

    Q_PROPERTY(QFont titleFont READ titleFont WRITE setTitleFont)
    Q_CLASSINFO("titleFont", "Title font")

    Q_PROPERTY(int margin READ margin WRITE setMargin)
    Q_CLASSINFO("margin", "Margin")

    Q_PROPERTY(int padding READ padding WRITE setPadding)
    Q_CLASSINFO("padding", "Padding")

    Q_PROPERTY(qreal border READ border WRITE setBorder)
    Q_CLASSINFO("border", "Border")

    Q_PROPERTY(QColor background READ background WRITE setBackground)
    Q_CLASSINFO("background", "Background")

    Q_PROPERTY(QColor headerBackground READ headerBackground WRITE setHeaderBackground)
    Q_CLASSINFO("headerBackground", "Header background")


public:
    explicit QtTableModelHtmlExporter(QAbstractTableModel* model = Q_NULLPTR);
    ~QtTableModelHtmlExporter();

    void setTitleFont(const QFont& font);
    QFont titleFont() const;

    void setBorder(qreal value);
    qreal border() const;

    void setMargin(int value);
    int margin() const;

    void setPadding(int value);
    int padding() const;

    void setBackground(QColor c);
    QColor background() const;

    void setHeaderBackground(QColor c);
    QColor headerBackground() const;

    // QtTableModelExporter interface
    QWidget *createEditor(QDialog *parent) const override;
    QStringList fileFilter() const override;
    bool exportModel(QIODevice *device) override;
    void storeIndex(const QModelIndex &index = QModelIndex()) override;

private:
    class QtTableModelHtmlExporterPrivate* d_ptr;
    Q_DECLARE_PRIVATE(QtTableModelHtmlExporter)
};

