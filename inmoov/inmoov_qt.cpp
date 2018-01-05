#include "inmoov_qt.h"
#include "ui_inmoov_qt.h"

inmoov_qt::inmoov_qt(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::inmoov_qt)
{
    ui->setupUi(this);
}

inmoov_qt::~inmoov_qt()
{
    delete ui;
}
