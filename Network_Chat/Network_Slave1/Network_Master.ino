#include <Wire.h>
// slave address เป็นเลขอะไรก็ได้ แค่ให้มันตรงกัน
#define SLAVE_ADDR1 9
#define SLAVE_ADDR2 10

// ตัวแปรที่ไว้เก็บข้อความที่ได้มาจาก slave2
char myMsg[10]; // requested data

void setup() {
    Serial.begin(9600);
  // ไม่รู้ทำไม แต่ถ้าไม่เขียน Serial.println อะไรสักอย่างก่อน มันจะ print แปลกๆ
    Serial.println("Master starts");
    Wire.begin();
}

void loop() {
    delay(1000);

    // ส่งข้อความ (ซึ่งก็คือ array ของ char) "token#0#" ไปที่ slave1
    Wire.beginTransmission(SLAVE_ADDR1);
    //          01234567 ข้อความนี้มี 8 ตัวอักษร (byte)
    Wire.write("token#0#");
    Wire.endTransmission();
    
    // master ขอข้อความจาก slave2
    Wire.requestFrom(SLAVE_ADDR2, 9);   // ขอข้อมูล 9 byte จาก slave2
    // ถ้า byte ตัวแรกเท่ากับหนึ่งให้แสดงผล, อันนี้เขียนให้เช็คว่า slave2 มีข้อมูลที่จะส่งมามั้ย ถ้าไม่มีก็ให้ byte ตัวแรกเป็น 0, ถ้ามีให้เป็น 1 แทน
    if (Wire.read() == '1')
    {
        for (byte i = 0; i < 8; i++)
        {
           myMsg[i] = Wire.read();
        }    
        myMsg[8] = '\0';
        Serial.println(myMsg);
    }    
}
