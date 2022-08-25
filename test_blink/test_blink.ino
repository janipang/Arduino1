//const int firstPin =  11;
//const int secondled = 9 ;
//const int thirdled =  8;
//const int fourthled = 7 ;

#define firstPin 11
#define secondPin 9
#define thirdPin 7
#define fourthPin 5

void setup() {
  pinMode(firstPin, OUTPUT);
  pinMode(secondPin, OUTPUT);
  pinMode(thirdPin, OUTPUT);
  pinMode(fourthPin, OUTPUT);

}

void loop() {
    digitalWrite(firstPin, HIGH);
    delay(500);
    digitalWrite(secondPin, HIGH);
    delay(500);
    digitalWrite(thirdPin, HIGH);
    delay(500);
    digitalWrite(fourthPin, HIGH);
    delay(500);

    
    digitalWrite(firstPin, LOW);
//    delay(200);    
    digitalWrite(secondPin, LOW);
//    delay(200);    
    digitalWrite(thirdPin, LOW);
//    delay(200);      
    digitalWrite(fourthPin, LOW);
//    delay(200); 
    delay(1000);
}
