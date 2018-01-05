#include "inmoov_qt.h"
#include "ui_inmoov_qt.h"

inmoov_qt::inmoov_qt(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::inmoov_qt)
{
    ui->setupUi(this);
    ui->tabWidget->setTabEnabled(1, false);
    ui->tabWidget->setTabEnabled(2, false);
    ui->tabWidget->setTabEnabled(3, false);
    //ui->tabWidget->setTabEnabled(4, false);

}

inmoov_qt::~inmoov_qt()
{
    delete ui;
}

void inmoov_qt::on_Button_add_clicked(){

    ui->tabWidget->addTab(new QLabel("Texto"), QString("novo"));
    //QWidget *newTab = new QWidget(tabWidget);
    //ui->tabWidget->addTab(newTab, tr("name"));

}
/*
    QToolButton *tab_add = new QToolButton();
    tab_add->setText("+");

    ui->tabWidget->addTab(new QLabel("Texto"), QString("novo"));
    ui->tabWidget->setTabEnabled(3, false);
    ui->tabWidget->tabBar()->setTabButton(3, QTabBar::RightSide, tab_add);
*/

void inmoov_qt::on_Button_Ativar_D_clicked(){

    if(ui->Button_Ativar_D->isChecked()){
        ui->tabWidget->setTabEnabled(1, true);
    }else{
        ui->tabWidget->setTabEnabled(1, false);
    }

}

void inmoov_qt::on_Button_Ativar_E_clicked(){

    if(ui->Button_Ativar_E->isChecked()){
        ui->tabWidget->setTabEnabled(2, true);
    }else{
        ui->tabWidget->setTabEnabled(2, false);
    }

}

void inmoov_qt::on_Button_Ativar_C_clicked(){

    if(ui->Button_Ativar_C->isChecked()){
        ui->tabWidget->setTabEnabled(3, true);
    }else{
        ui->tabWidget->setTabEnabled(3, false);
    }

}


void inmoov_qt::on_Button_executar_toggled(bool checked){

    QString info;
    if(checked){

        ui->textEdit_info->setText("Executando acao...");
        info = "Opcao: " + ui->comboBox_cabeca->currentText();
        ui->textEdit_info->append(info);
        if(ui->checkBox_continuo->isChecked()){
            ui->textEdit_info->append("Modo: Continuo.");
                ui->textEdit_info->append("Teste");
        }else{
            ui->textEdit_info->append("Modo: Simples.");
        }

    }

}
