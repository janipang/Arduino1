#include <Wire.h>
#include <string.h>
#define SLAVE_ADDR1 9
#define SLAVE_ADDR2 10
#define Me 1

int toWho = 0;
String myMsg = "";
String text = "";
char sendMsg[64];
int inputStep = 0;
bool waitNum = 1;
bool waitText = 0;
bool tokenState = 0;

int hasReceivedData = 0; // ตัวแปรที่บอกว่าได้ข้อมูลจาก master แล้วรึยัง ถ้าได้ = 1 ถ้ายังไม่ได้ = 0

void receiveEvent(int) { //เก็บข้อมูลที่ได้รับมาไว้ใน myMsg
    hasReceivedData = 1;
    myMsg = "";
    while (Wire.available()){
        myMsg += (char)Wire.read();
        //Serial.readString();
    }
    Serial.print("Wire.read >>");
    Serial.println(myMsg);
}

void serialReceived(){
    Serial.println("---infunc---");
    while (Serial.available() > 0){
        char inByte = Serial.read();
        Serial.println("---reading---");
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
    if (inputStep == 2){ //สถานะใส่เสร็จแล้ว เราเอามาประกอบข้อความ
        if (toWho >= 1 && toWho <= 3){
            myMsg[6] = '1'; //token#1#
            myMsg += toWho; //token#1#n
            myMsg += '#'; //token#1#n#
            myMsg += text ; //token#1#n#message
            toWho = 0;
        }
        else{
            Serial.println("send Failed! Invalid device");
            toWho = 0;
        }
        myMsg += '\0';
        Serial.println("---input2---");
    }
    Serial.print("Serial.read >>");
    Serial.println(myMsg);
    Serial.print(toWho);
    Serial.println("------------");
}

void changeTypeText(){
    if(myMsg != ""){
        for(int i = 0 ; i < myMsg.length() ; i++){
            sendMsg[i] = myMsg[i];
        }
        sendMsg[strlen(sendMsg)] = '\0';
        myMsg = "";
    }
}

void setup() {
    Serial.begin(9600);
    Serial.println("Slave1 starts");
    Wire.begin(SLAVE_ADDR1);
    Wire.onReceive(receiveEvent);
}

void loop() {
    if(myMsg[6] == '0'){
        Serial.println("---IN IF IN IF IN IF---");
        if (Serial.available() > 0){ //มีการinputข้อมูล
            Serial.println("---know that inputted----");
            serialReceived();
        }
        else {

        }
    }
    else if(myMsg[6] == '1'){ //toWho = Me
        if(myMsg[8] - 48 == Me){
            Serial.println(myMsg);
            myMsg = "token#0#"; //printแล้ว set ค่าคืน
        }
        else if(myMsg[8] - 48 != Me){
            myMsg = myMsg; //ส่งข้อความเดิม
        }
    }
    

    if (hasReceivedData){
        hasReceivedData = 0;

        Serial.print("Final Msg >>");
        Serial.println(myMsg);

        changeTypeText();
        // byte buffer[myMsg.length() + 1];
        // myMsg.toCharArray(buffer, myMsg.length() + 1);
        Wire.beginTransmission(SLAVE_ADDR2);
        Wire.write(sendMsg);
        Wire.endTransmission();

        Serial.print("Send  Msg >>");
        Serial.println(sendMsg);

    }
    
}


