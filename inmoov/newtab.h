#ifndef NEWTAB_H
#define NEWTAB_H

#include <QDialog>

namespace Ui {
class newTab;
}

class newTab : public QDialog
{
    Q_OBJECT

public:
    explicit newTab(QWidget *parent = 0);
    ~newTab();

private:
    Ui::newTab *ui;
};

#endif // NEWTAB_H
