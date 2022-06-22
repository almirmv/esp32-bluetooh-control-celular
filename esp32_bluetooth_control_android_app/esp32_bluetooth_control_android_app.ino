/*
  Nome do Projeto:  esp_32_bluetooth_control_android_app
  Nome do Aquivo:   esp_32_bluetooth_control_android_app.ino                 
  Dependencias:     N/A
  link:             
  Board:            DOIT ESP32 DEVKIT V1, 80MHZ, 921600 / Programador AVRISP mkll
  Compilador        N/A
  IDE:              Arduino IDE 1.8.19
  Hardware:         2 pontes H BTS7960, relé 5v 
  Escrito por:      Almir M Ventura
  Website:          www.omecatronico.com.br
  Modificado por:   N/A 
  Data:             19/06/2022
  Uso:              hobby
  Desenhos          N/A
  Copyright @       N/A
  
  Este programa é software Livre;
  a distribuíção toda ou parcial é permitida com atribuição.
    
  REVISIONS: (latest entry first)
  19/06/2022   -  corrigido bug ponte H direita sem funcionar
  18/06/2022   -  Primeiro release
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Descrição:  

  Simples controle de movimentação e acionamento de rele. Comunicação Bluetooth SSP (serial clássico)
  Software de controle remoto usado no Android foi: Arduino bluetooth controller (Giumig Apps)
  FailSafe: caso passe 2 segundos sem receber comandos de moviementação o robô fica no estado parado.
   
  */
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

//DEFINIÇOES DOS PINOS
#define MOTOR_ESQ_LPWM  23    //PWM motor Esquerdo L
#define MOTOR_ESQ_RPWM  22    //PWM motor Esquerdo R
#define MOTOR_ESQ_LEN   21    //Enable L
#define MOTOR_ESQ_REN   19    //Enable R

#define MOTOR_DIR_LPWM  18    //PWM motor Direito L
#define MOTOR_DIR_RPWM  17    //PWM motor Direito R
#define MOTOR_DIR_LEN   16    //Enable L
#define MOTOR_DIR_REN   4     //Enable R
#define RELE_PIN         2    //rele (também é o LED da placa depedendo do modelo)

#define FAIL_SAFE_TIME  2000  //milisegundos para parar o robo em comandos

//Propriedades do PWM
const int freq = 5000;      //frequencia do PWM
const int resolution = 8;   //8bits PWM

int velocidade = 255;        //velocidae do robo 0 a 255 (em 8bits)
char comando;
boolean confirmRequestPending = true;
unsigned long failSafeTimer = 0;     //

//Instanciando objeto do bluetooth
BluetoothSerial SerialBT;  //Classical Bluetooth (SPP) have the same functionalities of a normal Serial

void setup() {
  Serial.begin(115200);
  pinMode(RELE_PIN,OUTPUT);
  rele(LOW); //desliga rele
  
  inicializaMotores();
  inicializaBluetooth();
}

void loop() {
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    comando = SerialBT.read();
    Serial.println(comando);
    switch(comando){
    case 'a':
      mover(0,velocidade,0,velocidade);//frente
    break;
    case 'b':
      mover(velocidade,0,velocidade,0);//ré
    break;
    case 'c':
      mover(velocidade,0,0,velocidade);//esquerda
    break;
    case 'd':
      mover(0,velocidade,velocidade,0);//direita
    break;
    case 'p':
      parado();//parado
    break;
    case 'l':
      rele(HIGH);//liga o rele
    break;
    case 'm':
      rele(LOW);//desliga o rele
    break;
    }
  }
 //testa se passou muito tempo desde ultimo comando "mover"
 if(millis() - failSafeTimer  > FAIL_SAFE_TIME){
    parado();//parado
   }
 delay(20);
}
