#include <Wire.h>
#include <string.h>
#define SLAVE_ADDR1 9
#define SLAVE_ADDR2 10
#define Me 1

String receiveBuffer = "", gatewayBuffer = "", receivedMsg ="";
String serialBuffer = "", beginMsg ="";
char upload[64];

int inputStep = 0, toWho = 0;
String text = "";

void receiveEvent(int) {
    receiveBuffer = "";
    while (Wire.available()){
        receiveBuffer += (char)Wire.read();
    }
    Serial.print("Wire.read >>");
    Serial.println(receiveBuffer);
}

void serialReceived(){
    Serial.println("---infunc---"); //ต้องปริ้นไม่งั้นมันบัค
    while (Serial.available() > 0){
        char inByte = Serial.read();
        Serial.println("---reading---"); //ต้องปริ้นไม่งั้นมันบัค
        if (inputStep == 0){ //สถานะใส่เลขเครื่อง
            toWho = (toWho * 10) + inByte - 48;
            Serial.println("---input0---");
        }
        else if (inputStep == 1){ //สถานะใส่ข้อความ
            text += inByte;
            Serial.println("---input1---");
        }

        if (Serial.available() <= 0){
            inputStep = (inputStep + 1) % 3;
        }
    }
    if (inputStep == 2){ //สถานะ"ใส่เสร็จแล้ว" เราจะเอาข้อความมาประกอบ
        if (toWho >= 1 && toWho <= 3){
            serialBuffer = "token#S#N#";
            serialBuffer[6] = '0' + 1;
            serialBuffer[8] = '0' + toWho;
            serialBuffer += text;
            serialBuffer += '\0';
            inputStep = (inputStep + 1) % 3;
        }
        else{
            Serial.println("!!!!!!! send Failed! Invalid device");
            serialBuffer = "";
            inputStep = (inputStep + 1) % 3;
        }
        Serial.println("---input2---");
        Serial.print("SerialBuffer >>");
        Serial.println(serialBuffer);
        
        toWho = 0;
        text = "";
    }
}

void Upload(String message){
    for(int i = 0 ; i < message.length() ; i++){
        upload[i] = message[i];
    }
    upload[message.length()] = '\0';
}

void setup() {
    Serial.begin(9600);
    Serial.println("Slave1 starts");
    Wire.begin(SLAVE_ADDR1);
    Wire.onReceive(receiveEvent);
}

void loop() {
    if (Serial.available() > 0){
        serialReceived();
    }

    if (receiveBuffer.length() > 0){ //ถ้าได้รับข้อมูลที่ส่งมา
        if (receiveBuffer[6] == '1'){
            gatewayBuffer = receiveBuffer;
            if (receiveBuffer[8] - 48 == Me){
                receivedMsg = receiveBuffer;
                Serial.println(receivedMsg);
                Upload("token#0#");
            }
            else if (receiveBuffer[8] - 48 != Me){
                Serial.println(gatewayBuffer);
                Upload(gatewayBuffer);
            }
        }
        if (receiveBuffer[6] == '0'){
            if (serialBuffer.length() <= 0){
                Serial.println(receiveBuffer);
                Upload(receiveBuffer);
            }
            else if (serialBuffer.length() > 0){
                beginMsg = serialBuffer;
                serialBuffer = "";
                Serial.println(receiveBuffer); //show last token#0#
                Upload(beginMsg); //send new token
                Serial.print("beginMsg >>");
            }
        }
        receiveBuffer = "";
        gatewayBuffer = "";
        receivedMsg = "";
        beginMsg = "";

        Wire.beginTransmission(SLAVE_ADDR2);
        Wire.write(upload);
        Wire.endTransmission();
    }
    
}
