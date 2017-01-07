#include <Utility.h>
#include <TimedAction.h>
#include <SPI.h>
#include <Servo.h>
  Servo S;
#define IRQ   4   //interupt request
#define SS    9   //slave select

#define STBY  12  //

#define PWMA  7
#define PWMB  8
#define IN1   31
#define IN2   33
#define IN3   35
#define IN4   37

#define NOS   250
#define NOS_B 100
#define SRV   32
#define sens  A0

  int sudut = 0;
  float sensor, tempC;

void setup(){
  unsigned char u,x;
  delay(500);
  SPI.setBitOrder(MSBFIRST);    //aktifkan mode FIFO
  SPI.setDataMode(SPI_MODE0);   //
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  SPI.begin();
  pinMode(IRQ,INPUT);
  digitalWrite(IRQ, HIGH);
  pinMode(SS,OUTPUT);
  digitalWrite(SS, HIGH);
  pinMode(STBY,OUTPUT);
  digitalWrite(STBY, LOW);    //standby

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(sens, INPUT);
  S.attach(32);
  
  RFM_12_initialize();

  Serial.begin(9600);
}

unsigned int Write_CMD(unsigned int hex){
  unsigned char temp, Ha, eL;

  digitalWrite(SS,LOW);
  delay(1);
  Ha = SPI.transfer(hex / 256);
  eL = SPI.transfer(hex % 256);
  delay(1);
  digitalWrite(SS,HIGH);
  temp = (Ha * 256) + eL;
  return temp;
}

void RFM_12_initialize(void){
  Write_CMD(0x0000);
  Write_CMD(0x80D8);
  Write_CMD(0x82D8);
  Write_CMD(0xA640);
  Write_CMD(0xC657);
  Write_CMD(0x94A0); 
  Write_CMD(0xC2AC);
  Write_CMD(0xCA80);
  Write_CMD(0xCA83);
  Write_CMD(0xC49B);
  Write_CMD(0x9850);
  Write_CMD(0xE000);
  Write_CMD(0xC800);
  Write_CMD(0xC000);
}

unsigned char RFM_12_receive_byte(void){
  unsigned char receive_data;
  
  digitalWrite(SS,LOW);
  delay(1);
  SPI.transfer(0xB0);
  receive_data = SPI.transfer(0);
  delay(1);
  digitalWrite(SS,HIGH);
  return receive_data;
}

void decode(unsigned char dirs){
  if ((dirs&0xF0)==0x30){
    maju();
  }
  else if ((dirs&0xF0)==0xC0){
    mundur();
  }
  else if ((dirs&0xF0)==0x50){
    sudut+=45;
    S.write(sudut);
    delay(30);
  }
  else if ((dirs&0x0F)==0x03){
    kiri();
  }
  else if ((dirs&0x0F)==0x0C){
    kanan();
  }
  else if ((dirs&0x0F)==0x05){
    sudut-=45;
    S.write(sudut);
    delay(30);
  }
  else if (((dirs&0x0F)==0x00)&&((dirs&0xF0)==0x00)){
    off();
  }
}

void jalan(){
  unsigned char move;
  if (digitalRead(IRQ)==LOW){
    move = RFM_12_receive_byte();
    decode(move);
    Write_CMD(0xCA80);
    Write_CMD(0xCA83);
    digitalWrite(STBY, HIGH);    //run
  }
}

void sensors(){
  float mv = analogRead(sens);
  sensor = (mv / 1024) * 5000;
  tempC   = sensor / 10 ;
  Serial.println(tempC);
}

TimedAction ajalan = TimedAction(1,jalan);
TimedAction asensor= TimedAction(1000,sensors);

void loop(){
  ajalan.check();
  asensor.check();
}

void maju(){
  //sensors();
  analogWrite(PWMA, NOS);
  analogWrite(PWMB, NOS);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
void mundur(){
  //sensors();
  analogWrite(PWMA, NOS);
  analogWrite(PWMB, NOS);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void kiri(){
  //sensors();
  analogWrite(PWMA, NOS_B);
  analogWrite(PWMB, NOS_B);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void kanan(){
  //sensors();
  analogWrite(PWMA, NOS_B);
  analogWrite(PWMB, NOS_B);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
void off(){
  //sensors();
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
