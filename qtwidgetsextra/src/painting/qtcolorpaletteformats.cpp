#include <QRegExp>
#include <QTextStream>
#include <QtColorPalette>
#include <QtEndian>


bool readHexPalette(QIODevice* device, QtColorPalette& palette)
{
    QColor color;
    QString line;
    QTextStream stream(device);

    while(!stream.atEnd())
    {
        line = stream.readLine();
        if (line.isEmpty())
            continue; // skip empty lines

        // skip comments
        if (line.startsWith(';')) {
            continue;
        }

        bool isOk = false;
        QRgb rgb = line.toUInt(&isOk, 16);
        if (!isOk)
            continue;

        color.setRgba(rgb);
        palette.insert(color);
    }
    return true;
}

bool writeHexPalette(QIODevice* device, const QtColorPalette& palette)
{
    QTextStream stream(device);
    QStringList keys = palette.keys();
    for (auto it = keys.begin(); it != keys.end(); ++it)
        stream << "; " << *it << ": " << palette.value(*it) << endl;

    for (int i = 0; i < palette.size(); i++)
    {
        stream << hex << (quint32)palette.at(i).rgba() << endl;
        if (stream.status() != QTextStream::Ok)
            return false;
    }
    return true;
}



bool readGimpPalette(QIODevice* device, QtColorPalette& palette)
{
    static QRegExp colorRegExp("(\\d+) (\\d+) (\\d+)");
    static QRegExp metaRegExp("(\\w+): (\\w+.*)");

    QColor color;
    QString line;
    QTextStream stream(device);
    line = stream.readLine();
    if (line != "GIMP Palette")
        return false;
    while(!stream.atEnd())
    {
        line = stream.readLine();
        if (line.isEmpty())
            continue; // skip empty lines

        // skip comments
        if (line.startsWith('#')) {
            // try to parse metadata
            if (metaRegExp.indexIn(line) > -1) {
                palette.setValue(metaRegExp.cap(1), metaRegExp.cap(2));
            }
            continue;
        }

        if (line.startsWith("Name")) {
            if (metaRegExp.indexIn(line) > -1)
                palette.setValue(metaRegExp.cap(1), metaRegExp.cap(2));
        }

        // try to parse color
        if (colorRegExp.indexIn(line) > -1)
        {
            QString r = colorRegExp.cap(1);
            QString g = colorRegExp.cap(2);
            QString b = colorRegExp.cap(3);

            color.setRgb(r.toInt(), g.toInt(), b.toInt());
            palette.insert(color);
        }
    }
    return true;
}

bool writeGimpPalette(QIODevice* device, const QtColorPalette& palette)
{
    QTextStream stream(device);
    stream <<  "GIMP Palette" << endl;
    QStringList keys = palette.keys();
    for (auto it = keys.begin(); it != keys.end(); ++it) {
        if (*it != "Name")
            stream << "# ";
         stream << *it << ": " << palette.value(*it) << endl;
    }

    for (int i = 0; i < palette.size(); i++)
    {
        const QColor& c = palette.at(i);
        stream << c.red() << ' '
               << c.green() << ' '
               << c.blue() << " Untitled" << endl;
        if (stream.status() != QTextStream::Ok)
            return false;
    }
    return true;
}




bool readJascPalette(QIODevice* device, QtColorPalette& palette)
{
    static QRegExp colorRegExp("(\\d+) (\\d+) (\\d+)");

    QColor color;
    QString line;
    QTextStream stream(device);
    line = stream.readLine();
    if (line != "JASC-PAL")
        return false;

    line = stream.readLine();
    palette.setValue("Version", line);
    line = stream.readLine(); // number of colors - skip that
    palette.setValue("Color-Count", line);

    if (stream.status() != QTextStream::Ok)
        return false;

    while(!stream.atEnd())
    {
        line = stream.readLine();
        if (line.isEmpty())
            continue; // skip empty lines

        // skip comments
        if (line.startsWith(';')) {
            continue;
        }

        // try to parse color
        if (colorRegExp.indexIn(line) > -1)
        {
            QString r = colorRegExp.cap(1);
            QString g = colorRegExp.cap(2);
            QString b = colorRegExp.cap(3);

            color.setRgb(r.toInt(),
                         g.toInt(),
                         b.toInt());
            palette.insert(color);
        }
    }
    return true;
}

bool writeJascPalette(QIODevice* device, const QtColorPalette& palette)
{
    QTextStream stream(device);
    stream << "JASC-PAL" << endl;
    stream << 0100 << endl; // version
    stream << 255 << endl; // number of colors

    for (int i = 0; i < palette.size(); i++)
    {
        const QColor& c = palette.at(i);
        stream << c.red() << ' '
               << c.green() << ' '
               << c.blue() << endl;
        if (stream.status() != QTextStream::Ok)
            return false;
    }
    return true;
}




// see: https://www.adobe.com/devnet-apps/photoshop/fileformatashtml/#50577411_pgfId-1070626
struct AdobeColorTable
{
    static Q_CONSTEXPR size_t MAX_SIZE = 256; // color table size
    static Q_CONSTEXPR size_t BPC = 3; // bytes per color

    quint8 colors[MAX_SIZE * BPC];
    struct {
        quint16 count; // number of colors
        quint16 transparent; // transparent color
    } extra;
};

// see: https://www.adobe.com/devnet-apps/photoshop/fileformatashtml/#50577411_pgfId-1070626
// example: https://github.com/geoffb/dawnbringer-palettes
bool readAdobeColorTable(QIODevice* device, QtColorPalette& palette)
{
    AdobeColorTable act;
    memset(&act, 0, sizeof(act));

    quint64 bytesRead = 0;

    bytesRead = device->read(reinterpret_cast<char*>(&act), sizeof(act.colors));
    if (bytesRead < sizeof(act.colors))
        return false;

    bytesRead = device->read(reinterpret_cast<char*>(&act.extra), sizeof(act.extra));
    if (bytesRead != sizeof(act.extra)) {
        act.extra.count = AdobeColorTable::MAX_SIZE;
        act.extra.transparent = 0xFFFF;
    }
#ifdef Q_LITTLE_ENDIAN
    act.extra.count = qFromBigEndian(act.extra.count);
    act.extra.transparent = qFromBigEndian(act.extra.transparent);
#endif

    QColor color;
    const quint8* first = act.colors;
    const quint8* last = act.colors + act.extra.count * AdobeColorTable::BPC;
    for (; first != last; first += AdobeColorTable::BPC) {
        // color.setRgb(first[2], first[1], first[0]); // ????
        color.setRgb(first[0], first[1], first[2]);
        palette.insert(color);
    }
    return true;
}

bool writeAdobeColorTable(QIODevice* device, const QtColorPalette& palette)
{
    AdobeColorTable act;
    memset(&act, 0, sizeof(act));

    QRgb rgb;
    quint8* first = act.colors;
    quint16 n = (quint16)qMin(255, palette.size());
    for (int i = 0; i < n; i++, first += AdobeColorTable::BPC) {
        rgb = palette.at(i).rgb();
        first[0] = (quint8)qRed(rgb);
        first[1] = (quint8)qGreen(rgb);
        first[2] = (quint8)qBlue(rgb);
    }
    act.extra.count = qFromBigEndian(n);
    act.extra.transparent = 0xFFFF;
    return (device->write((const char*)&act, sizeof(act)) == sizeof(AdobeColorTable));
}


static Q_CONSTEXPR const char MsRiffId[sizeof(quint32)] = { 'R', 'I', 'F', 'F' };
static Q_CONSTEXPR const char MsPalId[sizeof(quint32)] = { 'P', 'A', 'L', 0x20 };
static Q_CONSTEXPR const char DataChunkId[sizeof(quint32)] = { 'd', 'a', 't', 'a' };


typedef struct tagRIFFPALETTEENTRY {
  quint8 peRed;
  quint8 peGreen;
  quint8 peBlue;
  quint8 peFlags;
} RIFFPALETTEENTRY;


union RIFFBUF
{
    quint32 ui32; // size
    char buf[sizeof(quint32)]; // character buf
    struct {
      quint16  palVersion; // palette version
      quint16  palNumEntries; // number of colors
    };
};

bool readRiffPalette(QIODevice* device, QtColorPalette& palette)
{
    RIFFBUF buf;
    if (device->read((char*)&buf, sizeof(RIFFBUF)) != sizeof(RIFFBUF))
        return false;
    if (::memcmp(&buf, MsRiffId, sizeof(RIFFBUF)) != 0)
        return false;

    // read size excluding header
    if (device->read((char*)&buf, sizeof(RIFFBUF)) != sizeof(RIFFBUF))
        return false;

    if (device->read((char*)&buf, sizeof(RIFFBUF)) != sizeof(RIFFBUF))
        return false;
    if (::memcmp(&buf, MsPalId, sizeof(RIFFBUF)) != 0)
        return false;

    while(!device->atEnd()) {
        if (device->read((char*)&buf, sizeof(RIFFBUF)) != sizeof(RIFFBUF))
            return false;
        if (::memcmp(&buf, DataChunkId, sizeof(RIFFBUF)) == 0)
            break;
    }

    // read data chunk size
    if (device->read((char*)&buf, sizeof(RIFFBUF)) != sizeof(RIFFBUF))
        return false;

    if (device->read((char*)&buf, sizeof(RIFFBUF)) != sizeof(RIFFBUF))
        return false;

    palette.setValue("Version", QString::number(buf.palVersion, 16));

    QColor color;
    RIFFPALETTEENTRY palEntry;
    for (int i = 0; i < buf.palNumEntries; i++) {
        if (device->read((char*)&palEntry, sizeof(RIFFPALETTEENTRY)) != sizeof(RIFFPALETTEENTRY))
            return false;
        color.setRgb(palEntry.peRed, palEntry.peGreen, palEntry.peBlue);
        palette.insert(color);
    }
    return true;
}

bool writeRiffPalette(QIODevice* device, const QtColorPalette& palette)
{
    Q_UNUSED(device)
    Q_UNUSED(palette)
    // TODO: implement me!

    return false;
}


