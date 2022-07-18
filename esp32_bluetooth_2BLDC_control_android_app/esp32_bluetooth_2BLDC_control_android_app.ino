/*
  Nome do Projeto:  esp_32_bluetooth_2BLDC_control_android_app
  Nome do Aquivo:   esp_32_bluetooth_2BLDC_control_android_app.ino                 
  Dependencias:     N/A
  link:             
  Board:            DOIT ESP32 DEVKIT V1, 80MHZ, 921600 / Programador AVRISP mkll
  Compilador        N/A
  IDE:              Arduino IDE 1.8.19
  Hardware:         3 pontes H BTS7960
  Escrito por:      Almir M Ventura
  Website:          www.omecatronico.com.br
  Modificado por:   N/A 
  Data:             17/07/2022
  Uso:              hobby
  Desenhos          N/A
  Copyright @       N/A
  
  Este programa é software Livre;
  a distribuíção toda ou parcial é permitida com atribuição.
    
  REVISIONS: (latest entry first)
  18/07/2022   -  corrigido bug ponte H direita sem funcionar
  17/07/2022   -  Primeiro release
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Descrição:  

  Controle de movimentação usando 2 motores BLDC de hoverboard. Comunicação Bluetooth SSP (serial clássico)
  Software de controle remoto usado no Android foi: Arduino bluetooth controller (Giumig Apps)
  FailSafe: caso passe 2 segundos sem receber comandos de moviementação o robô fica no estado parado.
   
  */
#include "BluetoothSerial.h"
#include <SimpleFOC.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

//DEFINIÇOES DOS PINOS--------------------------------------
#define M1_HALL_A_PIN   36    //HALL A do motor 1
#define M1_HALL_B_PIN   39    //HALL B do motor 1
#define M1_HALL_C_PIN   34    //HALL C do motor 1

#define M2_HALL_A_PIN   35    //HALL A do motor 2
#define M2_HALL_B_PIN   32    //HALL B do motor 2
#define M2_HALL_C_PIN   33    //HALL C do motor 2

#define M1_PWM_A_PIN    18    //PWM A do motor 1
#define M1_PWM_B_PIN     5    //PWM B do motor 1
#define M1_PWM_C_PIN    17    //PWM C do motor 1
#define M1_ENABLE_PIN   16    //ENABLE de PWM do motor 1

#define M2_PWM_A_PIN     4    //PWM A do motor 2
#define M2_PWM_B_PIN     0    //PWM B do motor 2
#define M2_PWM_C_PIN     2    //PWM C do motor 2
#define M2_ENABLE_PIN   15    //ENABLE de PWM do motor 2

#define RELE_PIN        19    //rele (também é o LED da placa depedendo do modelo)

//CONSTANTES-------------------------------------------------
#define M1_POLES_PAIRS    15    //15 pole pairs (30 magnets) hoverboard Motor (27N30P)
#define M2_POLES_PAIRS    15    //30 magnets hoverboard Motor

#define POWER_SUPPLY      24    //tensão de alimentação dos motores [V]
#define VOLTAGE_LIMIT     12    //tensão maxima que o motor pode receber [V] - (default = power_supply)
#define PWM_FREQUENCY  20000    //pwm frequency to be used [Hz]
#define FAIL_SAFE_TIME  2000    //milisegundos para parar o robo em comandos


int velocidade = 2;                     //velocidade radianos/segundo
char comando;
boolean confirmRequestPending = true;
unsigned long failSafeTimer = 0;        //usado para calcular tempo do failsafe


//Instanciando objetos ---------------------------------------
BluetoothSerial SerialBT;               //Classical Bluetooth (SPP) have the same functionalities of a normal Serial

HallSensor sensorM1 = HallSensor(M1_HALL_A_PIN, M1_HALL_B_PIN, M1_HALL_C_PIN, M1_POLES_PAIRS);       //(pinA,pinB,pinC, pole pairs)
BLDCDriver3PWM driverM1 = BLDCDriver3PWM(M1_PWM_A_PIN, M1_PWM_B_PIN, M1_PWM_C_PIN, M1_ENABLE_PIN);   //BLDCDriver3PWM( pin_pwmA, pin_pwmB, pin_pwmC, enable (optional))
BLDCMotor motorM1 = BLDCMotor(M1_POLES_PAIRS, 0.17);                                                 //BLDCMotor( pole_pairs , ( phase_resistance, KV_rating  optional) )

HallSensor sensorM2 = HallSensor(M2_HALL_A_PIN, M2_HALL_B_PIN, M2_HALL_C_PIN, M2_POLES_PAIRS);       //(pinA,pinB,pinC, pole pairs)
BLDCDriver3PWM driverM2 = BLDCDriver3PWM(M2_PWM_A_PIN, M2_PWM_B_PIN, M2_PWM_C_PIN, M2_ENABLE_PIN);   //BLDCDriver3PWM( pin_pwmA, pin_pwmB, pin_pwmC, enable (optional))
BLDCMotor motorM2 = BLDCMotor(M2_POLES_PAIRS, 0.17);                                                 //BLDCMotor( pole_pairs , ( phase_resistance, KV_rating  optional) )

//Callback Interrupçoes---------------------------------------
void doA_M1(){sensorM1.handleA();}
void doB_M1(){sensorM1.handleB();}
void doC_M1(){sensorM1.handleC();}

void doA_M2(){sensorM2.handleA();}
void doB_M2(){sensorM2.handleB();}
void doC_M2(){sensorM2.handleC();}

void setup() {
  Serial.begin(115200);
  pinMode(RELE_PIN,OUTPUT);
  rele(LOW); //desliga rele
  
  inicializaMotores();
  inicializaBluetooth();
}

void loop() {
  motorM1.loopFOC();                //FOC algorithm execution - should be executed as fast as possible > 1Khz
  motorM1.monitor();
  //motorM1.move(2);                // setting the target velocity or 2rad/s
  
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    comando = SerialBT.read();
    Serial.println(comando);
    switch(comando){
    case 'a':
      mover(velocidade,velocidade);//frente
    break;
    case 'b':
      mover(-velocidade,-velocidade);//ré
    break;
    case 'c':
      mover(-velocidade,velocidade);//esquerda
    break;
    case 'd':
      mover(velocidade,-velocidade);//direita
    break;
    case 'p':
      mover(0,0);//parado
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
   mover(0,0);//parado
   }
 //delay(20);
}
