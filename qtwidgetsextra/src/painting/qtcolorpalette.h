#ifndef QTCOLORPALETTE_H
#define QTCOLORPALETTE_H

#include <QColor>
#include <QVector>
#include <QSet>
#include <QHash>
#include <QStringList>
#include <QGradient>

#include <QtWidgetsExtra>

class QIODevice;

class QTWIDGETSEXTRA_EXPORT QtColorPalette
{
public:
    enum Format {
        UnknownFormat = -1,
        HexTxtFormat  = 0, // Text (Paint.NET)
        GimpFormat    = 1, // Text (GIMP)
        JascFormat    = 2, // Text (Corel PaintPro)
        RiffFormat    = 3, // Binary (MS RIFF Palette)
        ACTFormat     = 4, // Binary (Adobe ACT (Adobe Color Table))
        /// TODO: ACOFormat    = 5, // Binary (Adobe ACO (Adobe Color Swatch)
        /// TODO: ACEFormat    = 6, // Binary (Adobe ACE (Adobe Color Exchange)
        UserFormat    = 16,
        MaxFormat     = 32
    };
    typedef bool (*CustomRead)(QIODevice*, QtColorPalette&);
    typedef bool (*CustomWrite)(QIODevice*, const QtColorPalette&);

    QtColorPalette();
    ~QtColorPalette();

    QString value(const QString& key) const;
    void setValue(const QString& key, const QString &value);

    QStringList keys() const;

    const QVector<QColor> &colors() const;

    int size() const;
    bool isEmpty() const;

    QColor& at(int i);
    const QColor& at(int i) const;

    void insert(const QColor& c);
    void erase(int i);
    void clear();

    QGradient createGradient(QGradient::Type t) const;

    bool read(QIODevice* device, int format);
    bool write(QIODevice* device, int format) const;

    static bool registerCustomFormat(int format, CustomRead reader, CustomWrite writer);
    static bool isFormatSupported(int format);
    static int guessFormat(const QString& filePath);

    inline bool operator==(const QtColorPalette& other) const;
    inline bool operator!=(const QtColorPalette& other) const;
private:
    QVector<QColor> mColors;
    QHash<QString, QString> mMetaData;
};

inline bool QtColorPalette::operator==(const QtColorPalette &other) const {
    return (mColors == other.mColors);
}

inline bool QtColorPalette::operator!=(const QtColorPalette &other) const {
    return (mColors != other.mColors);
}

#endif // QTCOLORPALETTE_H
