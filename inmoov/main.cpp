#include "inmoov_qt.h"
#include <QApplication>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    inmoov_qt w;
    w.show();

    return a.exec();
}
