#include "qtsplashwidget.h"
#include <QtGlobal>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include <QStylePainter>
#include <QStyle>

class QtSplashWidgetPrivate
{
public:
    Q_DECLARE_PUBLIC(QtSplashWidget)

    QtSplashWidget *q_ptr;
    QTextDocument doc;
    QPixmap background;
    QString text;
    QColor currColor;
    int align;

    QLabel *copyrightLabel;
    QLabel *pixmapLabel;
    QLabel *titleLabel;
    QProgressBar *progressBar;


    QtSplashWidgetPrivate(QtSplashWidget* q);
    void init();
    void drawContents();
};


QtSplashWidgetPrivate::QtSplashWidgetPrivate(QtSplashWidget* q) :
    q_ptr(q) {
}


void QtSplashWidgetPrivate::init()
{
    align = Qt::AlignCenter;
    pixmapLabel = new QLabel(q_ptr);
    titleLabel = new QLabel(q_ptr);
    copyrightLabel = new QLabel(q_ptr);
    copyrightLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    copyrightLabel->setAlignment(Qt::AlignCenter);
    progressBar = new QProgressBar(q_ptr);
    progressBar->setTextVisible(false);
    progressBar->setFixedHeight(16);
    progressBar->setAttribute(Qt::WA_OpaquePaintEvent);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(pixmapLabel, 1);
    layout->addWidget(titleLabel, 2);
    layout->setContentsMargins(30, 30, 30, 3);

    QVBoxLayout *mainLayout = new QVBoxLayout(q_ptr);
    mainLayout->addLayout(layout);
    mainLayout->addWidget(progressBar);
    mainLayout->addWidget(copyrightLabel);
}

void QtSplashWidgetPrivate::drawContents()
{
    Q_Q(QtSplashWidget);
    QPixmap texturePixmap = background;
    if (!texturePixmap.isNull())
    {
        QPainter painter(&texturePixmap);

        painter.initFrom(q);
        q->drawContents(&painter);

        QPalette p = q->palette();
        p.setBrush(q->backgroundRole(), QBrush(texturePixmap));
        q->setPalette(p);

        progressBar->setPalette(qApp->palette());
    }
}



QtSplashWidget::QtSplashWidget( QWidget *parent ) :
    //QFrame(parent, Qt::SplashScreen),
    QFrame(parent, Qt::Popup|Qt::FramelessWindowHint|Qt::WindowDoesNotAcceptFocus),
    d_ptr(new QtSplashWidgetPrivate(this))
{
    Q_D(QtSplashWidget);
    d->init();
    d->titleLabel->setText(qApp->applicationName());
    d->copyrightLabel->setText(tr("%1. All rights reserved.").arg(qApp->applicationName()));

    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground);

    setFrameStyle(QFrame::Box|QFrame::Plain);
}

QtSplashWidget::QtSplashWidget( const QPixmap& pixmap /*= QPixmap()*/, 
                                const QString& title /*= QString()*/,
                                const QString& copyright /*= QString()*/,
                                QWidget* parent, /*= Q_NULLPTR*/
                                Qt::WindowFlags f /*= 0*/ ) :
    //QWidget(0, Qt::SplashScreen|f),
    QFrame(parent, f|Qt::Popup|Qt::FramelessWindowHint|Qt::WindowDoesNotAcceptFocus),
    d_ptr(new QtSplashWidgetPrivate(this))
{
    Q_D(QtSplashWidget);
    d->init();
    d->pixmapLabel->setPixmap(pixmap);
    d->titleLabel->setText(title);
    d->copyrightLabel->setText(copyright);

    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground);

    setFrameStyle(QFrame::Box|QFrame::Plain);
}

QtSplashWidget::~QtSplashWidget()
{
}


void QtSplashWidget::repaint()
{
    QWidget::repaint();
    d_ptr->drawContents();
    QApplication::flush();
}

void QtSplashWidget::drawContents( QPainter *painter )
{
    Q_D(QtSplashWidget);
    painter->setPen(d->currColor);
    QRect r = rect();
    r.setRect(r.x() + 5, r.y() + 5, r.width() - 10, r.height() - 10);
    if (Qt::mightBeRichText(d->text)) {
        d->doc.setTextWidth(r.width());
        QTextCursor cursor(&d->doc);
        cursor.select(QTextCursor::Document);
        QTextBlockFormat fmt;
        fmt.setAlignment(static_cast<Qt::Alignment>(d->align));
        cursor.mergeBlockFormat(fmt);
        painter->save();
        painter->translate(r.topLeft());
        d->doc.drawContents(painter);
        painter->restore();
    } else {
        painter->drawText(r, d->align, d->text);
    }
}



void QtSplashWidget::setBackground( const QPixmap& pixmap )
{
    Q_D(QtSplashWidget);

    if (pixmap.hasAlpha()) {
        QPixmap opaque(pixmap.size());
        QPainter p;
        p.begin(&opaque);
        p.fillRect(0, 0, pixmap.width(), pixmap.height(), palette().background());
        p.drawPixmap(0, 0, pixmap);
        p.end();
        d->background = opaque;
    } else {
        d->background = pixmap;
    }

    if (d->background.isNull()) {
        move(QApplication::desktop()->screenGeometry().center() - rect().center());
        return;
    }

    QRect r(0, 0, d->background.size().width(), d->background.size().height());
    resize(d->background.size());
    move(QApplication::desktop()->screenGeometry().center() - r.center());
    if (!isVisible())
        d->drawContents();
    else
        repaint();
}

QPixmap QtSplashWidget::background() const
{
    Q_D(const QtSplashWidget);
    return d->background;
}

void QtSplashWidget::setPixmap( QPixmap& pixmap )
{
    Q_D(QtSplashWidget);
    d->pixmapLabel->setPixmap(pixmap);
}

const QPixmap QtSplashWidget::pixmap() const
{
    Q_D(const QtSplashWidget);
    return (*d->pixmapLabel->pixmap());
}

void QtSplashWidget::setTitle( const QString& title )
{
    Q_D(QtSplashWidget);
    d->titleLabel->setText(title);
}

QString QtSplashWidget::title() const
{
    Q_D(const QtSplashWidget);
    return d->titleLabel->text();
}

void QtSplashWidget::setCopyright( const QString& text )
{
    Q_D(QtSplashWidget);
    d->copyrightLabel->setText(text);
}

QString QtSplashWidget::copyright() const
{
    Q_D(const QtSplashWidget);
    return d->copyrightLabel->text();
}

void QtSplashWidget::setRange( int min, int max )
{
    Q_D(QtSplashWidget);
    d->progressBar->setRange(min, max);
}

void QtSplashWidget::setMinimum( int min )
{
    Q_D(QtSplashWidget);
    d->progressBar->setMinimum(min);
}

void QtSplashWidget::setMaximum( int max )
{
    Q_D(QtSplashWidget);
    d->progressBar->setMaximum(max);
}

int QtSplashWidget::minimum() const
{
    Q_D(const QtSplashWidget);
    return d->progressBar->minimum();
}

int QtSplashWidget::maximum() const
{
    Q_D(const QtSplashWidget);
    return d->progressBar->maximum();
}

void QtSplashWidget::showMessage( const QString& text, int alignment, const QColor& color )
{
    Q_D(QtSplashWidget);
    d->text = text;
    d->align = alignment;
    d->currColor = color;
#ifdef QT_NO_TEXTHTMLPARSER
    d->doc.setPlainText(d->text);
#else
    d->doc.setHtml(d->text);
#endif
    emit messageChanged(d->text);
    repaint();
}

void QtSplashWidget::clearMessage()
{
    Q_D(QtSplashWidget);
    d->text.clear();
    emit messageChanged(d->text);
    repaint();
}

void QtSplashWidget::setProgress( int value )
{
    Q_D(QtSplashWidget);
    d->progressBar->setValue(value);
    qApp->processEvents();
}

void QtSplashWidget::paintEvent(QPaintEvent *)
{
    Q_D(QtSplashWidget);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QStyleOptionFrame opt;
#else
    QStyleOptionFrameV3 opt;
#endif
    opt.initFrom(this);

    QStylePainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Qt::transparent);
    painter.drawPrimitive(QStyle::PE_PanelTipLabel, opt);

    //painter.setPen(d->currColor);
    //painter.drawText(rect().adjusted(2, 2, -2, -2), d->align, d->text);

    painter.drawPixmap(1, 1, d->background);
}
