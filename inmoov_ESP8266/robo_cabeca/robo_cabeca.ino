#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <stdio.h>
#include <string.h>

#define TOPICO_SUBSCRIBE "robo/cabeca"
#define TOPICO_PUBLISH   "robo"
#define ID_MQTT  "135130"

//MAPEAMENTO
#define D0    16
#define D1    5
#define D2    4
#define D3    0
#define D4    2
#define D5    14
#define D6    12
#define D7    13
#define D8    15
#define D9    3
#define D10   1
#define A0    17

// CONSTANTES
const char* SSID = "";                 //WiFi
const char* PASSWORD = "";          //WiFi
const char* BROKER_MQTT = "192.168.0.15";  //MQTT
int BROKER_PORT = 1883;                     //MQTT


// VARIAVEIS
char estado_saida = '0';
int conectado = 0;

// SEI LA
WiFiClient espClient;             // Cria o objeto espClient
PubSubClient MQTT(espClient);     // Instancia o Cliente MQTT passando o objeto espClient

 
// FUNÇÕES
void initSerial();
void initWiFi();
void initMQTT();
void reconectWiFi(); 
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void VerificaConexoesWiFIEMQTT(void);
void InitOutput(void);

void setup(){
  
    //inicializações:
    pinMode(A0, INPUT);
    InitOutput();
    initSerial();
    initWiFi();
    initMQTT();
    
}

void initSerial(){ // Inicializa comunicação serial com baudrate 115200 (monitorar no terminal serial)

    Serial.begin(115200);

}

void InitOutput(void){ // Inicializa o output em nível lógico baixo

    pinMode(D0, OUTPUT);
    digitalWrite(D0, HIGH); 
             
}

void initWiFi(){ // Inicializa e conecta-se na rede WI-FI desejada
  
    delay(10);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    Serial.println("Aguarde");
    
    reconectWiFi();
    
}

void initMQTT(){ // Inicializa parâmetros de conexão MQTT
  
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);   //Set Broker e Porta
    MQTT.setCallback(mqtt_callback);            //atribui função de callback
    
}

void reconnectMQTT(){ // Reconecta-se ao broker MQTT 
  
    while (!MQTT.connected()){
      
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        
        if ( MQTT.connect(ID_MQTT) ){
          
            Serial.println("Conectado com sucesso ao broker MQTT!");
            MQTT.subscribe(TOPICO_SUBSCRIBE); 
            
        }else{
          
            Serial.println("Falha ao reconectar no broker.");
            Serial.println("Havera nova tentatica de conexao em 2s\n");
            delay(2000);
            
        }
        
    }
    
}

void reconectWiFi() // Reconecta-se ao WiFi
{
    //se já está conectado a rede WI-FI, nada é feito. 
    //Caso contrário, são efetuadas tentativas de conexão
    if (WiFi.status() == WL_CONNECTED)
        return;
        
    WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI
    
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(100);
        Serial.print(".");
    }
  
    Serial.println();
    Serial.print("Conectado com sucesso na rede ");
    Serial.print(SSID);
    Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP());
    
}

void VerificaConexoesWiFIEMQTT(void){ // Verifica o estado das conexões WiFI e ao broker MQTT. 
  
    if ( !MQTT.connected() ) 
        reconnectMQTT(); //se não há conexão com o Broker, a conexão é refeita
    
     reconectWiFi(); //se não há conexão com o WiFI, a conexão é refeita
     
}

void mqtt_callback(char* topic, byte* payload, unsigned int length){ // Função de callback | Chamada toda vez que uma informação chegar

    char* mensagem;

    payload[length] = '\0';
    mensagem = (char*)payload;
    
    //MQTT.publish( TOPICO_PUBLISH, "retorno" );

    if((String)mensagem == "stop")
        digitalWrite(D0, HIGH);
    else if((String)mensagem == "conectado")
        conectado = 1;
    else{
    
        digitalWrite(D0, LOW);
    
    }
        
    Serial.println(mensagem);
    
}

void loop(){ // Programa principal
  
    VerificaConexoesWiFIEMQTT();

    if(conectado == 0)
        MQTT.publish(TOPICO_PUBLISH, "robo/cabeca");

    MQTT.loop();
    delay(1000);
}


