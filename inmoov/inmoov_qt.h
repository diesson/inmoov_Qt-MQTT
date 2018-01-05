#ifndef INMOOV_QT_H
#define INMOOV_QT_H

#include <QMainWindow>

namespace Ui {
class inmoov_qt;
}

class inmoov_qt : public QMainWindow
{
    Q_OBJECT

public:
    explicit inmoov_qt(QWidget *parent = 0);
    ~inmoov_qt();

private:
    Ui::inmoov_qt *ui;
};

#endif // INMOOV_QT_H
