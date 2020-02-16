#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtColorPalette>
#include <QtColorGrid>

class QLabel;
class QSpinBox;
class QAbstractButton;
class QDialogButtonBox;
class QPlainTextEdit;
class QtGradientWidget;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();
private slots:
    void open();
    void save();
    void clicked(QAbstractButton* button);
    void colorSelected(const QColor& c);
    void setIconWidth(int w);
    void setIconHeight(int h);
private:
    int guessFormat(const QString& filePath);
private:
    QtGradientWidget* gradientView;
    QtColorGrid* colorGrid;
    QPlainTextEdit* textEdit;
    QDialogButtonBox* buttonBox;
    QSpinBox* wSizeEdit;
    QSpinBox* hSizeEdit;
    QLabel* colorLabel;
};



class QtGradientWidget : public QWidget
{
    Q_OBJECT

public:
    QtGradientWidget(QWidget *parent = 0);
    ~QtGradientWidget();

    const QtColorPalette& colorPalette() const;
    void readPalette(const QString& path, int format);
    void writePalette(const QString &path, int format);
    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

public:
    QtColorPalette mPalette;
    QGradient mGradient;
};


#endif // WIDGET_H
