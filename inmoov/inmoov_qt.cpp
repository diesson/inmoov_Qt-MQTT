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
    ui->C_info->setReadOnly(true);

    // Configuracao da conexao
    m_client = new QMqttClient(this);
    QString hostname = "192.168.0.15";//
    quint16 port = 1883;
    m_client->setHostname(hostname);
    m_client->setPort(port);

    // Informar o Hostname ao usuario
    ui->C_info->insertPlainText("Hostname: ");
    ui->C_info->insertPlainText(m_client->hostname());
    ui->C_info->insertPlainText("\n");

    // Conectar ao Broker
    connect(m_client, &QMqttClient::stateChanged, this, &inmoov_qt::updateLogStateChange);
    m_client->connectToHost();

}

inmoov_qt::~inmoov_qt(){

    delete ui;

}

void inmoov_qt::updateLogStateChange(){

    if(m_client->state() == QMqttClient::Connected)
        ui->C_info->insertPlainText("Conectado.\n");
    else if(m_client->state() == QMqttClient::Connecting)
        ui->C_info->insertPlainText("Conectando...\n");
    else
        ui->C_info->insertPlainText("Desconectado.\n");

}

void inmoov_qt::on_C_button_executar_toggled(bool checked){

    QString info;
    QByteArray messege;

    if(checked){

        ui->C_info->clear();
        ui->C_info->insertPlainText("Executando acao...\n");
        if(ui->C_opcao_outros->isChecked()){

            messege = ui->C_box_cabeca->currentText().toUtf8();
            info = "Opcao: " + ui->C_box_cabeca->currentText();
            ui->C_info->insertPlainText(info);

            if(ui->C_check_continuo->isChecked()){

                messege = messege + " [continuo]";
                ui->C_info->insertPlainText(" [Continuo]\n");

            }else{

                messege = messege + " [simples]";
                ui->C_info->insertPlainText(" [Simples]\n");

            }

        }else if(ui->C_opcao_esperar->isChecked()){
            info = "Opcao: " + ui->C_opcao_esperar->text().toUtf8() + "\n";
            messege = ui->C_opcao_esperar->text().toUtf8();
            ui->C_info->insertPlainText(info);

        }else if(ui->C_opcao_observar->isChecked()){

            info = "Opcao: " + ui->C_opcao_observar->text().toUtf8() + "\n";
            messege = ui->C_opcao_observar->text().toUtf8();
            ui->C_info->insertPlainText(info);

        }

        QString topic = "robo/cabeca";
        if (m_client->publish(topic, messege) == -1)
            ui->C_info->insertPlainText("Erro ao enviar a mensagem.\n");

    }

}

void inmoov_qt::on_C_button_cancelar_clicked(){

    ui->C_info->insertPlainText("**Execução cancelada.\n");

}

/* SYSTEM */

void inmoov_qt::on_O_button_add_clicked(){

    ui->tabWidget->addTab(new QLabel("Texto"), QString("novo"));

}

void inmoov_qt::on_O_ativar_D_clicked(){

    if(ui->O_ativar_D->isChecked()){
        ui->tabWidget->setTabEnabled(1, true);
    }else{
        ui->tabWidget->setTabEnabled(1, false);
    }

}

void inmoov_qt::on_O_ativar_E_clicked(){

    if(ui->O_ativar_E->isChecked()){
        ui->tabWidget->setTabEnabled(3, true);
    }else{
        ui->tabWidget->setTabEnabled(3, false);
    }

}

void inmoov_qt::on_O_ativar_C_clicked(){

    if(ui->O_ativar_C->isChecked()){
        ui->tabWidget->setTabEnabled(2, true);
    }else{
        ui->tabWidget->setTabEnabled(2, false);
    }

}
