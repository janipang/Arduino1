#include <Wire.h>
#include <string.h>
#define SLAVE_ADDR1 9
#define SLAVE_ADDR2 10
#define Me 3

//select mode
bool debugMode = 0;

String receiveBuffer = "", gatewayBuffer = "", receivedMsg ="";
String serialBuffer = "", beginMsg ="";
char upload[100];

int inputStep = 0, toWho = 0;
String text = "";
int requestPrepared = 0;
int direction = 0;

void receiveEvent() {
    receiveBuffer = "";
    while (Wire.available()){
        int inByte = Wire.read();
        if(inByte < 128){
            receiveBuffer += (char)inByte ;
        }
    }    
    receiveBuffer += '\0';

    //Serial.print("Wire.read >>"); //for debugging
    //Serial.print(receiveBuffer);
    //Serial.print("----from SLAVE");
    //Serial.println(direction + 1);
}

void serialReceived(){
    
    while (Serial.available() > 0){
        char inByte = Serial.read();
        if (inputStep == 0){ //สถานะใส่เลขเครื่อง
            if (inByte >= 48 && inByte < 58){ //ให้มันทำงานเฉพาะตัวที่เป็นตัวเลข (ascii48 คือเลข 0)
                //Serial.print("inByte >>");
                //Serial.println(inByte);
                toWho = (toWho * 10) + inByte - 48;
            }
        }
        else if (inputStep == 1){ //สถานะใส่ข้อความ
            if (inByte != 10){
                text += inByte;
            }
        }

        if (Serial.available() <= 0){
            inputStep = (inputStep + 1) % 3;
        }
    }
    if (inputStep == 2){ //สถานะ"ใส่เสร็จแล้ว" เราจะเอาข้อความมาประกอบ
      //Serial.print("toWho >>");
      //Serial.println(toWho);
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
        //Serial.print("SerialBuffer >>");
        //Serial.println(serialBuffer);
        
        toWho = 0;
        text = "";
    }
}

void Upload(String message){
    for(int i = 0 ; i < message.length() ; i++){
        upload[i] = message[i];
    }
    upload[message.length()] = '\0';
    direction = (direction + 1) % 2;
}

void PrintCutMsg(String message){
    char cutMsg[64] = "";
    for(int i = 10 ; i < message.length() ; i++){ //วนลูปแค่ตัวที่เป็นข้อความหลังโทเคน
        cutMsg[i - 10] = message[i];
    }
    cutMsg[message.length() - 10] += '\0';
    Serial.println(cutMsg);
}

void setup() {
    Serial.begin(9600);
    Serial.println("Master starts"); //เขียนกันบัค
    Wire.begin();
    receiveBuffer = "token#0#";
    Upload(receiveBuffer);
}

void loop() {
    if (debugMode == 0){
        Wire.requestFrom(SLAVE_ADDR1 + direction , 64);
        if (Wire.available() > 0){
            receiveEvent();
        } 

        if (Serial.available() > 0){
            serialReceived();
        }

        if (receiveBuffer.length() > 0){ //ถ้าได้รับข้อมูลที่ส่งมา
            if (receiveBuffer[6] == '1'){
                gatewayBuffer = receiveBuffer;
                if (receiveBuffer[8] - 48 == Me){
                    receivedMsg = receiveBuffer;
                    Serial.print("receivedMsg >>"); //for debugging but it's better if u delete
                    PrintCutMsg(receivedMsg); //use this func to print only message behind token
                    Upload("token#0#");
                }
                else if (receiveBuffer[8] - 48 != Me){
                    Upload(gatewayBuffer);
                }
            }
            if (receiveBuffer[6] == '0'){
                if (serialBuffer.length() <= 0){
                    Upload(receiveBuffer);
                }
                else if (serialBuffer.length() > 0){
                    beginMsg = serialBuffer;
                    serialBuffer = "";//show last token#0#
                    Upload(beginMsg); //send new token
                    //Serial.print("beginMsg >>"); for debugging
                    //Serial.println(beginMsg);
                }
            }
            receiveBuffer = "";
            gatewayBuffer = "";
            receivedMsg = "";
            beginMsg = "";

            Wire.beginTransmission(SLAVE_ADDR1 + direction);
            Wire.write(upload);
            Wire.endTransmission();
        }
        delay(400);     
    }


//----------------------------------------------------------------------
 

    if (debugMode == 1){
        Wire.requestFrom(SLAVE_ADDR1 + direction , 64);
        if (Wire.available() > 0){
            receiveEvent();
        } 

        if (Serial.available() > 0){
            serialReceived();
        }

        if (receiveBuffer.length() > 0){ //ถ้าได้รับข้อมูลที่ส่งมา
            if (receiveBuffer[6] == '1'){
                gatewayBuffer = receiveBuffer;
                if (receiveBuffer[8] - 48 == Me){
                    receivedMsg = receiveBuffer;
                    Serial.print("receivedMsg >>"); //for debugging but it's better if u delete
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
                    Serial.println(beginMsg);
                }
            }
            
            Serial.print("----from SLAVE");
            Serial.println(((direction + 1) % 2) + 1); //แสดงค่าdirectionเดิมที่รับมาก่อนที่จะโดนฟังก์Uploadเปลี่ยน

            receiveBuffer = "";
            gatewayBuffer = "";
            receivedMsg = "";
            beginMsg = "";

            Wire.beginTransmission(SLAVE_ADDR1 + direction);
            Wire.write(upload);
            Wire.endTransmission();
        }
        delay(400);
    }
   
}
