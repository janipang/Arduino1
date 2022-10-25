#include <Wire.h>
#define SLAVE_ADDR1 9
#define SLAVE_ADDR2 10

char myMsg[10] = {'0', 't', 'h', 'e', 'r', 'e', 'i', 's', 'n', 'o'}; // 0 ตัวแรกเป็นเหมือน state ที่เอาไว้บอก master ว่ายังไม่มีข้อมูลส่งให้เฉยๆ (ที่เหลือคือพิมมั่วๆ ไม่มีอะไร)

void receiveEvent(int) {
    // ถ้า slave2 ได้รับข้อมูลแล้ว ให้ตัวแรกเป็น 1 เพื่อบอก master ว่าได้ข้อมูลแล้วทีหลัง
    myMsg[0] = '1';

    // อ่านข้อมูลที่เหลือหลังจากตัวแรก
    for (byte i = 1; i < 8; i++)
    {
       myMsg[i] = Wire.read();
       Serial.print(myMsg[i]);
    }

    myMsg[8] = Wire.read();
    Serial.println(myMsg[8]);
    myMsg[9] = '\0';  //null character
}

// function นี้จะทำงานเมื่อ master ขอข้อมูลจาก slave2
void requestEvent() {
    // เราเซ็ตให้ master ขอข้อมูลทุก 1000 ms ใน loop ถ้ามันมีข้อมูล (ถ้าตัวแรก = 1 แปลว่ามีข้อมูล) มันถึงจะส่งให้
    if (myMsg[0] == '1')
    {
        Wire.write(myMsg);
        myMsg[0] = '0';
    } 
}

void setup() {
    Serial.begin(9600);
    Serial.println("Slave2 starts");
    Wire.begin(SLAVE_ADDR2);
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent); // อันนี้ก็เซ็ตว่า ถ้ามีการขอข้อมูลต้องเรียก function อะไร
}

void loop() {
    
}
