// C/C++
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
// Qt
#include "inmoov_qt.h"
#include <QApplication>
// MQTT
#include <QtMqtt/QtMqtt>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    inmoov_qt w;
    w.show();

    return a.exec();
}
