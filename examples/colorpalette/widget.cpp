#include "widget.h"
#include <QtWidgets>
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
#include <QPainter>


static QHash<QString, int> filters;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    if (filters.empty()) {
        filters["Hex Text Palette (*.txt *.hextxt)"] = QtColorPalette::HexTxtFormat;
        filters["GIMP Palette (*.cmap *.gpl)"] = QtColorPalette::GimpFormat;
        filters["JASC Palette (*.pal)"] = QtColorPalette::JascFormat;
        filters["RIFF Palette (*.pal)"] = QtColorPalette::RiffFormat;
        filters["Adobe Color Table (*.act)"] = QtColorPalette::ACTFormat;
    }

    gradientView = new QtGradientWidget(this);
    colorGrid = new QtColorGrid(this);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Open|QDialogButtonBox::Save);
    connect(buttonBox, SIGNAL(clicked(QAbstractButton*)), SLOT(clicked(QAbstractButton*)));

    textEdit = new QPlainTextEdit(this);
    textEdit->setReadOnly(true);

    wSizeEdit = new QSpinBox(this);
    wSizeEdit->setRange(8, 128);
    wSizeEdit->setValue(colorGrid->iconSize().width());
    connect(wSizeEdit, SIGNAL(valueChanged(int)), SLOT(setIconWidth(int)));
    hSizeEdit = new QSpinBox(this);
    hSizeEdit->setRange(8, 128);
    hSizeEdit->setValue(colorGrid->iconSize().height());
    connect(hSizeEdit, SIGNAL(valueChanged(int)), SLOT(setIconHeight(int)));

    colorLabel = new QLabel(tr("Color: <unspecified>"), this);
    connect(colorGrid, SIGNAL(colorChanged(QColor)), SLOT(colorSelected(QColor)));

    QHBoxLayout* sizeLayout = new QHBoxLayout;
    sizeLayout->addWidget(new QLabel(tr("Icon Size:")));
    sizeLayout->addWidget(wSizeEdit);
    sizeLayout->addWidget(hSizeEdit);
    sizeLayout->addStretch();

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(gradientView, 1);
    layout->addWidget(colorGrid);
    layout->addWidget(textEdit);
    layout->addLayout(sizeLayout);
    layout->addWidget(colorLabel);
    layout->addWidget(buttonBox);

}

Widget::~Widget()
{

}
void Widget::clicked(QAbstractButton* button)
{
   if (button == (QAbstractButton*)buttonBox->button(QDialogButtonBox::Open))
       open();

   if (button == (QAbstractButton*)buttonBox->button(QDialogButtonBox::Save))
       save();
}

void Widget::colorSelected(const QColor &c)
{
    colorLabel->setText(tr("Color: %1").arg(c.name()));
}

void Widget::setIconWidth(int w)
{
    QSize size = colorGrid->iconSize();
    size.setWidth(w);
    colorGrid->setIconSize(size);
}

void Widget::setIconHeight(int h)
{
    QSize size = colorGrid->iconSize();
    size.setHeight(h);
    colorGrid->setIconSize(size);
}

void Widget::open()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open Color Palette"), QString(), filters.keys().join(";;"));
    if (filePath.isEmpty())
        return;

    int format = QtColorPalette::guessFormat(filePath);
    if (format == -1) {
        QMessageBox::critical(this, tr("Error"), tr("Unknown palette format"));
        return;
    }

    gradientView->readPalette(filePath, format);
    colorGrid->setColors(gradientView->colorPalette());

    textEdit->clear();
    QStringList keys = gradientView->mPalette.keys();
    for (auto it = keys.begin(); it != keys.end(); ++it)
        textEdit->appendPlainText(QString("%1 : %2").arg(*it, gradientView->mPalette.value(*it)));
}

void Widget::save()
{
    QString selected;
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save Color Palette"), QString(), filters.keys().join(";;"), &selected);
    if (filePath.isEmpty())
        return;

    int format = filters[selected];
    gradientView->writePalette(filePath, format);
}






QtGradientWidget::QtGradientWidget(QWidget *parent) :
    QWidget(parent)
{
    setMinimumHeight(16);
}

QtGradientWidget::~QtGradientWidget()
{
}

const QtColorPalette &QtGradientWidget::colorPalette() const
{
    return mPalette;
}

void QtGradientWidget::readPalette(const QString &path, int format)
{
    QFile file(path);
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to open the file '%1': %2")
                              .arg(path, file.errorString()));
        return;
    }
    if (!mPalette.read(&file, format)) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to read palette"));
        return;
    }

    mGradient = mPalette.createGradient(QGradient::LinearGradient);
    static_cast<QLinearGradient&>(mGradient).setStart(x(), y());
    static_cast<QLinearGradient&>(mGradient).setFinalStop(x() + width(), y());
    repaint();
}

void QtGradientWidget::writePalette(const QString &path, int format)
{
    QFile file(path);
    if (!file.open(QFile::WriteOnly)) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to save the file '%1': %2")
                              .arg(path, file.errorString()));
        return;
    }

    if (!mPalette.write(&file, format)) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to write palette"));
    }
}

void QtGradientWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if (mPalette.size() > 0)
        painter.fillRect(rect(), mGradient);
    else
        painter.fillRect(rect(), Qt::white);
}

void QtGradientWidget::resizeEvent(QResizeEvent *event)
{
    static_cast<QLinearGradient&>(mGradient).setStart(x(), y());
    static_cast<QLinearGradient&>(mGradient).setFinalStop(x() + width(), y());
    QWidget::resizeEvent(event);
}
