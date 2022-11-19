int buttons[3] = {12, 11, 10};
int readings[3];
int LEDPins[3] = {4, 3, 2};
int pressed[3] = {HIGH, LOW, LOW};
unsigned long timeWhenPressedLED[3] = {0, 0, 0};
// We use timeWhenPressedLED to check if the button has been pressed. If it hasn't, then timeWhenPressed = 0.
// We also use it to let the light be high for 3 seconds.

int buttonState[3];
int lastButtonState[3] = {LOW, HIGH, HIGH};
unsigned long lastDebounceTime[3];

unsigned long timeNow = 0;
unsigned long pressedTime = 0;

long debounceDelay = 50;

void setup()
{
  Serial.begin(9600);
  pinMode(buttons[0], INPUT);
  pinMode(buttons[1], INPUT);
  pinMode(buttons[2], INPUT_PULLUP);

  pinMode(LEDPins[0], OUTPUT);
  pinMode(LEDPins[1], OUTPUT);
  pinMode(LEDPins[2], OUTPUT);
}

void loop()
{
  timeNow = millis();

  for (int i = 0; i < 3; i++) {
    readings[i] = digitalRead(buttons[i]);
    debounce(i);
    checkLightFor3Seconds(i);
  }

  controlYellow();
}

void debounce(int button)
{
  if (readings[button] != lastButtonState[button])
  {
    lastDebounceTime[button] = millis();
  }
  if ((millis() - lastDebounceTime[button]) > debounceDelay)
  {
    if (readings[button] != buttonState[button])
    {
      buttonState[button] = readings[button];

      if (buttonState[button] == pressed[button])
      {
        toggleLED(button);
      }
    }
  }
  lastButtonState[button] = readings[button];
}


void toggleLED(int button){
  // if red is high and the button that is pressed is not red
  if (digitalRead(LEDPins[0]) == HIGH && LEDPins[button] != LEDPins[0]){
    return;
  }

  // if green is high and the button that is pressed is yellow
  if (digitalRead(LEDPins[2]) == HIGH && LEDPins[button] == LEDPins[1]){
    return;
  }

  timeWhenPressedLED[button] = millis();
  Serial.print("timeWhenPressedLED[button] (time when pressed this button) = ");
  Serial.println(timeWhenPressedLED[button]);
        
  if (LEDPins[button] != 3){
    digitalWrite(LEDPins[button], !digitalRead(LEDPins[button]));
    return;
  }
}

void controlYellow()
{
  if (timeWhenPressedLED[1] == 0){
    return;
  }
  
  int timeAfterPressedLED3 = millis() - timeWhenPressedLED[1];

  if ((timeAfterPressedLED3 > 0 && timeAfterPressedLED3 < 500) || (timeAfterPressedLED3 >= 1000 && timeAfterPressedLED3 < 1500))
  {
    digitalWrite(LEDPins[1], HIGH);
  }
  else if ((timeAfterPressedLED3 >= 500 && timeAfterPressedLED3 < 1000) || (timeAfterPressedLED3 >= 1500 && timeAfterPressedLED3 < 2000))
  {
    digitalWrite(LEDPins[1], LOW);
  }
  else
  {
    timeWhenPressedLED[1] = 0;
  }
}

void checkLightFor3Seconds(int button)
{ 
  if (timeWhenPressedLED[button] == 0 || LEDPins[button] == 3){
    return;
  }
  
  if (millis() - timeWhenPressedLED[button] >= 3000)
  {
    if (digitalRead(LEDPins[button]) == HIGH)
    {
      digitalWrite(LEDPins[button], LOW);
    }

    Serial.print("timeAfterPressedLED[button] (time after pressed this button) = ");
    Serial.println(millis());
    timeWhenPressedLED[button] = 0;
  }
}
