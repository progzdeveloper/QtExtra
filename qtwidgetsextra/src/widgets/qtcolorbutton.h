#pragma once
#include <QToolButton>
#include <QColor>

#include <QtWidgetsExtra>

typedef QVector<QColor> QtColorSet;

class QTWIDGETSEXTRA_EXPORT QtColorButton :
        public QToolButton
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor USER true)
    Q_PROPERTY(PopupStyle popupStyle READ popupStyle WRITE setPopupStyle NOTIFY popupStyleChanged)

    Q_DISABLE_COPY(QtColorButton)
public:

    enum PopupStyle
    {
        GridPopup,
        ListPopup
    };
    Q_ENUM(PopupStyle)

    explicit QtColorButton(QWidget* parent = Q_NULLPTR);
    explicit QtColorButton(const QStringList& colorNames, QWidget* parent = Q_NULLPTR);
    explicit QtColorButton(const QtColorSet& colorSet, QWidget* parent = Q_NULLPTR);

    ~QtColorButton();

    QColor color() const;

    void setColors(const QtColorSet& colorSet);
    QtColorSet colors() const;

    void setGridWidth(int width);
    int gridWidth() const;

    PopupStyle popupStyle() const;

    void updateMenu();

public Q_SLOTS:
    void setColor(const QColor& c);
    void setPopupStyle(PopupStyle style);

private Q_SLOTS:
    void colorSelected();
    void pickColor();

Q_SIGNALS:
    void colorChanged(const QColor& c);
    void popupStyleChanged(PopupStyle);

private:
    QT_PIMPL(QtColorButton)
};


