void inicializaMotores(){
  //configura os canais LED PWM
  ledcSetup(0, freq, resolution);
  ledcSetup(1, freq, resolution);
  ledcSetup(2, freq, resolution);
  ledcSetup(3, freq, resolution);
  //atrela os pinos GPIO aos canais de PWM
  ledcAttachPin(MOTOR_ESQ_LPWM, 0); //canal PWM 0
  ledcAttachPin(MOTOR_ESQ_RPWM, 1); //canal PWM 1
  ledcAttachPin(MOTOR_DIR_LPWM, 2); //canal PWM 2
  ledcAttachPin(MOTOR_DIR_RPWM, 3); //canal PWM 3
  //pinos de enable
  pinMode(MOTOR_ESQ_LEN,OUTPUT);
  pinMode(MOTOR_ESQ_REN,OUTPUT);
  pinMode(MOTOR_DIR_LEN,OUTPUT);
  pinMode(MOTOR_DIR_REN,OUTPUT);

  parado(); //assegurar que inicializa parado
  }

void parado(){
  //PWM off
  ledcWrite(0,0);
  ledcWrite(1,0);
  ledcWrite(2,0);
  ledcWrite(3,0);
  //enables off
  digitalWrite(MOTOR_ESQ_LEN,LOW);
  digitalWrite(MOTOR_ESQ_REN,LOW);
  digitalWrite(MOTOR_DIR_LEN,LOW);
  digitalWrite(MOTOR_DIR_REN,LOW);
  }
  
void mover(int meL, int meR, int mdL, int mdR){
  //PWM
  ledcWrite(0,meL); //canal PWM 0 (MOTOR_ESQ_LPWM)
  ledcWrite(1,meR); //canal PWM 1 (MOTOR_ESQ_RPWM)
  ledcWrite(2,mdL); //canal PWM 2 (MOTOR_DIR_LPWM)
  ledcWrite(3,mdR); //canal PWM 3 (MOTOR_DIR_RPWM)
  //enables
  digitalWrite(MOTOR_ESQ_LEN,HIGH);
  digitalWrite(MOTOR_ESQ_REN,HIGH);
  digitalWrite(MOTOR_DIR_LEN,HIGH);
  digitalWrite(MOTOR_DIR_REN,HIGH);
  failSafeTimer = millis(); //zera timer de fail safe 
  }

void rele(bool estado){
  digitalWrite(RELE_PIN,estado);
  }
