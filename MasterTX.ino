#include <SPI.h>

#define IRQ 4 //interrupts request output (active low)
#define SS  9 //select slave

#define B_MAJU    2
#define B_MUNDUR  3
#define B_KIRI    5
#define B_KANAN   6
#define B_SERV_L  7
#define B_SERV_R  8

unsigned int Write_CMD(unsigned int hex){
  unsigned char value, Ha, eL;
  digitalWrite(SS, LOW);
  delay(1);
  Ha = SPI.transfer(hex / 256);
  eL = SPI.transfer(hex % 256);
  delay(1);
  digitalWrite(SS, HIGH);
  value = (Ha * 256) + eL;
  return value;
}
void RFM_12_TX_initialize(){
  Write_CMD(0x0000);
  Write_CMD(0x80D8);//enable register,433MHz,12.5pF
  Write_CMD(0x8208);//Turn on crystal,!PA
  Write_CMD(0xA640);// 434MHz
  Write_CMD(0xC657);// 4.8kbps
  Write_CMD(0x94A0);//VDI,FAST,134kHz,0dBm,-103dBm
  Write_CMD(0xC2AC);
  Write_CMD(0xCA80);
  Write_CMD(0xCA83);//FIFO8,SYNC,
  Write_CMD(0xC49B);
  Write_CMD(0x9850);//!mp,9810=30kHz,MAX OUT
  Write_CMD(0xE000);//NOT USE
  Write_CMD(0xC80E);//NOT USE
  Write_CMD(0xC000);//1.0MHz,2.2V
}
void RFM_12_TX_open(){
  Write_CMD(0xB8AA);
  Write_CMD(0xB8AA);
  Write_CMD(0xB8AA);
  Write_CMD(0x8228);
  Write_CMD(0x8238);
  while(digitalRead(IRQ));
  Write_CMD(0xB82D);
  Write_CMD(0xB8D4);
}
void RFM_12_TX_sending(unsigned char data){
  while(digitalRead(IRQ));
  Write_CMD(0xB800 + data);
}
void RFM_12_TX_close(){
  Write_CMD(0xB8AA);
  Write_CMD(0x8208);
  Write_CMD(0x8200);
}
void setup(){
  delay(500); //wait 500ms
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  SPI.begin();
  pinMode(IRQ, INPUT);
  digitalWrite(IRQ, HIGH); //aktifkan pullup resistor
  pinMode(SS, OUTPUT);
  digitalWrite(SS, HIGH);
  //PERENJEKAN
  pinMode(B_MAJU,INPUT);
  digitalWrite(B_MAJU,HIGH);
  pinMode(B_MUNDUR,INPUT);
  digitalWrite(B_MUNDUR,HIGH);
  pinMode(B_KANAN,INPUT);
  digitalWrite(B_KANAN,HIGH);
  pinMode(B_KIRI,INPUT);
  digitalWrite(B_KIRI,HIGH);
  pinMode(B_SERV_L,INPUT);
  digitalWrite(B_SERV_L,HIGH);
  pinMode(B_SERV_R,INPUT);
  digitalWrite(B_SERV_R,HIGH);
  
  RFM_12_TX_initialize();
  Serial.begin(9600);
}
void loop(){
  unsigned char move, premove;
  while(1) {
    if (digitalRead(B_MAJU)==LOW){
        move=move&0x0F;
        move=move|0x30;
        Serial.println(move);
    }
    else if (digitalRead(B_MUNDUR)==LOW){
        move=move&0x0F;
        move=move|0xC0;
        Serial.println(move);
    }
    else if (digitalRead(B_SERV_L)==LOW){
        move=move&0x0F;
        move=move|0x50;
        Serial.println(move);
    }
    else if ((digitalRead(B_MAJU)==HIGH)&&(digitalRead(B_MUNDUR)==HIGH)/*&&(digitalRead(B_SERV_L)==HIGH)*/){
        move=move&0x0F;
        move=move|0x00;
        Serial.println(move);
    }
    
    if (digitalRead(B_KIRI)==LOW){
        move=move&0xF0;
        move=move|0x03;
        Serial.println(move);
    }
    else if (digitalRead(B_KANAN)==LOW){
        move=move&0xF0;
        move=move|0x0C;
        Serial.println(move);
    }
    else if (digitalRead(B_SERV_R)==LOW){
        move=move&0xF0;
        move=move|0x05;
        Serial.println(move);
    }
    else if ((digitalRead(B_KIRI)==HIGH)&&(digitalRead(B_KANAN)==HIGH)/*&&(digitalRead(B_SERV_R)==HIGH)*/){
        move=move&0xF0;
        move=move|0x00;
        Serial.println(move);
    }

    if(premove != move){
      RFM_12_TX_open();
      RFM_12_TX_sending(move);
      RFM_12_TX_close();
      premove = move;
    }
    delay(100);
  }
}
