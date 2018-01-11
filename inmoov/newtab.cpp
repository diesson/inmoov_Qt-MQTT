#include "newtab.h"
#include "ui_newtab.h"

newTab::newTab(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::newTab)
{
    ui->setupUi(this);
}

newTab::~newTab()
{
    delete ui;
}
