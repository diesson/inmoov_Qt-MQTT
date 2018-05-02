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
        QString info = "robot";
        if (!(m_client->subscribe(info))){
            QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Erro ao criar o topico robot!"));
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

    if((topic == QString("/robot/")) || (topic == QString("/robot")) || (topic == QString("robot/")) || (topic == QString("robot"))){

        char* msg = new char[message.toStdString().length()+1];
        memcpy(msg, message.toStdString().c_str(), message.toStdString().length() + 1);

        std::istringstream is(msg);
        getline(is, recebido, '/');

        if(recebido.compare("") == 0)
            getline(is, recebido, '/');

        if(recebido.compare("robot") == 0){

            getline(is, recebido, '/');

            if(recebido.compare("head") == 0){

                getline(is, recebido, '/');
                if(recebido.compare("currents") == 0){

                    info = QString(message);
                    if(!(m_client->subscribe(info))){
                        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Erro ao criar o topico robot!"));
                        return;
                    }

                    if(getline(is, recebido, '/')){

                        if(recebido.compare("vert") == 0){
                            ui->C_lcd_1->setEnabled(true);
                            ui->C_lcd_1->setStyleSheet("background-color: rgb(185, 255, 191)");
                            ui->C_lcd_1->setPalette(Qt::darkGray);
                            ui->R_info->insertPlainText("Leitura de corrente vertical da cabeca conectado\n");
                        }else if(recebido.compare("horz") == 0){
                            ui->C_lcd_2->setEnabled(true);
                            ui->C_lcd_2->setStyleSheet("background-color: rgb(185, 255, 191)");
                            ui->C_lcd_2->setPalette(Qt::darkGray);
                            ui->R_info->insertPlainText("Leitura de corrente horizontal da cabeca conectado\n");
                        }else{
                            QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Informacoes do servo nao encontradas\nFavor cadastrar o novo servo."));
                        }

                    }

                }else{
                    ui->tabWidget->setTabEnabled(2, true);
                    ui->status_label_C->setStyleSheet("color: rgb(85, 170, 0);");
                    ui->R_info->insertPlainText("Cabeca conectada\n");
                    ui->C_info->insertPlainText("Conectado\n");
                    //retorno = "conectado";
                    //info = "robot/head";
                    //if(m_client->publish(info, retorno) != -1){}
                }

            }else if(recebido.compare("right") == 0){

                getline(is, recebido, '/');
                if(recebido.compare("currents") == 0){

                    info = QString(message);
                    if(!(m_client->subscribe(info))){
                        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Erro ao criar o topico robot!"));
                        return;
                    }

                    if(getline(is, recebido, '/')){

                        if(recebido.compare("vert") == 0){
                            ui->D_lcd_1->setEnabled(true);
                            ui->D_lcd_1->setStyleSheet("background-color: rgb(185, 255, 191)");
                            ui->D_lcd_1->setPalette(Qt::darkGray);
                            ui->R_info->insertPlainText("Leitura de corrente vertical do braco direito conectado\n");
                        }else if(recebido.compare("horz") == 0){
                            ui->D_lcd_2->setEnabled(true);
                            ui->D_lcd_2->setStyleSheet("background-color: rgb(185, 255, 191)");
                            ui->D_lcd_2->setPalette(Qt::darkGray);
                            ui->R_info->insertPlainText("Leitura de corrente horizontal do braco esquerdo conectado\n");
                        }else{
                            QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Informacoes do servo nao encontradas\nFavor cadastrar o novo servo."));
                        }

                    }

                }else{
                    ui->tabWidget->setTabEnabled(1, true);
                    ui->status_label_D->setStyleSheet("color: rgb(85, 170, 0);");
                    ui->R_info->insertPlainText("Braco direito conectado\n");
                    ui->D_info->insertPlainText("Conectado\n");
                    //retorno = "conectado";
                    //info = "robot/right";
                    //if(m_client->publish(info, retorno) != -1){}
                }

            }else if(recebido.compare("left") == 0){

                getline(is, recebido, '/');
                if(recebido.compare("currents") == 0){

                    info = QString(message);
                    if(!(m_client->subscribe(info))){
                        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Erro ao criar o topico robot!"));
                        return;
                    }

                    if(getline(is, recebido, '/')){

                        if(recebido.compare("vert") == 0){
                            ui->E_lcd_1->setEnabled(true);
                            ui->E_lcd_1->setStyleSheet("background-color: rgb(185, 255, 191)");
                            ui->E_lcd_1->setPalette(Qt::darkGray);
                            ui->R_info->insertPlainText("Leitura de corrente vertical do braco esquerdo conectado\n");
                        }else if(recebido.compare("horz") == 0){
                            ui->E_lcd_2->setEnabled(true);
                            ui->E_lcd_2->setStyleSheet("background-color: rgb(185, 255, 191)");
                            ui->E_lcd_2->setPalette(Qt::darkGray);
                            ui->R_info->insertPlainText("Leitura de corrente horizontal do braco esquerdo conectado\n");
                        }else{
                            QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Informacoes do servo nao encontradas\nFavor cadastrar o novo servo."));
                        }

                    }

                }else{
                    ui->tabWidget->setTabEnabled(3, true);
                    ui->status_label_E->setStyleSheet("color: rgb(85, 170, 0);");
                    ui->R_info->insertPlainText("Braco esquerdo conectado\n");
                    ui->E_info->insertPlainText("Conectado\n");
                    //retorno = "conectado";
                    //info = "robot/left";
                    //if(m_client->publish(info, retorno) != -1){}
                }

            }else{

                //QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Mensagem recebida invalida."));
                ui->R_info->insertPlainText("Mensagem recebida invalida.\n");

            }

        }

    }else{

        info = topic.name();

        char* msg = new char[info.toStdString().length()+1];
        memcpy(msg, info.toStdString().c_str(), info.toStdString().length() + 1);

        std::istringstream is(msg);
        getline(is, recebido, '/');

        if(recebido.compare("") == 0)
            getline(is, recebido, '/');

        if(recebido.compare("robot") == 0){

            getline(is, recebido, '/');

            if(recebido.compare("head") == 0){

                getline(is, recebido, '/');
                if(recebido.compare("currents") == 0){

                    getline(is, recebido, '/');
                    recebido_Q = QString::fromStdString(recebido);
                    if(recebido_Q == "vert"){
                        ui->C_lcd_1->display(message.toDouble());
                    }else if(recebido_Q == "horz"){
                        ui->C_lcd_2->display(message.toDouble());
                    }
                }

            }else if(recebido.compare("right") == 0){

                getline(is, recebido, '/');
                if(recebido.compare("currents") == 0){

                    getline(is, recebido, '/');
                    recebido_Q = QString::fromStdString(recebido);
                    if(recebido_Q == "vert"){
                        ui->D_lcd_1->display(message.toDouble());
                    }else if(recebido_Q == "horz"){
                        ui->D_lcd_2->display(message.toDouble());
                    }
                }

            }else if(recebido.compare("left") == 0){

                getline(is, recebido, '/');
                if(recebido.compare("currents") == 0){

                    getline(is, recebido, '/');
                    recebido_Q = QString::fromStdString(recebido);
                    if(recebido_Q == "vert"){
                        ui->E_lcd_1->display(message.toDouble());
                    }else if(recebido_Q == "horz"){
                        ui->E_lcd_2->display(message.toDouble());
                    }
                }

            }else{
                QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Esse servo nao deveria existir. \nFalta cadastrar uma funcao."));
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

        QString topic1 = "robot/head";
        QString topic2 = "robot/right";
        QString topic3 = "robot/left";
        if ((m_client->publish(topic1, message) == -1) || (m_client->publish(topic2, message) == -1) || (m_client->publish(topic3, message) == -1))
            ui->R_info->insertPlainText("Erro ao enviar a mensagem.\n");

    }else{

        ui->R_info->insertPlainText("**Execução cancelada.\n");


        message = "stop";
        QString topic1 = "robot/head";
        QString topic2 = "robot/right";
        QString topic3 = "robot/left";
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
    QString topic;

    ui->C_button_executar->setEnabled(false);
    if(checked){

        ui->C_info->clear();
        info = "Hostname: " + m_client->hostname() + " \t Port: " + QString::number(m_client->port()) + "\n";
        ui->C_info->insertPlainText(info);
        ui->C_info->insertPlainText("Executando acao...\n");
        if(ui->C_opcao_outros->isChecked()){

            if(ui->C_box_cabeca->currentText().toUtf8() == "Movimento - Sim"){

                info = "Opcao: " + ui->C_box_cabeca->currentText();
                ui->C_info->insertPlainText(info);

                int continuo = 0;
                int i = 1;
                if(ui->C_check_continuo->isChecked()){
                    continuo = 1;
                    ui->C_info->insertPlainText(" [Continuo]\n");
                }else
                    ui->C_info->insertPlainText("\n");

                topic = "robot/head/horz";
                message = "50";
                if (m_client->publish(topic, message) == -1){
                    ui->C_info->insertPlainText("Erro ao enviar a mensagem.\n");
                    ui->C_button_executar->setChecked(false);
                    ui->C_button_executar->setEnabled(true);
                }

                while((ui->C_button_executar->isChecked()) && i < 4){

                    topic = "robot/head/vert";
                    message = "100";
                    if (m_client->publish(topic, message) == -1){
                        ui->C_info->insertPlainText("Erro ao enviar a mensagem.\n");
                        ui->C_button_executar->setChecked(false);
                        ui->C_button_executar->setEnabled(true);
                    }


                    topic = "robot/head/vert";
                    message = "0";
                    if (m_client->publish(topic, message) == -1){
                        ui->C_info->insertPlainText("Erro ao enviar a mensagem.\n");
                        ui->C_button_executar->setChecked(false);
                        ui->C_button_executar->setEnabled(true);
                    }

                    if(continuo == 0){
                        i++;
                    }else
                        continuo = 0;

                    if(ui->C_button_executar->isChecked())
                        QThread::msleep(2000);

                }

            }

        }else if(ui->C_opcao_esperar->isChecked()){
            info = "Opcao: " + ui->C_opcao_esperar->text().toUtf8() + "\n";
            ui->C_info->insertPlainText(info);

            topic = "robot/head/vert";
            message = "50";
            if (m_client->publish(topic, message) == -1){
                ui->C_info->insertPlainText("Erro ao enviar a mensagem.\n");
                ui->C_button_executar->setChecked(false);
                ui->C_button_executar->setEnabled(true);
            }

            topic = "robot/head/horz";
            message = "50";
            if (m_client->publish(topic, message) == -1){
                ui->C_info->insertPlainText("Erro ao enviar a mensagem.\n");
                ui->C_button_executar->setChecked(false);
                ui->C_button_executar->setEnabled(true);
            }

        }else if(ui->C_opcao_observar->isChecked()){

            info = "Opcao: " + ui->C_opcao_observar->text().toUtf8() + "\n";
            ui->C_info->insertPlainText(info);


            topic = "robot/head/vert";
            message = "0";
            if (m_client->publish(topic, message) == -1){
                ui->C_info->insertPlainText("Erro ao enviar a mensagem.\n");
                ui->C_button_executar->setChecked(false);
                ui->C_button_executar->setEnabled(true);
            }

            topic = "robot/head/horz";
            message = "50";
            if (m_client->publish(topic, message) == -1){
                ui->C_info->insertPlainText("Erro ao enviar a mensagem.\n");
                ui->C_button_executar->setChecked(false);
                ui->C_button_executar->setEnabled(true);
            }

        }

    }

}

void inmoov_qt::on_C_button_cancelar_clicked(){

    ui->C_info->insertPlainText("**Execução cancelada.\n");

    //QByteArray message = "stop";
    //QString topic = "robot/head/vert";
    //if (m_client->publish(topic, message) == -1){
    //    ui->C_info->insertPlainText("Erro ao parar execução.\n");
    //    ui->C_button_executar->setChecked(true);
    //}else{
        ui->C_button_executar->setEnabled(true);
    //}

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
