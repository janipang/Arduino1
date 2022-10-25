#include <Wire.h>
#include <string.h>
#define SLAVE_ADDR1 9
#define SLAVE_ADDR2 10
#define Me 1

int toWho = 0;
String myMsg = "";
String text = "";
char sendMsg[64];
bool waitNum = 1;
bool waitText = 0;

int hasReceivedData = 0; // ตัวแปรที่บอกว่าได้ข้อมูลจาก master แล้วรึยัง ถ้าได้ = 1 ถ้ายังไม่ได้ = 0

void receiveEvent(int) {
    hasReceivedData = 1;
    while (Wire.available()){
        myMsg += (char)Wire.read();
        //Serial.readString();
    }
    Serial.print("Wire.read >>");
    Serial.println(myMsg);
}

void serialReceived(int){
    while (Serial.available() > 0){
        char inByte = Serial.read();
        if (waitNum){
            toWho = (toWho * 10) + inByte - 48;
        }
        else if (waitText){
            text += inByte;
        }
        
        if (Serial.available() <= 0){
            waitNum = !waitNum;
            waitText = !waitText;
        }
    }
    if (toWho >= 1 && toWho <= 3){
        myMsg[6] = '1'; //token#1#
        myMsg += '0' + toWho; //token#1#n
        myMsg += '#'; //token#1#n#
        myMsg += text ; //token#1#n#message
    }
    else{
        Serial.println("send Failed! Invalid device");
    }
    myMsg += '\0';
    Serial.print("Serial.read >>");
    Serial.println(myMsg);
    Serial.print(toWho);
    Serial.println("------------");
    toWho = 0;
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
        if (Serial.available() > 0){ //มีการinputข้อมูล
            serialReceived;
        }
        else {

        }
    }
    else if(myMsg[6] == '1'){ //toWho = Me
        if(myMsg[8] - 48 == Me){
            Serial.println(myMsg);
            myMsg = "token#0#"; //ส่งแล้ว set ค่าคืน
        }
        else if(myMsg[8] - 48 != Me){
            myMsg = myMsg; //ส่งข้อความเดิม
        }
    }
    

    if (hasReceivedData){
        hasReceivedData = 0;

        changeTypeText();
        // byte buffer[myMsg.length() + 1];
        // myMsg.toCharArray(buffer, myMsg.length() + 1);
        Wire.beginTransmission(SLAVE_ADDR2);
        Wire.write(sendMsg);
        Wire.endTransmission();

        Serial.print("Final Msg >>");
        Serial.println(myMsg);
        Serial.print("Send  Msg >>");
        Serial.println(sendMsg);

        myMsg = "";
    }
    
}


