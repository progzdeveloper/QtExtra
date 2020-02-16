#include <cstdio>
#include <QTextStream>
#include "controller.h"

int main()
{
    DomainController controller;
    QString command;
    QTextStream input(stdin);
    for (;;) {
        if (input.atEnd())
            break;
        input >> command;
        if (command.isEmpty())
            continue;
        controller.invoke(command);
    }
    return 0;
}
