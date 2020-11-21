#ifndef QTMESSAGELOGWIDGET_H
#define QTMESSAGELOGWIDGET_H
#include <QtWidgetsExtra>
#include <QWidget>
#if QT_VERSION < 0x050000
#include <QtGui/QAbstractScrollArea>
#else
#include <QtWidgets/QAbstractScrollArea>
#endif

QT_BEGIN_NAMESPACE
class QString;
class QStringList;
class QColor;
class QRegularExpression;
QT_END_NAMESPACE

class QTWIDGETSEXTRA_EXPORT QtMessageLogWidget :
        public QAbstractScrollArea
{
    Q_OBJECT
    Q_PROPERTY( uint historySize READ historySize WRITE setHistorySize )
    Q_PROPERTY( QString text READ text )
    Q_PROPERTY( uint minimumVisibleLines READ minimumVisibleLines WRITE setMinimumVisibleLines )
    Q_PROPERTY( uint minimumVisibleColumns READ minimumVisibleColumns WRITE setMinimumVisibleColumns )
    Q_PROPERTY( bool alternatingRowColors READ alternatingRowColors WRITE setAlternatingRowColors )

public:
    /*!
     * Constructor. Creates an empty QtMessageLogWidget.
     */
    explicit QtMessageLogWidget( QWidget * parent = Q_NULLPTR );
    /*!
     * Destructor.
     */
    ~QtMessageLogWidget();


    /*!
     * \property QtMessageLogWidget::historySize
     *
     * Specifies the maximum number of lines this widget will hold before
     * dropping old lines. The default is INT_MAX (ie. essentially unlimited).
     * Get this property's value using %historySize(), and set it with
     * %setHistorySize().
     */
    void setHistorySize( unsigned int size );
    unsigned int historySize() const;

    /*!
     * \property QtMessageLogWidget::text
     *
     * Contains the current %text as a single string.
     */
    QString text() const;

    /*!
     * \property QtMessageLogWidget::minimumVisibleLines
     * Specifies the number of lines that should be visible at any one
     * time. The default is 1 (one).
     * Get this property's value using %minimumVisibleLines(), and set it
     * using %setMinimumVisibleLines().
     */
    void setMinimumVisibleLines( unsigned int num );
    unsigned int minimumVisibleLines() const;
    /*!
     * \property QtMessageLogWidget::minimumVisibleColumns
     *
     * Specifies the number of columns that should be visible at any one
     * time. The default is 1 (one). The width is calculated using
     * QFontMetrics::averageCharWidth(), if that is available. Otherwise,
     * the width of \c M is used.
     * Get this property's value using %minimumVisibleColumns(), and set it
     * using %setMinimumVisibleColumns().
     */
    void setMinimumVisibleColumns( unsigned int num );
    unsigned int minimumVisibleColumns() const;

    /*!
     * \property QtMessageLogWidget::alternatingRowColors
     *
     * Specifies whether the background should be drawn using
     * row-alternating colors. The default is \c false.
     * Get this property's value using %alternatingRowColors(), and set it
     * using %setAlternatingRowColors().
     */
    void setAlternatingRowColors( bool on );
    bool alternatingRowColors() const;

    int setupStyle(const QRegularExpression& expr, const QColor& background, const QColor& foreground);
    int removeStyle(int id);
    void clearStyles();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

public Q_SLOTS:
    /*!
     * Clears the text.
     */
    void clear();
    /*!
     * Appends \a str to the view, highlighting the line
     */
    virtual void message( const QString & msg );
/*protected:
    virtual void highlight( const QString& text );*/

    // QAbstractScrollArea interface
protected:
    void scrollContentsBy(int dx, int dy) Q_DECL_OVERRIDE;

    // QWidget interface
protected:
    void paintEvent( QPaintEvent * ) Q_DECL_OVERRIDE;
    void timerEvent( QTimerEvent * ) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent * ) Q_DECL_OVERRIDE;
    void changeEvent( QEvent * ) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent * ) Q_DECL_OVERRIDE;

private:
    QT_PIMPL(QtMessageLogWidget)
};





#endif // QTMESSAGELOGWIDGET_H
