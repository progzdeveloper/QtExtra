#pragma once
#include <QFrame>
#include <QPixmap>

#include <QtWidgetsExtra>

class QTWIDGETSEXTRA_EXPORT QtSplashWidget :
        public QFrame
{
    Q_OBJECT
    Q_DISABLE_COPY(QtSplashWidget)
public:
    explicit QtSplashWidget(QWidget *parent = Q_NULLPTR);

    explicit QtSplashWidget(const QPixmap& pixmap = QPixmap(),
                   const QString& title = QString(),
                   const QString& copyright = QString(),
                   QWidget *parent = Q_NULLPTR,
                   Qt::WindowFlags f = 0);

    ~QtSplashWidget();

    void setBackground(const QPixmap& pixmap);
    QPixmap background() const;

    void setPixmap(QPixmap& pixmap);
    const QPixmap pixmap() const;

    void setTitle(const QString& title);
    QString title() const;

    void setCopyright(const QString& text);
    QString copyright() const;

    void setRange(int min, int max);
    void setMinimum(int min);
    void setMaximum(int max);
    int minimum() const;
    int maximum() const;

    void repaint();

public Q_SLOTS:
    void showMessage(const QString& text, int alignment, const QColor& color);
    void clearMessage();
    void setProgress(int value);

Q_SIGNALS:
    void messageChanged(const QString& text);

protected:
    virtual void drawContents(QPainter *painter);
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
private:
    QT_PIMPL(QtSplashWidget)
};
