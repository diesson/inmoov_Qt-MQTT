#ifndef INMOOV_QT_H
#define INMOOV_QT_H

#include <QMainWindow>
#include <QLabel>
#include <QToolButton>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

namespace Ui {
class inmoov_qt;
}

class inmoov_qt : public QMainWindow
{
    Q_OBJECT

public:
    explicit inmoov_qt(QWidget *parent = 0);
    ~inmoov_qt();

private slots:
    void on_Button_add_clicked();

    void on_Button_Ativar_D_clicked();

    void on_Button_Ativar_E_clicked();

    void on_Button_Ativar_C_clicked();

    void on_Button_executar_toggled(bool checked);

private:
    Ui::inmoov_qt *ui;
};

#endif // INMOOV_QT_H
