#include "qtspinboxedit.h"
#include <QMenu>
#include <QSlider>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QStyle>
#include <QPixmap>
#include <QPainter>

class QtSpinBoxEditPrivate
{
public:
    QtSpinBoxEdit* q_ptr;
    QMenu* menu;
    QSlider* slider;

    QtSpinBoxEditPrivate(QtSpinBoxEdit* q)
        : q_ptr(q), menu(Q_NULLPTR), slider(Q_NULLPTR)
    {}

    void createMenu()
    {
        QLineEdit* lineEdit = q_ptr->findChild<QLineEdit*>();
        if (lineEdit)
        {
            menu = new QMenu(q_ptr);

            slider = new QSlider(Qt::Horizontal, menu);
            slider->setRange(q_ptr->minimum(), q_ptr->maximum());
            QObject::connect(slider, &QSlider::valueChanged, q_ptr, &QtSpinBoxEdit::setValue);
            QHBoxLayout* layout = new QHBoxLayout(menu);
            layout->setMargin(1);
            layout->addWidget(slider, 1);

            QAction* action = lineEdit->addAction(icon(), QLineEdit::LeadingPosition);
            QObject::connect(action, &QAction::triggered, q_ptr, &QtSpinBoxEdit::showEditor);
        }
    }

    QIcon icon() const
    {
        Q_CONSTEXPR QPoint polygon[] = { { -4, -2 }, { 4, -2 }, { 0, 2 } };
        static QPixmap pixmap;
        if (pixmap.isNull())
        {
            pixmap = QPixmap(16, 16);
            pixmap.fill(Qt::transparent);

            QPainter painter(&pixmap);
            painter.setRenderHint(QPainter::Antialiasing);
            painter.translate(pixmap.rect().center());
            painter.setPen(Qt::NoPen);
            painter.setBrush(Qt::darkGray);
            painter.drawPolygon(polygon, 3);
        }
        return pixmap;
    }
};


QtSpinBoxEdit::QtSpinBoxEdit(QWidget *parent)
    : QSpinBox(parent), d_ptr(new QtSpinBoxEditPrivate(this))
{
    Q_D(QtSpinBoxEdit);
    d->createMenu();
}

QtSpinBoxEdit::~QtSpinBoxEdit()
{
}

void QtSpinBoxEdit::showEditor()
{
    Q_D(QtSpinBoxEdit);
    if (d->menu && d->slider)
    {
        d->slider->setRange(minimum(), maximum());
        d->slider->setValue(value());
        d->menu->setMinimumWidth(width());
        d->menu->move(mapToGlobal(rect().bottomLeft()));
        d->menu->exec();
    }
}
