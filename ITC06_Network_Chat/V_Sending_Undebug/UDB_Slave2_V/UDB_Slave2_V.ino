#include <Wire.h>
#include <string.h>
#define SLAVE_ADDR1 9
#define SLAVE_ADDR2 10
#define Me 2

//select mode
bool debugMode = 1;

String receiveBuffer = "", gatewayBuffer = "", receivedMsg ="";
String serialBuffer = "", beginMsg ="";
char upload[100];

int inputStep = 0, toWho = 0;
String text = "";
int requestPrepared = 0;

void receiveEvent(int) {
    receiveBuffer = "";
    while (Wire.available()){
        receiveBuffer += (char)Wire.read();
    }
    //Serial.print("Wire.read >>"); /for debugging
    //Serial.println(receiveBuffer);
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

void Upload(String message){ //for change datatype to char and use variable "upload" to collect msg u want to Wire.write()
    for(int i = 0 ; i < message.length() ; i++){
        upload[i] = message[i];
    }
    upload[message.length()] = '\0';
    requestPrepared = 1; //after upload it'll be ready to write() by the request
}

void PrintCutMsg(String message){ //use only in normal mode that show only text behind token so we don't print first 10 index token#S#N#
  	char cutMsg[64] = "";
    for(int i = 10 ; i < message.length() ; i++){ //วนลูปแค่ตัวที่เป็นข้อความหลังโทเคน
      	cutMsg[i - 10] = message[i];
    }
  	cutMsg[message.length() - 10] += '\0';
  	Serial.println(cutMsg);
}

void requestEvent(){
    if (requestPrepared == 1)
    {
        Wire.write(upload);
        requestPrepared = 0;
    } 
}

void setup() {
    Serial.begin(9600);
    Serial.println("Slave2 starts");
    Wire.begin(SLAVE_ADDR2);
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent); 
}

void loop() {
    if (debugMode == 0){ //normal mode :will show only massege that terminates at this device 
        if (Serial.available() > 0){
            serialReceived();
        }

        if (receiveBuffer.length() > 0){ //ถ้าได้รับข้อมูลที่ส่งมา
            if (receiveBuffer[6] == '1'){
                gatewayBuffer = receiveBuffer;
                if (receiveBuffer[8] - 48 == Me){
                    receivedMsg = receiveBuffer;
                    Serial.print("receivedMsg >>"); //for debugging
                    PrintCutMsg(receivedMsg);
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
                    serialBuffer = "";
                    Upload(beginMsg); //send new token
                    //Serial.print("beginMsg >>"); //for debugging
                    //Serial.println(beginMsg);
                }
            }
            receiveBuffer = ""; //reset variable value
            gatewayBuffer = "";
            receivedMsg = "";
            beginMsg = "";

        }
        delay(400);
    }  


//----------------------------------------------------------------------


    if (debugMode == 1){ //T. required us to have debug mode that show every token that have transmitted
        if (Serial.available() > 0){
            serialReceived();
        }

        if (receiveBuffer.length() > 0){ //ถ้าได้รับข้อมูลที่ส่งมา
            if (receiveBuffer[6] == '1'){
                gatewayBuffer = receiveBuffer;
                if (receiveBuffer[8] - 48 == Me){
                    receivedMsg = receiveBuffer;
                    Serial.print("receivedMsg >>"); //for debugging
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
                    //Serial.print("beginMsg >>");
                    //Serial.println(beginMsg);
                }
            }
            receiveBuffer = "";
            gatewayBuffer = "";
            receivedMsg = "";
            beginMsg = "";

        }
        delay(400); //use this value bc. its so fast that i cant find the error :but gym can change it na
    }
  
}
