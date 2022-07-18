void inicializaBluetooth(){
  SerialBT.enableSSP();
  SerialBT.onConfirmRequest(BTConfirmRequestCallback);
  SerialBT.onAuthComplete(BTAuthCompleteCallback);
  SerialBT.begin("Droid 1"); //Bluetooth device name
  Serial.println("[INFO] Bluetooh pronto para parear");
  }


void BTConfirmRequestCallback(uint32_t numVal){
  confirmRequestPending = true;
  Serial.println(numVal); //envia senha de pareamento pelo serial
  
  //na linha abaixo ta confimando senha automaticamente.
  //se tiver LCD no robo pode mostrar o "numVal" e somente depois executar a linha abaixo.
  SerialBT.confirmReply(true);  //aceita conexao 
}

void BTAuthCompleteCallback(boolean success){
  confirmRequestPending = false;
  if (success){
    Serial.println("[INFO] Pareamento ok!");
  }
  else{
    Serial.println("[ERRO] Pareamento rejeitado pelo usuario!");
  }
}
