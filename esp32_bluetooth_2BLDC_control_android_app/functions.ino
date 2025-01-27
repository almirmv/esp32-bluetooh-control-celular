void inicializaMotores(){
  
  //MOTOR 1-----------------------------------------------------------------------  
  sensorM1.init();                                    // initialize sensor hardware
  sensorM1.enableInterrupts(doA_M1, doB_M1, doC_M1);  // hardware interrupt enable
  motorM1.linkSensor(&sensorM1);                      // link the motor to the sensor
  
  driverM1.pwm_frequency = PWM_FREQUENCY;             // pwm frequency to be used [Hz]
  driverM1.voltage_power_supply = POWER_SUPPLY;       // power supply voltage [V]
  driverM1.voltage_limit = VOLTAGE_LIMIT;             // Max DC voltage allowed - default voltage_power_supply
  driverM1.init();                                    // driver init
  motorM1.linkDriver(&driverM1);                      // link the motor to the driver

  //motorM1.useMonitoring(Serial);                      //para monitorar a inicialização do motor 1
  
  motorM1.controller = MotionControlType::velocity;   // set control loop type to be used
  motorM1.init();                                     // initialize motor
  motorM1.initFOC();                                  // align encoder and start FOC

  //MOTOR 2-------------------------------------------------------------------------
  sensorM2.init();                                    // initialize sensor hardware
  sensorM2.enableInterrupts(doA_M2, doB_M2, doC_M2);  // hardware interrupt enable
  motorM2.linkSensor(&sensorM2);                      // link the motor to the sensor
  
  driverM2.pwm_frequency = PWM_FREQUENCY;             // pwm frequency to be used [Hz]
  driverM2.voltage_power_supply = POWER_SUPPLY;       // power supply voltage [V]
  driverM2.voltage_limit = VOLTAGE_LIMIT;             // Max DC voltage allowed - default voltage_power_supply
  driverM2.init();                                    // driver init
  motorM2.linkDriver(&driverM2);                      // link the motor to the driver
  //motorM2.useMonitoring(Serial);                      //para monitorar a inicialização do motor 1

  //motorM2.foc_modulation = FOCModulationType::SpaceVectorPWM;// choose FOC modulation
  // set motion control loop to be used
  //motorM2.torque_controller = TorqueControlType::voltage; // voltage torque control mode
  //motorM2.controller = MotionControlType::torque;// set control loop type to be used
  motorM2.controller = MotionControlType::velocity;   // set control loop type to be used
 

  // controller configuration based on the control type 
  // velocity PID controller parameters
  // default P=0.5 I = 10 D =0
  motorM2.PID_velocity.P = 0.2;
  motorM2.PID_velocity.I = 20.0;
  motorM2.PID_velocity.D = 0;
  // jerk control using voltage voltage ramp
  // default value is 300 volts per sec  ~ 0.3V per millisecond
  motorM2.PID_velocity.output_ramp = 300;

  // velocity low pass filtering
  // default 5ms - try different values to see what is the best. 
  // the lower the less filtered
  motorM2.LPF_velocity.Tf = 0.02;
  motorM2.P_angle.P = 20;                 // angle loop controller
  motorM2.velocity_limit = 50;            // angle loop velocity limit


  
  motorM2.init();                       // initialize motor
  motorM2.initFOC();                    // align encoder and start FOC
  motorM2.target = 2; // set the inital target value
  
  //mover(0,0);                       //assegurar que inicializa parado
  }
  
void mover(float velM1, float velM2){
  Serial.print(velM1);
  Serial.print(" / ");
  Serial.print(velM2);
  
  motorM1.move(velM1);            // setting the target velocity rad/s
  motorM2.move(velM2);            // setting the target velocity rad/s
  
  failSafeTimer = millis(); //zera timer de fail safe 
  }

void rele(bool estado){
  digitalWrite(RELE_PIN,estado);
  }
