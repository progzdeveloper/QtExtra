#ifndef QTITEMFILTER_H
#define QTITEMFILTER_H

#include <QObject>
#include <QModelIndex>
#include <QVariant>
#include <QVector>
#include <QHash>
#include <QIcon>
#include <QFont>
#include <QColor>

#include <functional>

#include <QtWidgetsExtra>

class QTWIDGETSEXTRA_EXPORT QtProxyModelIndex
{
public:
    QtProxyModelIndex(const QModelIndex& index, int role) :
        mIndex(index), mRole(role) {
    }

    ~QtProxyModelIndex() {}

    inline int role() const { return mRole; }

    inline int row() const { return mIndex.row(); }
    inline int column() const { return mIndex.column(); }

    inline QtProxyModelIndex parent() const {
        return QtProxyModelIndex(mIndex.parent(), mRole);
    }

    inline QtProxyModelIndex sibling(int row, int column) const {
        return QtProxyModelIndex(mIndex.sibling(row, column), mRole);
    }

    inline QVariant data() const {
        return mIndex.data(mRole);
    }

    inline QVariant data(int role) const {
        auto it = mCache.find(role);
        return (it == mCache.end() ? mIndex.data(role) : *it);
    }

    inline void setData(int role, const QVariant& value) {
        mCache[role] = value;
    }

    inline bool isValid() const {
        return mIndex.isValid();
    }

    QModelIndex index() const {
        return mIndex;
    }

private:
    QHash<int, QVariant> mCache;
    QModelIndex mIndex;
    int mRole;
};

class QtAbstractItemFilter
{
    Q_GADGET
    Q_DISABLE_COPY(QtAbstractItemFilter)

    Q_PROPERTY(QString objectName READ objectName WRITE setObjectName)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)
public:
    QtAbstractItemFilter();
    virtual ~QtAbstractItemFilter();

    void setObjectName(const QString& name);
    QString objectName() const;

    void setEnabled(bool on = true);
    bool isEnabled() const;

    virtual bool accepted(const QModelIndex& index) const = 0;
    virtual bool isRoleSupported(int /*role*/) const = 0;

protected:
    virtual bool accepts(const QVariant& v) const = 0;

protected:
    QString mObjectName;
    bool mEnabled;
};

template<class _Predicate>
class QtCustomItemFilter : public QtAbstractItemFilter
{
public:
    QtCustomItemFilter(_Predicate&& p, int role = Qt::DisplayRole) :
        mPred(std::forward<_Predicate>(p)), mRole(role)
    {
    }

    void setPatternRole(int role) { mRole = role; }
    int patternRole() const { return mRole; }

    bool accepted(const QModelIndex& index) const Q_DECL_OVERRIDE
    {
        if (isEnabled())
            return accepts(index.data(mRole));
        return true;
    }

    bool isRoleSupported(int role) const Q_DECL_OVERRIDE
    {
        return (mRole == role);
    }

protected:
     bool accepts(const QVariant& v) const Q_DECL_OVERRIDE
     {
         return mPred(v);
     }
private:
     _Predicate mPred;
     int mRole;
};

class QTWIDGETSEXTRA_EXPORT QtItemFilter : public QtAbstractItemFilter
{
    Q_GADGET
    Q_DISABLE_COPY(QtItemFilter)

    Q_PROPERTY(Condition condition READ condition WRITE setCondition)
    Q_PROPERTY(Qt::MatchFlags matchFlags READ matchFlags WRITE setMatchFlags)
    Q_PROPERTY(RegexOptions regexOptions READ regexOptions WRITE setRegexOptions)
    Q_PROPERTY(QString patternString READ patternString WRITE setPatternString)
    Q_PROPERTY(Type patternType READ patternType WRITE setPatternType)
    Q_PROPERTY(int patternRole READ patternRole WRITE setPatternRole)

public:
    enum Type {
        Invalid = QMetaType::UnknownType,
        Bool = QMetaType::Bool,
        Int = QMetaType::Int,
        UInt = QMetaType::UInt,
        LongLong = QMetaType::LongLong,
        ULongLong = QMetaType::ULongLong,
        Double = QMetaType::Double,
        Char = QMetaType::QChar,
        Map = QMetaType::QVariantMap,
        List = QMetaType::QVariantList,
        String = QMetaType::QString,
        StringList = QMetaType::QStringList,
        ByteArray = QMetaType::QByteArray,
        BitArray = QMetaType::QBitArray,
        Date = QMetaType::QDate,
        Time = QMetaType::QTime,
        DateTime = QMetaType::QDateTime,
        Url = QMetaType::QUrl,
        Locale = QMetaType::QLocale,
        Rect = QMetaType::QRect,
        RectF = QMetaType::QRectF,
        Size = QMetaType::QSize,
        SizeF = QMetaType::QSizeF,
        Line = QMetaType::QLine,
        LineF = QMetaType::QLineF,
        Point = QMetaType::QPoint,
        PointF = QMetaType::QPointF,
        RegExp = QMetaType::QRegExp,
        RegularExpression = QMetaType::QRegularExpression,
        Hash = QMetaType::QVariantHash,
        EasingCurve = QMetaType::QEasingCurve,
        Uuid = QMetaType::QUuid,

        Font = QMetaType::QFont,
        Pixmap = QMetaType::QPixmap,
        Brush = QMetaType::QBrush,
        Color = QMetaType::QColor,
        Palette = QMetaType::QPalette,
        Image = QMetaType::QImage,
        Polygon = QMetaType::QPolygon,
        Region = QMetaType::QRegion,
        Bitmap = QMetaType::QBitmap,
        Cursor = QMetaType::QCursor,
        KeySequence = QMetaType::QKeySequence,
        Pen = QMetaType::QPen,
        TextLength = QMetaType::QTextLength,
        TextFormat = QMetaType::QTextFormat,
        Matrix = QMetaType::QMatrix,
        Transform = QMetaType::QTransform,
        Matrix4x4 = QMetaType::QMatrix4x4,
        Vector2D = QMetaType::QVector2D,
        Vector3D = QMetaType::QVector3D,
        Vector4D = QMetaType::QVector4D,
        Quaternion = QMetaType::QQuaternion,
        PolygonF = QMetaType::QPolygonF,
        Icon = QMetaType::QIcon,
        SizePolicy = QMetaType::QSizePolicy,

        UserType = QMetaType::User
    };
    Q_ENUM(Type)

    enum Condition {
        None         = 0x0,
        Equal        = 0x1,
        NotEqual     = 0x2,
        Less         = 0x4,
        Greater      = 0x8,
        Match        = 0x10,
        LessEqual    = Less|Equal,
        GreaterEqual = Greater|Equal
    };
    Q_ENUM(Condition)

    enum RegexOption {
        NoOptions      = 0x0,
        MultiLine      = 0x1,
        Unicode        = 0x2,
        ExtendedSyntax = 0x4,
        Optimize       = 0x8
    };
    Q_DECLARE_FLAGS(RegexOptions, RegexOption)
    Q_FLAG(RegexOptions)

    QtItemFilter();
    virtual ~QtItemFilter();

    void setPattern(const QVariant& pattern);
    QVariant pattern() const;

    // for design properties
    void setPatternString(const QString& pattern);
    QString patternString() const;

    // for design properties
    void setPatternType(Type t);
    Type patternType() const;

    void setPatternRole(int role);
    int patternRole() const;

    void setCondition(Condition c);
    Condition condition() const;

    void setMatchFlags(Qt::MatchFlags f);
    Qt::MatchFlags matchFlags() const;

    void setRegexOptions(RegexOptions opt);
    RegexOptions regexOptions() const;

    bool isRoleSupported(int /*role*/) const Q_DECL_OVERRIDE
    {
        return true;
    }

    virtual Qt::ItemFlags flags(Qt::ItemFlags f) const
    {
        return f;
    }

    bool accepted(const QModelIndex& index) const Q_DECL_OVERRIDE;

protected:
    bool accepts(const QVariant& v) const Q_DECL_OVERRIDE;

private:
    QScopedPointer<class QtItemFilterPrivate> d_ptr;
    Q_DECLARE_PRIVATE(QtItemFilter)
};


class QTWIDGETSEXTRA_EXPORT QtItemMapper : public QtItemFilter
{
    //Q_GADGET
public:
    virtual QVariant map(const QtProxyModelIndex& index) const {
        if (isEnabled() && index.isValid()) {
            if (accepts(index.data(patternRole()))) // bypass accepted method
                return data(index);
        }
        return index.data();
    }
protected:
    virtual QVariant data(const QtProxyModelIndex& index) const {
        return index.data();
    }
};


class QTWIDGETSEXTRA_EXPORT QtItemFormatter :
        public QtItemMapper
{
    Q_GADGET
    Q_PROPERTY(QString format READ format WRITE setFormat)
    Q_PROPERTY(int textRole READ textRole WRITE setTextRole)
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)
public:
    typedef std::function<QString(const QVariant&)> Formatter;

    QtItemFormatter();
    virtual ~QtItemFormatter();

    void setFormatter(int type, const Formatter& formatter);
    Formatter formatter(int type) const;

    // pattern syntax: {0} {1} ...
    void setFormat(const QString& format);
    QString format() const;

    void setTextRole(int role);
    int textRole() const;

    void setOrientation(Qt::Orientation orientation);
    Qt::Orientation orientation() const;

    void setModel(QAbstractItemModel* model);
    QAbstractItemModel* model() const;

    void setSiblings(const QVector<int>& s);
    QVector<int> siblings() const;

    bool isRoleSupported(int role) const Q_DECL_OVERRIDE {
        return (role == Qt::DisplayRole ||
                role == Qt::ToolTipRole ||
                role == Qt::StatusTipRole);
    }

    virtual QString format(const QVariant& value) const;

    // QtItemMapper interface
protected:
    QVariant data(const QtProxyModelIndex &index) const Q_DECL_OVERRIDE;
    QString formatted(const QtProxyModelIndex &index) const;

private:
    QScopedPointer<class QtItemFormatterPrivate> d_ptr;
    Q_DECLARE_PRIVATE(QtItemFormatter)
};


class QTWIDGETSEXTRA_EXPORT QtRichTextFormatter :
        public QtItemFormatter
{
    Q_GADGET
    Q_PROPERTY(QColor foreground READ foreground WRITE setForeground)
    Q_PROPERTY(QColor background READ background WRITE setBackground)
    Q_PROPERTY(QFont font READ font WRITE setFont)
    Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment)

public:
    QtRichTextFormatter();
    virtual ~QtRichTextFormatter();

    void setForeground(const QColor& c);
    QColor foreground() const;

    void setBackground(const QColor& c);
    QColor background() const;

    void setFont(const QFont& font);
    QFont font() const;

    void setAlignment(Qt::Alignment align);
    Qt::Alignment alignment() const;

    // QtItemMapper interface
protected:
    QVariant data(const QtProxyModelIndex &index) const Q_DECL_OVERRIDE;

private:
    QScopedPointer<class QtRichTextFormatterPrivate> d_ptr;
    Q_DECLARE_PRIVATE(QtRichTextFormatter)
};


class QTWIDGETSEXTRA_EXPORT QtItemHighlighter :
        public QtItemMapper
{
    Q_GADGET
    Q_PROPERTY(QColor foreground READ foreground WRITE setForeground)
    Q_PROPERTY(QColor background READ background WRITE setBackground)
    Q_PROPERTY(QFont font READ font WRITE setFont)
    Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon)

public:
    QtItemHighlighter();
    virtual ~QtItemHighlighter();

    void setForeground(const QColor& c);
    QColor foreground() const;

    void setBackground(const QColor& c);
    QColor background() const;

    void setFont(const QFont& font);
    QFont font() const;

    void setAlignment(Qt::Alignment align);
    Qt::Alignment alignment() const;

    void setIcon(const QIcon& icon);
    void setIcon(const QString& path);
    QIcon icon() const;

    // QtItemFilter interface
public:
    bool isRoleSupported(int role) const Q_DECL_OVERRIDE {
        return (role == Qt::DecorationRole ||
                role == Qt::ForegroundRole ||
                role == Qt::BackgroundRole ||
                role == Qt::FontRole ||
                role == Qt::TextAlignmentRole);
    }

    // QtItemMapper interface
protected:
    QVariant data(const QtProxyModelIndex &index) const Q_DECL_OVERRIDE;

private:
    QScopedPointer<class QtItemHighlighterPrivate> d_ptr;
    Q_DECLARE_PRIVATE(QtItemHighlighter)
};


/*
class QtItemModelMapper
{
public:
    QtItemModelMapper();
    virtual ~QtItemModelMapper();

    void setModelIndex(int i);
    int modelIndex() const;

    void setIndexRole(int role);
    int indexRole() const;

    void setOrientation(Qt::Orientation orientation);
    Qt::Orientation orientation() const;

    void setModel(QAbstractItemModel* model);
    QAbstractItemModel* model() const;

    // QtItemMapper interface
protected:
    virtual QVariant data(const QtProxyModelIndex &index) const Q_DECL_OVERRIDE;
};
*/


#endif // QTITEMFILTER_H
