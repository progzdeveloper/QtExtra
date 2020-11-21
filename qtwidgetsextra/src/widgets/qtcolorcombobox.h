#ifndef QTCOLORCOMBOBOX_H
#define QTCOLORCOMBOBOX_H

#include <QComboBox>

#include <QtWidgetsExtra>

class QPushButton;

class QTWIDGETSEXTRA_EXPORT QtColorComboBox :
        public QComboBox
{
    Q_OBJECT
    Q_PROPERTY(bool colorDialogEnabled READ isColorDialogEnabled WRITE setColorDialogEnabled)
    Q_PROPERTY(int colorCount READ colorCount)
    Q_PROPERTY(QColor currentColor READ currentColor)
    Q_PROPERTY(QStringList colors READ colors WRITE setColors)
    Q_DISABLE_COPY(QtColorComboBox)

public:
    explicit QtColorComboBox(QWidget *parent = Q_NULLPTR);
    ~QtColorComboBox();

    void setStandardColors();

    void addColor(const QColor & color);
    void addColor(const QColor & color, const QString & name);
    void insertColor(int index, const QColor & color, const QString & name);

    QColor color(int index) const;

    void setColors(const QStringList &map);
    QStringList colors() const;

    int colorCount() const;
    QColor currentColor() const;

    bool isColorDialogEnabled() const;
    void setColorDialogEnabled(bool enabled = true);

public Q_SLOTS:
    void setCurrentColor(const QColor& color);

Q_SIGNALS:
    void activated(const QColor &);

protected:
    bool eventFilter(QObject *watched, QEvent *e) Q_DECL_OVERRIDE;
    void dragEnterEvent(QDragEnterEvent *) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent *) Q_DECL_OVERRIDE;

private slots:
    void slotActivated(int);
    void slotPopupDialog();

private:
    QT_PIMPL(QtColorComboBox)
};


#endif // QTCOLORCOMBOBOX_H
