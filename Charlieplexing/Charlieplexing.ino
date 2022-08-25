void setup() {

}

void loop() {
  //2
  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH);
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  pinMode(4, INPUT);
  delay(1000);

  //4
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  pinMode(3, INPUT);
  delay(700);

  //6
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  pinMode(3, OUTPUT);
  digitalWrite(3, LOW);
  pinMode(2, INPUT);
  delay(700);

  //5
  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH);
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  pinMode(2, INPUT);
  delay(700);
  
  //3
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  pinMode(3, INPUT);
  delay(700);
  
  //1
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  pinMode(3, OUTPUT);
  digitalWrite(3, LOW);
  pinMode(4, INPUT);
  delay(700); 
  
  ////////////////////////////
  
  //1
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  pinMode(3, OUTPUT);
  digitalWrite(3, LOW);
  pinMode(4, INPUT);
  delay(700);  
  
  //3
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  pinMode(3, INPUT);
  delay(700);

  //5
  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH);
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  pinMode(2, INPUT);
  delay(700);

  //6
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  pinMode(3, OUTPUT);
  digitalWrite(3, LOW);
  pinMode(2, INPUT);
  delay(700);

  //4
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  pinMode(3, INPUT);
  delay(700);
  
    //2
  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH);
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  pinMode(4, INPUT);
  delay(700);
  
}
