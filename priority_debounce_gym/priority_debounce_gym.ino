//define button, LED, pressed

#define buttonPullDown 12
#define buttonPullUp 11
#define buttonInputPullUp 10

#define LED4 4 // red
#define LED3 3 // yellow
#define LED2 2 // green

#define pressedPullDown HIGH
#define pressedPullUp LOW

unsigned long timeNow = 0;
unsigned long timeLED4 = 0;
unsigned long timeLED3 = 0;
unsigned long timeLED2 = 0;  //variable for set light closing timer 

unsigned long pressedTime = 0; //variable for debouncing

void setup()
{
  Serial.begin(9600);
  pinMode(buttonPullUp, INPUT);
  pinMode(buttonPullDown, INPUT);
  pinMode(buttonInputPullUp, INPUT_PULLUP);

  pinMode(LED4, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED2, OUTPUT);
}

void loop()
{
  timeNow = millis();
  
  int readbuttonPullDown = digitalRead(buttonPullDown);
  int readbuttonPullUp = digitalRead(buttonPullUp);
  int readbuttonInputPullUp = digitalRead(buttonInputPullUp);

  toggleLED(LED4, readbuttonPullDown, pressedPullDown);
  toggleLED(LED3, readbuttonPullUp, pressedPullUp);
  toggleLED(LED2, readbuttonInputPullUp, pressedPullUp);

  checkLightFor3Seconds(timeLED4, 4);
  controlYellow(timeLED3, 3);
  checkLightFor3Seconds(timeLED2, 2);
}

void toggleLED(int LED, int reading, int pressed){
  if (LED == 2 && digitalRead(LED4)== HIGH){
    return;
  }
  if (LED == 3 && (digitalRead(LED4)== HIGH || digitalRead(LED2)== HIGH)){
    return;
  }
  
  if (reading == pressed && debounce(pressedTime))
  {
    Serial.println("millis() before pressed = ");
    Serial.println(millis());
    pressedTime = millis();

    if (LED != 3){
      digitalWrite(LED, !digitalRead(LED));
    }
//    delay(500);

    switch(LED){ //set timer after light is opening
      case 4:
        timeLED4 = millis();
        break;
       case 3:
        timeLED3 = millis();
        break;
       case 2:
        timeLED2 = millis();
        break;
       default:
        break;
    }
  }
}
void controlYellow(unsigned long timeLED, int LED){
    if(timeLED != 0 && millis()- timeLED >= 0 && millis()- timeLED < 500){
      digitalWrite(LED, HIGH);
    }
    else if(timeLED != 0 && millis()- timeLED >= 500 && millis()- timeLED < 1000){
      digitalWrite(LED, LOW);
    }
    else if(timeLED != 0 && millis()- timeLED >= 1000 && millis()- timeLED < 1500){
      digitalWrite(LED, HIGH);
    }
    else if(timeLED != 0 && millis()- timeLED >= 1500 && millis()- timeLED < 2000){
      digitalWrite(LED, LOW);
    }
    else{
      timeLED3 = 0; //reset timer sfter blink
    }
}
void checkLightFor3Seconds(unsigned long timeLED, int LED){ //after 3 sec ,close the light and reset timer
   if (timeLED != 0 && millis() - timeLED >= 3000)
  {
    Serial.println("millis() after pressed = ");
    Serial.println(millis());

    if(digitalRead(LED) == HIGH){
      digitalWrite(LED, LOW);
    }
    
    switch(LED){
      case 4:
        timeLED4 = 0;
        break;
//       case 3:
//        timeLED3 = 0;
       case 2:
        timeLED2 = 0;
        break;
       default:
        break;
    }
  }
}

boolean debounce(unsigned long pressedTime){
  int isNotBounce = 0;
  if(millis()- pressedTime >= 220){
    isNotBounce = 1; //ถ้าเวลากดห่างจากรอบที่แล้ว70msถือว่าเป็นการกด ไม่ใช่Bounce
  }
  return isNotBounce;
}
