#include "widget.h"
#include <QtMessageLogView>
#include <QBoxLayout>
#include <QFormLayout>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTimer>
#include <QRegExp>
#include <QDateTime>
#include <QtPropertyWidget>
#include <QtMessageLogWidget>

QString formatMessage(int level, const QString& text)
{
    QString result;
    switch (level) {
    case 1: // warning
        result += "<p style=\"color:#000000; background-color:#fce94f;\">";
        result += text;
        result += "</p>";
        break;
    case 2: // error
        result += "<p style=\"color:#ffffff; background-color:#0000ff;\">";
        result += text;
        result += "</p>";
        break;
    case 3: // critical
        result += "<p style=\"color:#ffffff; background-color:#ef2929;\">";
        result += text;
        result += "</p>";
        break;
    default:
        result += text;
        break;
    }

    return result;
}

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    timer = new QTimer(this);
    timer->setInterval(10);
    connect(timer, SIGNAL(timeout()), SLOT(createMessage()));

    widget = new QtMessageLogWidget(this);
    widget->setAlternatingRowColors(true);
    widget->setFont(QFont("Ubuntu", 8));
    widget->setHistorySize(32);

    widget->setupStyle(QRegularExpression("Failed"), Qt::red, Qt::white);
    widget->setupStyle(QRegularExpression("error"), Qt::yellow, Qt::darkRed);
    widget->setupStyle(QRegularExpression("warning"), Qt::blue, Qt::white);
    widget->setupStyle(QRegularExpression("sdb"), Qt::white, Qt::blue);
    widget->setupStyle(QRegularExpression("usb"), Qt::white, Qt::darkGreen);


    view = new QtMessageLogView(this);
    view->setRotationLimit(32);
    view->setVisibleFields(QtMessageLogView::Level|QtMessageLogView::Timestamp|
                           QtMessageLogView::Category|QtMessageLogView::Message);



    generateButton = new QPushButton(tr("Generate..."), this);
    generateButton->setCheckable(true);
    generateButton->setChecked(false);
    connect(generateButton, SIGNAL(toggled(bool)), SLOT(toggleTimer(bool)));

    clearButton = new QPushButton(tr("Clear"), this);
    connect(clearButton, SIGNAL(clicked()), view, SLOT(clear()));
    connect(clearButton, SIGNAL(clicked()), widget, SLOT(clear()));


    QtPropertyWidget* editor = new QtPropertyWidget(this);
    editor->setMinimumWidth(378);
    editor->setObject(view);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(generateButton);
    buttonLayout->addWidget(clearButton);
    buttonLayout->addStretch();

    QVBoxLayout* viewLayout = new QVBoxLayout;
    viewLayout->addLayout(buttonLayout);
    viewLayout->addWidget(view);

    QHBoxLayout* layout = new QHBoxLayout;
    layout->addLayout(viewLayout, 1);
    layout->addWidget(editor);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(layout);
    mainLayout->addWidget(widget);

    file.setFileName(":/log.txt");
    if (!file.open(QFile::ReadOnly)) {
        QString text = tr("Failed to open the file '%1': %2").arg(file.fileName()).arg(file.errorString());
        view->message(-1, (int)file.error(), "system", text);
        generateButton->setEnabled(false);
    }
}

Widget::~Widget()
{
}

void Widget::createMessage()
{
    static QRegExp regExp("\\[(.*)\\] (\\d+) (\\w+) (\\d+): (.*)");

    if (file.atEnd()) {
        file.seek(0);
    }

    QString line = file.readLine();
    if (line.isEmpty())
        return;

    if (regExp.indexIn(line) < 0)
        return;

    int level = regExp.cap(2).toInt();
    int code = regExp.cap(4).toInt();
    QString category = regExp.cap(3);
    QString text = regExp.cap(5);
    //view->message(level, code, category, formatMessage(level, text));
    view->message(level, code, category, text);
    widget->message('[' + QDateTime::currentDateTime().toString() + "] " + text);
}

void Widget::toggleTimer(bool on)
{
    on ? timer->start() : timer->stop();
}
