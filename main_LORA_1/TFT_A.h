
void TFT_AA() {


  //เริ่มต้น WiFi
  tft.fillScreen(TFT_BLUE);
  tft.setTextColor(tft.color24to16(0xFFFFFF));//สีข้อความ
  tft.setTextSize(3);                         //ขนาดข้อความ
  tft.drawString("WiFi connected", 100, 140);        //ข้อความ 100-160
  delay(2000);


  //ส่วนของเมนู วัดระยะ
  tft.fillScreen(TFT_BLUE);
  tft.setTextColor(tft.color24to16(0xFFFFFF));          //สีข้อความ
  tft.setTextSize(2);                                   //ขนาดข้อความ
  tft.drawString("Distance menu choice", 0, 10);        //ข้อความ 100-160
  tft.setTextSize(2);                                   //ขนาดข้อความ
  tft.drawString("choice => ", 0, 40);           //ข้อความ 100-170
  tft.drawString("(A) Warning distance : 800 mm", 0, 70);   //ข้อความ
  tft.drawString("(B) Warning distance : 1000 mm", 0, 110);
  tft.drawString("(C) Warning distance : 1500 mm", 0, 150);
  tft.setTextColor(tft.color24to16(0xFF0000));          //สีข้อความ
  tft.setTextSize(2.5);                                   //ขนาดข้อความ
  tft.drawString("Timeout select 5 ms", 0, 190);
  delay(10000);//ทำต่อใน 10 ms


  //เข้าเงื่อนไข
  choice = Serial.read();         //อ่านค่าที่เลือกจากSerial เปลี่ยนเป็นสวิตกด
  if ( choice == 'a') {
    Distance = 800;
    Serial.println("Distance = 800 mm");
    tft.fillScreen(TFT_BLUE);
    tft.setTextColor(tft.color24to16(0xFFFFFF));        //สีข้อความ
    tft.setTextSize(3);                                 //ขนาดข้อความ
    tft.drawString("Distance is: ", 60, 150);           //ข้อความ
    tft.setCursor(330, 150);
    tft.print(Distance);
    delay(500);
  }
  else if ( choice == 'b') {
    Distance = 1000;
    Serial.println("Distance = 1000 mm");
    tft.fillScreen(TFT_BLUE);
    tft.setTextColor(tft.color24to16(0xFFFFFF));        //สีข้อความ
    tft.setTextSize(3);                                 //ขนาดข้อความ
    tft.drawString("Distance is: ", 60, 150);          //ข้อความ
    tft.setCursor(330, 150);
    tft.print(Distance);
    delay(500);
  }
  else if ( choice == 'c') {
    Distance = 1500;
    Serial.println("Distance = 1500 mm");
    tft.fillScreen(TFT_BLUE);
    tft.setTextColor(tft.color24to16(0xFFFFFF));        //สีข้อความ
    tft.setTextSize(3);                                 //ขนาดข้อความ
    tft.drawString("Distance is: ", 60, 150);          //ข้อความ
    tft.setCursor(330, 150);
    tft.print(Distance);
    delay(500);
  }
  else {
    Distance = 600; Serial.println("Auto distance = 600 mm"); delay(1000);//( choice != 'a'||'b'||'c')
    tft.fillScreen(TFT_BLUE);
    tft.setTextColor(tft.color24to16(0xFFFFFF));        //สีข้อความ
    tft.setTextSize(3);                                 //ขนาดข้อความ
    tft.drawString("Auto distance", 130, 150);           //ข้อความ
    delay(2000);
  }

}
