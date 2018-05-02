#ifndef INMOOV_QT_H
#define INMOOV_QT_H
// C/C++
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sstream>
// Qt
#include <QMainWindow>
#include <QLabel>
#include <QToolButton>
#include <QtWidgets/QMessageBox>
// MQTT
#include <QtMqtt/QtMqtt>
#include <QtMqtt/QMqttClient>
#include <QtMqtt/QMqttTopicName>
#include <QMqttClient>
// Delay
#include <QThread>

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
    void on_O_button_add_clicked();

    void on_O_ativar_D_clicked();

    void on_O_ativar_E_clicked();

    void on_O_ativar_C_clicked();

    void on_C_button_executar_toggled(bool checked);

    void on_C_button_cancelar_clicked();

    void updateLogStateChange();

    void on_R_button_conectar_clicked();

    void brokerDisconnected();

    void setClientPort(QString p);

    void on_R_button_executar_clicked();

    void on_R_button_ping_clicked();

    void on_actionQuit_triggered();

    void setDevice(const QByteArray &message, const QMqttTopicName &topic);

private:
    Ui::inmoov_qt *ui;
    QMqttClient *m_client;
};

#endif // INMOOV_QT_H
