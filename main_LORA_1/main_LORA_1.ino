#include "jpeg1.h"
#include "jpeg2.h"
#include "jpeg3.h"

int Distance = 0;
char choice;

#include <EasyScheduler.h>//ฟังก์ชั่น loop
//Scheduler
Schedular Task1; // สร้างงาน Task
Schedular Task2;
Schedular Task3;
Schedular Task4;
Schedular Task5;
Schedular Task6;
#define time_A 300 //กำหนดการทำงานใน Tasks

#include "config.h"//รวมฟังก์ชั่น
int aa = 0;
/******************************************* *setup****************************************************/
void setup() {

  Serial.begin(115200);

  tft.init();         //สั่งให้หน้าจอเริ่มทำงาน
  tft.setRotation(1); // หมุนหน้าจอ
  //ฟังก์ชั่นรูป
  tft.setSwapBytes(true);
  tft.pushImage(0, 0, 480, 343, welcome1);
  delay(3000);
  TFT_AA();           //ส่วนของตัวเลือกระยะกับแสดงเชื่อมต่อไวไฟ
  tft.pushImage(0, 0, 480, 343, test_2);


  Task1.start();      // สั่งให้งาน Task เริ่มทำงาน
  Task2.start();
  Task3.start();
  Task4.start();
  Task5.start();
  Task6.start();

  startLoRA(); //LORA

  //setupวัดระยะ
  Wire.begin();
  if (!sensor.init())
  {
    Serial.println("Failed to detect and initialize sensor!");
    tft.setTextColor(tft.color24to16(0xFF4500));
    tft.setTextSize(2);//ขนาดข้อความ
    tft.drawString("Failed to detect and initialize sensor!", 10, 120);
    while (1) {}
  }
  sensor.startContinuous();//วัดระยะ

  //setupแกน
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

  //หัวใจ
  Serial.println("Initializing...");
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30102 was not found. Please check wiring/power/solder jumper at MH-ET LIVE MAX30102 board. ");
    tft.setTextColor(tft.color24to16(0xFF4500));
    tft.setTextSize(2);//ขนาดข้อความ
    tft.drawString("Sensor not found.!MAX30102", 90, 120);
    delay(3000);
  }
  //Setup to sense a nice looking saw tooth on the plotter
  byte ledBrightness = 0x7F;  //Options: 0=Off to 255=50mA
  byte sampleAverage = 4;     //Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 2;           //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  //Options: 1 = IR only, 2 = Red + IR on MH-ET LIVE MAX30102 board
  int sampleRate = 200;       //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411;       //Options: 69, 118, 215, 411
  int adcRange = 16384;       //Options: 2048, 4096, 8192, 16384
  // Set up the wanted parameters
  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings
  particleSensor.setPulseAmplitudeRed(0x0A);    //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0);     //Turn off Green LED
  particleSensor.enableDIETEMPRDY();


  //เซนเซอร์วัอุณหภูมิ  GY-906 MLX90614
  /*
    Serial.println("Adafruit MLX90614 test");
    mlx.begin();
  */

  //เปิดไฟ led
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);    //LED บนบอร์ด(GPIO 2)

  //บัตเซอร์
  pinMode(buzzer, OUTPUT);  //buzzer



}
/*******************************************void loop**************************************************/
void loop() {

  Task1.check(t1Callback, time_A);  // ทำงานฟังก์ชั่น LED1_blink ทุก 1 วินาที
  Task2.check(t2Callback, time_A);
  Task3.check(t3Callback, time_A);
  Task4.check(t4Callback, time_A);
  Task5.check(t5Callback, time_A);
  Task6.check(t6Callback, time_A);

  //แสดงค่าเซนเซอร์
  heart_A();//หัวใจ
  spo2_A ();

  //เสียง
  /*    DacAudio.FillBuffer();// Fill the sound buffer with data
      long i = sensor.readRangeContinuousMillimeters();
      if(i > Distance)//600
      {
        char c = 'A';
        switch (c) {
            case 'A':
            DacAudio.Play(&ForceWithYou);          // play it, this will cause it to repeat and repeat...
            break;                                 //การพักคำสั่ง เมื่อ case ตรงตามเงื่อนไขจะออกจาก switch ทันที
         delay(1000);
        }
      }*/

  //LORA
  getReadings();
  sendReadings();
  //delay(1000);
}
/********************************************LoRA******************************************************/
void startLoRA() {
  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);

  while (!LoRa.begin(BAND) && counter < 10) {
    Serial.print(".");
    counter++;
    delay(500);
  }
  if (counter == 10) {
    // Increment readingID on every new reading
    readingID++;
    Serial.println("Starting LoRa failed!");
  }
  Serial.println("LoRa Initialization OK!");
}
void getReadings() {
  //รับค่าจากเซนเซอร์มาเก็บไว้
  phase1 = phase;
  AcX1 = AcX;
  AcY1 = AcY;
  AcZ1 = AcZ;
  ESpO2_1 = ESpO2;
  beatAvg1 = beatAvg;
}
void sendReadings() {
  //ส่งค่าให้กับ LORA อีกตัว
  LoRaMessage = String(readingID) + "/" + String(phase1) + "/" + String(AcX1) + "/" + String(AcY1) + "/" + String(AcZ1) + "/" + String(ESpO2_1) + "/" + String(beatAvg1);
    //Send LoRa packet to receiver
    LoRa.beginPacket();
    LoRa.print(LoRaMessage);
    LoRa.endPacket();

    Serial.print("phase :");
    Serial.println(phase1);

    Serial.print("AcX :");
    Serial.println(AcX1);

    Serial.print("AcY :");
    Serial.println(AcY1);

    Serial.print("AcZ :");
    Serial.println(AcZ1);

    Serial.print("ESpO2 :");
    Serial.println(ESpO2_1);

    Serial.print("beatAvg :");
    Serial.println(beatAvg1);

    Serial.print("Pressure:");
    Serial.println(pressure);

    Serial.print("Sending packet: ");
    Serial.println(readingID);
    readingID++;
}
/********************************************Task******************************************************/
void t1Callback() {
  //แสดงระยะ
  long phase = sensor.readRangeContinuousMillimeters();
  Serial.print("phase : ");
  Serial.println(phase);
  tft.fillRect(200, 25, 80, 20, tft.color24to16(0xfff486));//ปิดข้อความ
  tft.setTextColor(tft.color24to16(0xFF0066));//สีข้อความ
  tft.setTextSize(3);//ขนาดข้อความ
  tft.setCursor(210, 25);
  tft.print(phase);

  long pp_1 = 0;
  pp_1 = sensor.readRangeContinuousMillimeters();
  if (pp_1 <= Distance)
  {
    tft.fillRect(0, 85, 480, 95, tft.color24to16(0x76bcff));  //ปิดข้อความแจ้งเตือนตรงกลาง
    tft.setTextColor(tft.color24to16(0xFFFF00));
    tft.setTextSize(5); //ขนาดข้อความ
    tft.drawString("Be Careful", 100, 120);
    digitalWrite(buzzer, HIGH);
    delay(30);
    digitalWrite(buzzer, LOW);
  }
  else
  {
    tft.fillRect(0, 85, 480, 95, tft.color24to16(0x76bcff));  //ปิดข้อความแจ้งเตือนตรงกลาง
    tft.setTextColor(tft.color24to16(0xFFFF00));
    tft.setTextSize(5); //ขนาดข้อความ
    tft.drawString("BALANCE", 140, 120);
  }
}
/********************************************Task******************************************************/
void t2Callback() {

  //แสดงแกน
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);   // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true); // request a total of 14 registers
  AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L) //Tmp/340.00+36.53
  GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  Serial.print("ACC X:");
  Serial.print(AcX);
  Serial.print("  ACC Y:");
  Serial.print(AcY);
  Serial.print("  ACC Z:");
  Serial.print(AcZ);
  Serial.println();

  //แสดงแกนบนจอ
  tft.fillRect(0, 20, 110, 40, tft.color24to16(0x76bcff));//ปิดให้ค่าลัน
  tft.setTextColor(tft.color24to16(0x00FFCC));//สีข้อความ
  tft.setTextSize(1);//ขนาดข้อความ
  tft.drawString("ACC X: ", 10, 20);//ข้อความ
  tft.setCursor(45, 20);
  tft.print(AcX);
  tft.drawString("ACC Y: ", 10, 30);//ข้อความ
  tft.setCursor(45, 30);
  tft.print(AcY);
  tft.drawString("ACC Z: ", 10, 40);//ข้อความ
  tft.setCursor(45, 40);
  tft.print(AcZ);
}
/********************************************Task******************************************************/
void t3Callback() {

  //จำลองแสดงค่า Tmp
  tft.fillRect(380, 250, 100, 20, tft.color24to16(0xfff486));
  tft.setTextColor(tft.color24to16(0xFF0066));//สีข้อความ
  tft.setTextSize(3);//ขนาดข้อความ
  tft.setCursor(380, 250);
  tft.print(Tmp / 340.00 + 36.53);
}
/********************************************Task******************************************************/
void t4Callback() {
  //เอาไว้แสดงค่าอุณหภูมิ
  //เซนเซอร์วัอุณหภูมิ  GY-906 MLX90614
  /*Serial.print("Ambient = ");
    Serial.print(mlx.readAmbientTempC());
    Serial.print("*C\tObject = ");
    Serial.print(mlx.readObjectTempC()); Serial.println("*C");
    Serial.print("Ambient = ");
    Serial.print(mlx.readAmbientTempF());
    Serial.print("*F\tObject = ");
    Serial.print(mlx.readObjectTempF()); Serial.println("*F");
    Serial.println();*/
}
/********************************************Task******************************************************/
void t5Callback() {

  //นับเวลาการทำงาน
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 1000) {
    sec++;
    if (sec >= 60) {
      m++;
      sec = 0;
    }
    if (m >= 60) {
      h++;
      m = 0;
    }
    if (h >= 24) {
      h = 0;
    }
    previousMillis = currentMillis;
  }
  //  Serial.print(h);Serial.print(":");
  //  Serial.print(m);Serial.print(":");
  //  Serial.println(sec);

  tft.fillRect(0, 0, 110, 20, tft.color24to16(0x76bcff));
  tft.setTextColor(tft.color24to16(0xFF0066));//สีข้อความ
  tft.setTextSize(1);//ขนาดข้อความ
  tft.drawString("Time: ", 10, 10);   //ข้อความ
  tft.setCursor(40, 10);
  tft.print(h);
  tft.drawString(" : ", 50, 10);      //ข้อความ
  tft.setCursor(60, 10);
  tft.print(m);
  tft.drawString(" : ", 70, 10);      //ข้อความ
  tft.setCursor(80, 10);
  tft.print(sec);
}
/********************************************Task******************************************************/
void t6Callback() {
  //เงื่อนไขแจ้งเตือน
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);   // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true); // request a total of 14 registers
  AcX = Wire.read() << 8 | Wire.read();
  if (AcX > 8000)
  {
    char g = 'B';
    switch (g) {
      case 'B':
        digitalWrite(buzzer, HIGH);
        delay(50);
        digitalWrite(buzzer, LOW);
        break;                            //การพักคำสั่ง เมื่อ case ตรงตามเงื่อนไขจะออกจาก switch ทันที
    }
  }
  if (AcX > 8000) {
    tft.fillRect(420, 2, 58, 60, tft.color24to16(0x76bcff));
    tft.pushImage(420, 2, 58, 60, fall); //รูปคนล้ม
  } if (AcX < 8000) {
    tft.fillRect(420, 2, 58, 60, tft.color24to16(0x76bcff));
    tft.pushImage(420, 2, 58, 60, stand);
  }
}
