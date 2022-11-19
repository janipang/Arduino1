int buttons[2] = {10, 11};
int readings[2];
// pressed = high

int buttonState[2];
int lastButtonState[2] = {LOW, LOW};
unsigned long lastDebounceTime[2];

// tinker is weird with the debounceDelay
long debounceDelay = 10;

int random_number = random(1, 9);
int guess_number = 0;

int bitPattern[10] = {B11000000, B11111001, B10100100, B10110000, B10011001, B10010010, B10000010, B11111000, B10000000, B10010000};
const byte numPins = 8;
//A, B, C, D, E, F, G, DP
const int segmentPins[8] = {2, 3, 4, 5, 6, 7, 8, 9};

void setup() {
  Serial.begin(19200);
  pinMode(buttons[0], INPUT);
  pinMode(buttons[1], INPUT);

  for (int i = 0; i < numPins; i++)
    pinMode(segmentPins[i], OUTPUT);

  Serial.print("random number = ");
  Serial.println(random_number);
}

void loop() {
  handle_guess_button();
  handle_start_button();
}


void displayNumber(int number)
{
  boolean isBitSet;
  for (int segment = 0; segment < 8; segment++) {
    isBitSet = bitRead(bitPattern[number], segment);
    digitalWrite(segmentPins[segment], isBitSet);
  }
}

void displayGuessResult(int guessResult)
{
  //0          G          L
  int guessBitPattern[3] = {B11000000, B11000010, B11000111};
  boolean isBitSet;
  for (int segment = 0; segment < 8; segment++) {
    isBitSet = bitRead(guessBitPattern[guessResult], segment);
    digitalWrite(segmentPins[segment], isBitSet);
  }
}

void handle_guess_button()
{
  readings[0] = digitalRead(buttons[0]);
  //increment guess number in pressed.
  if (readings[0] != lastButtonState[0])
  {
    lastDebounceTime[0] = millis();
  }
  if ((millis() - lastDebounceTime[0]) > debounceDelay)
  {
    if (readings[0] != buttonState[0])
    {
      buttonState[0] = readings[0];

      if (buttonState[0] == HIGH)
      {
        //increment guess number in pressed.
        guess_number = guess_number + 1 > 9 ? 1 : guess_number + 1;
        displayNumber(guess_number);
      }
    }
  }
  lastButtonState[0] = readings[0];
}

void handle_start_button()
{
  readings[1] = digitalRead(buttons[1]);
  // if start pressed :
  if (readings[1] != lastButtonState[1])
  {
    lastDebounceTime[1] = millis();
  }
  if ((millis() - lastDebounceTime[1]) > debounceDelay)
  {
    if (readings[1] != buttonState[1])
    {
      buttonState[1] = readings[1];

      if (buttonState[1] == HIGH)
      {
        if (guess_number == random_number)
        { 
          displayGuessResult(0);
          random_number = random(1,9);
          Serial.print("random number = ");
          Serial.println(random_number);
        }
        else if (guess_number > random_number) 
        {
          displayGuessResult(1);
        }
        else
        {
          displayGuessResult(2);
        }
      }
    }
  }
  lastButtonState[1] = readings[1];
}
