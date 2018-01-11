#include "inmoov_qt.h"
#include "ui_inmoov_qt.h"
#include "newtab.h"

inmoov_qt::inmoov_qt(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::inmoov_qt)
{
    ui->setupUi(this);

    ui->tabWidget->setTabEnabled(1, false);
    ui->tabWidget->setTabEnabled(2, false);
    ui->tabWidget->setTabEnabled(3, false);
    ui->tabWidget->setTabEnabled(4, true);

    // Terminais de informacao para o usuario
    ui->C_info->setReadOnly(true);
    ui->R_info->setReadOnly(true);
    ui->D_info->setReadOnly(true);
    ui->E_info->setReadOnly(true);

    // Configuracao da conexao
    m_client = new QMqttClient(this);
    m_client->setHostname(ui->R_edit_host->text());
    m_client->setPort(ui->R_edit_port->text().toUInt());

    // Conectar ao Broker
    connect(m_client, &QMqttClient::stateChanged, this, &inmoov_qt::updateLogStateChange);
    connect(m_client, &QMqttClient::disconnected, this, &inmoov_qt::brokerDisconnected);

    // Atualizar Hostname e Port
    connect(ui->R_edit_host, &QLineEdit::textChanged, m_client, &QMqttClient::setHostname);
    connect(ui->R_edit_port, &QLineEdit::textChanged, this, &inmoov_qt::setClientPort);

    // Configura resposta do botao 'Ping'
    connect(m_client, &QMqttClient::pingResponseReceived, this, [this]() {
        ui->R_button_ping->setEnabled(true);
        ui->R_info->insertPlainText("PING: Retornado\n");
    });

    // Aguardar conexoes
    connect(m_client, &QMqttClient::messageReceived, this, &inmoov_qt::setDevice);

    updateLogStateChange();

}

inmoov_qt::~inmoov_qt(){

    delete ui;

}

/* ROBO */

void inmoov_qt::on_R_button_conectar_clicked(){

    QString info;

    if((ui->R_edit_host->text() != "") & (ui->R_edit_port->text() != "")){

        if(m_client->state() == QMqttClient::Disconnected){

            // Alterar interface
            ui->R_edit_host->setEnabled(false);
            ui->R_edit_port->setEnabled(false);
            ui->R_button_conectar->setText(tr("Desconectar"));
            ui->R_button_executar->setEnabled(true);
            ui->R_button_ping->setEnabled(true);

            // Conectar
            m_client->connectToHost();

            // Informacoes de log
            ui->R_info->clear();
            info = "Hostname: " + m_client->hostname() + " \nPort: " + QString::number(m_client->port()) + "\n";
            ui->R_info->insertPlainText(info);

        }else{

            ui->R_edit_host->setEnabled(true);
            ui->R_edit_port->setEnabled(true);
            ui->R_button_conectar->setText(tr("Conectar"));
            ui->R_button_executar->setEnabled(false);
            if(ui->R_button_executar->text() == "Parar")
                on_R_button_executar_clicked();
            m_client->disconnectFromHost();

        }

    }

}

void inmoov_qt::brokerDisconnected(){

    ui->R_edit_host->setEnabled(true);
    ui->R_edit_port->setEnabled(true);
    ui->R_button_conectar->setText(tr("Conectar"));

}

void inmoov_qt::updateLogStateChange(){

    if(m_client->state() == QMqttClient::Connected){

        ui->R_info->insertPlainText("Conectado\n");

        // Inscrição no topico
        QString info = "robo";
        if (!(m_client->subscribe(info))){
            QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Erro ao criar o topico robo!"));
            return;
        }

    }else if(m_client->state() == QMqttClient::Connecting)
        ui->R_info->insertPlainText("Conectando...\n");
    else
        ui->R_info->insertPlainText("Desconectado\n");

}

void inmoov_qt::setClientPort(QString p){

    m_client->setPort(p.toUInt());

}

void inmoov_qt::setDevice(const QByteArray &message, const QMqttTopicName &topic){

    QString info;
    QString recebido_Q;
    QByteArray retorno;
    std::string recebido;

    info = "robo";
    if(topic == info){


        char* msg = new char[message.toStdString().length()+1];
        memcpy(msg, message.toStdString().c_str(), message.toStdString().length() + 1);

        std::istringstream is(msg);
        getline(is, recebido, '/');

        if(recebido.compare("robo") == 0){

            getline(is, recebido, '/');

            if(recebido.compare("cabeca") == 0){

                retorno = "conectado";
                info = "robo/cabeca";
                if(m_client->publish(info, retorno) != -1){
                    ui->tabWidget->setTabEnabled(2, true);
                    ui->status_label_C->setStyleSheet("color: rgb(85, 170, 0);");
                }

            }else if(recebido.compare("direita") == 0){

                retorno = "conectado";
                info = "robo/direita";
                if(m_client->publish(info, retorno) != -1){
                    ui->tabWidget->setTabEnabled(1, true);
                    ui->status_label_D->setStyleSheet("color: rgb(85, 170, 0);");
                }

            }else if(recebido.compare("esquerda") == 0){

                retorno = "conectado";
                info = "robo/esquerda";
                if(m_client->publish(info, retorno) != -1){
                    ui->tabWidget->setTabEnabled(3, true);
                    ui->status_label_E->setStyleSheet("color: rgb(85, 170, 0);");
                }

            }else{

                recebido_Q = QString::fromStdString(recebido);
                newTab *myNewTab = new newTab();

                QFrame* caixa = new QFrame(myNewTab);
                QRadioButton* Opcao_1 = new QRadioButton(caixa);
                Opcao_1->setText(QString(recebido_Q));

                ui->tabWidget->addTab(myNewTab, QString(recebido_Q));
                if(getline(is, recebido, '/')){

                    recebido_Q = QString::fromStdString(recebido);
                    QRadioButton* opcao = new QRadioButton(caixa);
                    opcao->setText(QString(recebido_Q));

                }

            }

        }
    }

}

void inmoov_qt::on_R_button_executar_clicked(){

    QString info;
    QByteArray message;

    if(ui->R_button_executar->text() == "Executar"){

        ui->R_button_executar->setText(tr("Parar"));

        ui->R_info->clear();
        info = "Hostname: " + m_client->hostname() + " \t Port: " + QString::number(m_client->port()) + "\n";
        ui->R_info->insertPlainText(info);
        ui->R_info->insertPlainText("Executando acao...\n");

        message = ui->R_box_comandos->currentText().toUtf8();
        info = "Opcao: " + ui->R_box_comandos->currentText() + "\n";
        ui->R_info->insertPlainText(info);

        QString topic1 = "robo/cabeca";
        QString topic2 = "robo/direita";
        QString topic3 = "robo/esquerda";
        if ((m_client->publish(topic1, message) == -1) || (m_client->publish(topic2, message) == -1) || (m_client->publish(topic3, message) == -1))
            ui->R_info->insertPlainText("Erro ao enviar a mensagem.\n");

    }else{

        ui->R_info->insertPlainText("**Execução cancelada.\n");


        message = "stop";
        QString topic1 = "robo/cabeca";
        QString topic2 = "robo/direita";
        QString topic3 = "robo/esquerda";
        if ((m_client->publish(topic1, message) == -1) || (m_client->publish(topic2, message) == -1) || (m_client->publish(topic3, message) == -1))
            ui->R_info->insertPlainText("Erro ao parar execução.\n");
        else
            ui->R_button_executar->setText(tr("Executar"));

    }

}

void inmoov_qt::on_R_button_ping_clicked(){

    ui->R_button_ping->setEnabled(false);
    m_client->requestPing();
    ui->R_info->insertPlainText("PING: Enviado\n");

}

/* CABECA */

void inmoov_qt::on_C_button_executar_toggled(bool checked){

    QString info;
    QByteArray message;

    ui->C_button_executar->setEnabled(false);
    if(checked){

        ui->C_info->clear();
        info = "Hostname: " + m_client->hostname() + " \t Port: " + QString::number(m_client->port()) + "\n";
        ui->C_info->insertPlainText(info);
        ui->C_info->insertPlainText("Executando acao...\n");
        if(ui->C_opcao_outros->isChecked()){

            message = ui->C_box_cabeca->currentText().toUtf8();
            info = "Opcao: " + ui->C_box_cabeca->currentText();
            ui->C_info->insertPlainText(info);

            if(ui->C_check_continuo->isChecked()){

                message = message + " [continuo]";
                ui->C_info->insertPlainText(" [Continuo]\n");

            }else{

                message = message + " [simples]";
                ui->C_info->insertPlainText(" [Simples]\n");

            }

        }else if(ui->C_opcao_esperar->isChecked()){
            info = "Opcao: " + ui->C_opcao_esperar->text().toUtf8() + "\n";
            message = ui->C_opcao_esperar->text().toUtf8();
            ui->C_info->insertPlainText(info);

        }else if(ui->C_opcao_observar->isChecked()){

            info = "Opcao: " + ui->C_opcao_observar->text().toUtf8() + "\n";
            message = ui->C_opcao_observar->text().toUtf8();
            ui->C_info->insertPlainText(info);

        }

        QString topic = "robo/cabeca";
        if (m_client->publish(topic, message) == -1){
            ui->C_info->insertPlainText("Erro ao enviar a mensagem.\n");
            ui->C_button_executar->setChecked(false);
            ui->C_button_executar->setEnabled(true);
        }

    }

}

void inmoov_qt::on_C_button_cancelar_clicked(){

    ui->C_info->insertPlainText("**Execução cancelada.\n");

    QByteArray message = "stop";
    QString topic = "robo/cabeca";
    if (m_client->publish(topic, message) == -1){
        ui->C_info->insertPlainText("Erro ao parar execução.\n");
        ui->C_button_executar->setChecked(true);
    }else{
        ui->C_button_executar->setEnabled(true);
    }

}

/* SYSTEM */

void inmoov_qt::on_O_ativar_D_clicked(){

    if(ui->O_ativar_D->isChecked()){
        ui->tabWidget->setTabEnabled(1, true);
        ui->status_label_D->setStyleSheet("color: rgb(85, 170, 0);");
    }else{
        ui->tabWidget->setTabEnabled(1, false);
        ui->status_label_D->setStyleSheet("color: rgb(207, 0, 0);");
    }

}

void inmoov_qt::on_O_ativar_E_clicked(){

    if(ui->O_ativar_E->isChecked()){
        ui->tabWidget->setTabEnabled(3, true);
        ui->status_label_E->setStyleSheet("color: rgb(85, 170, 0);");
    }else{
        ui->tabWidget->setTabEnabled(3, false);
        ui->status_label_E->setStyleSheet("color: rgb(207, 0, 0);");
    }

}

void inmoov_qt::on_O_ativar_C_clicked(){

    if(ui->O_ativar_C->isChecked()){
        ui->tabWidget->setTabEnabled(2, true);
        ui->status_label_C->setStyleSheet("color: rgb(85, 170, 0);");
    }else{
        ui->tabWidget->setTabEnabled(2, false);
        ui->status_label_C->setStyleSheet("color: rgb(207, 0, 0);");
    }

}

void inmoov_qt::on_O_button_add_clicked(){

    ui->tabWidget->addTab(new QLabel("Texto"), QString("novo"));

}

void inmoov_qt::on_actionQuit_triggered(){

    QApplication::quit();
}
