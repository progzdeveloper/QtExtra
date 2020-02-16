#include "qtcolorpalette.h"
#include <QPair>
#include <QIODevice>
#include <QFileInfo>


bool readHexPalette(QIODevice* device, QtColorPalette& palette);
bool writeHexPalette(QIODevice* device, const QtColorPalette& palette);

bool readGimpPalette(QIODevice* device, QtColorPalette& palette);
bool writeGimpPalette(QIODevice* device, const QtColorPalette& palette);

bool readJascPalette(QIODevice* device, QtColorPalette& palette);
bool writeJascPalette(QIODevice* device, const QtColorPalette& palette);

bool readRiffPalette(QIODevice* device, QtColorPalette& palette);
bool writeRiffPalette(QIODevice* device, const QtColorPalette& palette);

bool readAdobeColorTable(QIODevice* device, QtColorPalette& palette);
bool writeAdobeColorTable(QIODevice* device, const QtColorPalette& palette);

struct QtColorPaletteFormat
{
    QPair<
        QtColorPalette::CustomRead,
        QtColorPalette::CustomWrite
    > formats[QtColorPalette::MaxFormat];

    QtColorPaletteFormat()
    {
        ::memset(formats, 0, sizeof(formats));

        formats[QtColorPalette::HexTxtFormat] = {
            &readHexPalette, &writeHexPalette
        };

        formats[QtColorPalette::GimpFormat] = {
            &readGimpPalette, &writeGimpPalette
        };

        formats[QtColorPalette::JascFormat] = {
            &readJascPalette, &writeJascPalette
        };

        formats[QtColorPalette::RiffFormat] = {
            &readRiffPalette, &writeRiffPalette
        };

        formats[QtColorPalette::ACTFormat] = {
            &readAdobeColorTable, &writeAdobeColorTable
        };
    }

    bool registrate(QtColorPalette::CustomRead reader,
                    QtColorPalette::CustomWrite writer,
                    int format)
    {
        if (reader == Q_NULLPTR ||
            writer == Q_NULLPTR)
            return false;

        if (format < QtColorPalette::UserFormat ||
            format >= QtColorPalette::MaxFormat)
            return false;

        formats[format] = qMakePair(reader, writer);
        return true;
    }

    bool isSupported(int format) const
    {
        if (format < 0 || format >= QtColorPalette::MaxFormat)
            return false;
        return (formats[format].first != Q_NULLPTR &&
                formats[format].second != Q_NULLPTR);
    }
};
static QtColorPaletteFormat __globalColorPaletteFormats;




static inline void setupGradient(const QVector<QColor>& colors, QGradient& gradient)
{
    qreal pos = 0;
    qreal offset = qreal(1) / (qreal)colors.size();
    for (auto it = colors.begin(); it != colors.end(); ++it, pos += offset) {
        gradient.setColorAt(pos, *it);
    }
}




QtColorPalette::QtColorPalette()
{
}

QtColorPalette::~QtColorPalette()
{
}

QString QtColorPalette::value(const QString &key) const
{
    auto it = mMetaData.constFind(key);
    return (it != mMetaData.end() ? (*it) : QString());
}

void QtColorPalette::setValue(const QString &key, const QString& value)
{
    mMetaData[key] = value;
}

QStringList QtColorPalette::keys() const
{
    return mMetaData.keys();
}

const QVector<QColor>& QtColorPalette::colors() const
{
    return mColors;
}

int QtColorPalette::size() const
{
    return mColors.size();
}

bool QtColorPalette::isEmpty() const
{
    return mColors.isEmpty();
}

QColor &QtColorPalette::at(int i)
{
    return mColors[i];
}

const QColor &QtColorPalette::at(int i) const
{
    return mColors.at(i);
}

void QtColorPalette::insert(const QColor &c)
{
    return mColors.push_back(c);
}

void QtColorPalette::erase(int i)
{
    mColors.erase(mColors.begin() + i);
}

void QtColorPalette::clear()
{
    mColors.clear();
    mMetaData.clear();
}

QGradient QtColorPalette::createGradient(QGradient::Type t) const
{
    switch (t) {
        case QGradient::LinearGradient: {
            QLinearGradient linear;
            setupGradient(mColors, linear);
            return linear;
        }
        case QGradient::RadialGradient: {
            QRadialGradient radial;
            setupGradient(mColors, radial);
            return radial;
        }
        case QGradient::ConicalGradient: {
            QConicalGradient conical;
            setupGradient(mColors, conical);
            return conical;
        }
        default:
            break;
    }
    return QGradient();
}

bool QtColorPalette::read(QIODevice *device, int format)
{
    mColors.clear();
    mMetaData.clear();
    if (device == Q_NULLPTR)
        return false;
    if (!device->isOpen() || !device->isReadable())
        return false;
    if (!__globalColorPaletteFormats.isSupported(format))
        return false;
    return __globalColorPaletteFormats.formats[format].first(device, *this);
}

bool QtColorPalette::write(QIODevice *device, int format) const
{
    if (device == Q_NULLPTR)
        return false;
    if (!device->isOpen() || !device->isWritable())
        return false;
    if (!__globalColorPaletteFormats.isSupported(format))
        return false;
    return __globalColorPaletteFormats.formats[format].second(device, *this);
}

bool QtColorPalette::registerCustomFormat(int format, QtColorPalette::CustomRead reader, QtColorPalette::CustomWrite writer)
{
    return __globalColorPaletteFormats.registrate(reader, writer, format);
}

bool QtColorPalette::isFormatSupported(int format)
{
    return __globalColorPaletteFormats.isSupported(format);
}

int QtColorPalette::guessFormat(const QString &filePath)
{
    static const char IsRIFF[] = { 'R', 'I', 'F', 'F' };
    static const char IsGIMP[] = { 'G', 'I', 'M', 'P', ' ', 'P', 'a', 'l', 'e', 't', 't', 'e' };
    static const char IsJASC[] = { 'J', 'A', 'S', 'C', '-', 'P', 'A', 'L' };

    char buf[16];
    QFileInfo fileInfo(filePath);
    QString ext = fileInfo.completeSuffix();

    QFile file(filePath);
    if (!file.open(QFile::ReadOnly)) {
        return UnknownFormat;
    }
    if (file.read(buf, sizeof(buf)) != sizeof(buf))
        return UnknownFormat;

    if (ext == "pal")
    {
        if (memcmp(buf, IsJASC, sizeof(IsJASC)) == 0)
            return QtColorPalette::JascFormat;

        if (memcmp(buf, IsRIFF, sizeof(IsRIFF)) == 0)
            return QtColorPalette::RiffFormat;
    }

    if (ext == "cmap" || ext == "gpl") {
        if (memcmp(buf, IsGIMP, sizeof(IsGIMP)) == 0)
            return QtColorPalette::GimpFormat;
    }

    if (ext == "hextxt" || ext == "txt") {
        return QtColorPalette::HexTxtFormat;
    }

    if (ext == "act") {
        return QtColorPalette::ACTFormat;
    }

    return UnknownFormat;
}

