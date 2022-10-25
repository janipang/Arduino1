#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//ตัวแปรสำหรับเกม
int runner = 1;
int x_ball = 0, y_ball = 0, dx = 1, dy = 1; 
int x_bar = 63, y_bar = 32 ;
int score = 0;
void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();

  say_pong(); //เป็นหน้าต่างต้อนรับเข้าเกม
  /*display.invertDisplay(true); //ไว้ใช้ตอนจบโปรแกรม
  delay(1500);*/
}

void loop() {
  int x_bar = map(analogRead(2), 0, 1024, 0, 117);
  int y_bar = 31 ;
  
  if(x_ball <= 1){dx = 2;}
  if(x_ball >=123){dx = -2;}
  if(y_ball <= 1){dy = 2;}
  if(y_ball >= 31){
    if(x_ball >= x_bar && x_ball <= x_bar + 15){
      tone(9,440,300);
      dy = -2;
      score += 1;
    }
    else{
      runner = 0;
    }
  }
  x_ball += dx;
  y_ball += dy;
  display.clearDisplay();
  move_ball(x_ball, y_ball);
  move_bar(x_bar ,y_bar);

  
  if(runner == 0){
    gameover(score);
    runner = 1;
    x_ball = 0;
    y_ball = 0;
    score = 0;
    say_pong(); //เป็นหน้าต่างต้อนรับเข้าเกม
  }
}

void say_pong(){
  display.clearDisplay();
  
  display.setTextSize(3);   
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(20, 2); 
  display.println(F("PONG!"));
  
  display.setTextSize(4);
  display.setTextColor(SSD1306_WHITE); // 
  display.setCursor(60, 7);    
  display.println(F("_"));
  
  display.display();

  display.startscrollleft(0x0B, 0x0F);
  delay(900);
  display.stopscroll();
  delay(50);
  display.startscrollright(0x0B, 0x0F);
  delay(800);
  display.stopscroll();
  delay(40);
  display.startscrollleft(0x0B, 0x0F);
  delay(700);
  display.stopscroll();
  delay(50);  
  display.startscrollright(0x0B, 0x0F);
  delay(300);
  display.stopscroll();
  delay(100);
}
void move_ball(int x, int y){
  display.drawPixel(x, y, SSD1306_WHITE);
  display.display();
  
}
void move_bar(int x_bar, int y_bar){

  display.drawFastHLine(x_bar,y_bar,15,WHITE); //draw Horizontal line
  display.display();
}
void gameover(int score){
  display.clearDisplay();
  
  display.setTextSize(2);   
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(35, 2); 
  display.print(F("Game     Over!"));
  display.display();
  delay(1000);

  display.clearDisplay();
  
  display.setTextSize(1);   
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(20, 2); 
  display.println(F("Your score is"));

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(55, 15); 
  display.println(score); //ใส่คะแนน

  display.display(); 
  delay(4000);
}
