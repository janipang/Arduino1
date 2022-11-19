#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>
#include <TimerOne.h>

#define OLED_RESET -1
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define BUZZER 11
#define LDR 3
#define potent_pin A1

const int button[5] = {4, 8, 7 ,6 ,5}; //save mode1 mode2 mode3 clear
int lastButtonState[5] = {LOW ,LOW ,LOW ,LOW ,LOW};
unsigned long lastDebounceTime[5];
int buttonState[5] = {LOW ,LOW ,LOW ,LOW ,LOW};
int readings[5];
const int debounceDelay = 50;

int eeAddress = 0;

struct Time {
  int sec;
  int minn;
  int hour;
};
Time Clock;
Time Clock2;
Time Clock3;
Time ClockCD; //cd is from countdown it will get value of delta time = Clock2 - Clock
Time Clock4; //no saving to EEPROM

int song_index = 0;
unsigned long song_time = 0;
int C5 = 262, D5 = 294, E5 = 320, F5 = 349, G5 = 392, La5 = 440;
int note[54] = {C5, D5, F5, D5, La5, La5, G5,
                C5, D5, F5, D5, G5, G5, F5, C5, D5, F5, D5, F5, G5, E5, D5, C5, C5, G5, F5, C5, D5, F5, D5, La5, La5, G5,
                C5, D5, F5, D5, 523, E5, F5, E5, D5, C5, D5, F5, D5, F5, G5, E5, D5, C5, C5,
                G5, F5};
int period[54] =
    {200, 200, 200, 200, 600, 600, 1200, 200, 200, 200
    , 200, 600, 600, 1200, 200, 200, 200, 200, 800, 400
    , 600, 200, 800, 400, 800, 1600, 200, 200, 200, 200
    , 600, 600, 1200, 200, 200, 200, 200, 800, 400, 600
    , 200, 400, 200, 200, 200, 200, 800, 400, 600, 200
    , 800, 400, 800, 3200};
int mode = 0;
int modeStep[4] = {5 ,4 ,2 ,4}; //null alarm countdown setting
int modeRound[4] = {1 ,5 ,3 ,5};

int beeping = 0;
unsigned long start_beep = 0;
unsigned long success_setting = 0;
int countdownTime = 0;

///////////////////////////////////////////////////////////////////////////////////////////

int debounce(int i){
  int return_val = 0;
  if (readings[i] != lastButtonState[i])
  {
    lastDebounceTime[i] = millis();
  }
  if ((millis() - lastDebounceTime[i]) > debounceDelay)
  {
    if (readings[i] != buttonState[i])
    {
      buttonState[i] = readings[i];

      if (buttonState[i] == HIGH)
      {
        return_val = 1;
      }
    }
  }
  lastButtonState[i] = readings[i];
  return return_val;
}

void open_clock(){
  EEPROM.get(eeAddress, Clock);
  EEPROM.get(eeAddress + (1 * sizeof(Time)), Clock2);
  EEPROM.get(eeAddress + (2 * sizeof(Time)), Clock3);
  EEPROM.get(eeAddress + (3 * sizeof(Time)), ClockCD);
  EEPROM.get(eeAddress + (4 * sizeof(Time)), mode);
  EEPROM.get(eeAddress + (4 * sizeof(Time) + 1*sizeof(int)), modeStep[mode]);
  EEPROM.get(eeAddress + (4 * sizeof(Time) + 2*sizeof(int)), countdownTime);
  Serial.println("___________get__________");
  Serial.println(String(Clock.hour) + ":" + String(Clock.minn) + ":" + String(Clock.sec));
  Serial.println("mode:" + String(mode) + "   modeStep:" + String(modeStep[mode]));
//  Clock.sec += 1; //คาถากันบัค
}

void run_time(){
  Clock.sec += 1;
  Clock.minn += Clock.sec / 60;
  Clock.hour += Clock.minn / 60;
  Clock.sec %= 60;
  Clock.minn %= 60;
  Clock.hour %= 24;
}

void alarm_mode(){
    int data = analogRead(potent_pin);
    if (modeStep[1] == 0){
      display_mode();
      Clock2.hour = 0;
      Clock2.minn = 0;
      Clock2.sec = 0;
    }
    else if (modeStep[1] == 1){
      Clock2.hour = map(data, 0, 1023, 0, 23);
      display_time(Clock2);
    }
    else if (modeStep[1] == 2){
      Clock2.minn = map(data, 0, 1023, 0, 59);
      display_time(Clock2);
    }
    else if (modeStep[1] == 3){
      Clock2.sec = map(data, 0, 1023, 0, 59);
      display_time(Clock2);
    }
    else if (modeStep[1] == 4){
      display_run_alarm();
      if((Clock.hour == Clock2.hour) && (Clock.minn == Clock2.minn) && (Clock.sec == Clock2.sec)){ //ถ้าถึงเวลาที่ตั้งไว้
        beeping = 1;
        song_time = millis();
      }
    }
}

void countdown(){
    int data = analogRead(potent_pin);
    if (modeStep[2] == 0){
      display_mode();
    }
    else if (modeStep[2] == 1){
      countdownTime = map(data, 0, 1023, 0, 12) * 5;//*1
      Clock3_init(countdownTime);
      update_ClockCD();
      display_time(ClockCD);
    }
    else if (modeStep[2] == 2){
      if (ClockCD.hour == 5){
        //ไม่ต้องอัพเดทเวลา ให้มันเป็น5:00:00ไป
      }
      else if((ClockCD.hour == 0) && (ClockCD.minn == 0) && (ClockCD.sec == 0)){ //ถ้าถึงเวลาที่ตั้งไว้
        beeping = 1;
        song_time = millis(); //ผ่านบรรทัดนี้หลายรอบไม่เหมือนอลาร์ม
        ClockCD.hour = 5; //แค่ต้องเปลี่ยนเอาร์เป็นอะไรก็ได้ที่ไม่ใช่ศุนย์มันจะได้ไม่เข้าลูป
      }
      else{
        update_ClockCD(); //ใช้การเอา clock3 - clockที่เดินอยู่
      }
      display_run_countdown();
    }
}

void setting_mode(){
    int data = analogRead(potent_pin);
    if (modeStep[3] == 0){
      display_mode();
    }
    else if (modeStep[3] == 1){
      Clock4.hour = map(data, 0, 1023, 0, 23);
      display_time(Clock4);
    }
    else if (modeStep[3] == 2){
      Clock4.minn = map(data, 0, 1023, 0, 59);
      display_time(Clock4);
    }
    else if (modeStep[3] == 3){
      Clock4.sec = map(data, 0, 1023, 0, 59);
      display_time(Clock4);
    }
    else if (modeStep[3] == 4){
      if (millis() - success_setting < 1000){
        display_finished_setting();
      }
      else{
        Clock.hour = Clock4.hour;
        Clock.minn = Clock4.minn;
        Clock.sec = Clock4.sec;
        mode = 0;
      }
    }
}

void display_run_alarm(){
  display.clearDisplay();

  String time_now = get_time_pattern(Clock.hour ,Clock.minn ,Clock.sec);
  String time_set = get_time_pattern(Clock2.hour ,Clock2.minn ,Clock2.sec);

  display.drawRect(0, 0, 128, 32, WHITE);
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);   
  display.setCursor(20, 5); 
  display.println(time_now);
  
  display.setTextSize(1);   
  display.setCursor(45, 23); 
  display.println(time_set);
  
  display.dim(digitalRead(LDR));
  display.display();
}

void display_time(Time clock){
  if(mode == 2){
    display.clearDisplay();

    display.drawRect(0, 0, 128, 32, WHITE);
    String time_now = get_time_pattern(clock.hour ,clock.minn ,clock.sec);
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(40, 8);
    display.println(time_now);
    display.dim(digitalRead(LDR));
    display.display();
  }
  else{
    display.clearDisplay();

    display.drawRect(0, 0, 128, 32, WHITE);
    String time_now = get_time_pattern(clock.hour ,clock.minn ,clock.sec);
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(20, 8);
    display.println(time_now);
    display.dim(digitalRead(LDR));
    display.display();
  }
  
}

void display_mode(){
  if (mode == 0){
  }
  if (mode == 1){
    display.clearDisplay();
    display.drawRect(0, 0, 128, 32, WHITE);
    display.setTextSize(2);   
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(6, 10); 
    display.println("ALARM MODE");
    display.dim(digitalRead(LDR));
    display.display();
  }
  if (mode == 2){
    display.clearDisplay();
    display.drawRect(0, 0, 128, 32, WHITE); 
    display.setTextSize(2);   
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 10); 
    display.println("COUNTDOWN");
    display.dim(digitalRead(LDR));
    display.display();
  }
  if (mode == 3){
    display.clearDisplay();
    display.drawRect(0, 0, 128, 32, WHITE); 
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(5, 10);
    display.println("RESET TIME");
    display.dim(digitalRead(LDR));
    display.display();
  }
    
}

String get_time_pattern(int hour, int minn ,int sec){
  if (mode == 0 || mode == 1 || mode == 3){
    String time_pattern = "00:00:00";
    time_pattern[0] = (hour / 10) + '0';
    time_pattern[1] = (hour % 10) + '0';
    
    time_pattern[3] = (minn / 10) + '0';
    time_pattern[4] = (minn % 10) + '0';
    
    time_pattern[6] = (sec / 10) + '0';
    time_pattern[7] = (sec % 10) + '0';
    return time_pattern;
  }
  if (mode == 2){
    String time_pattern = "00:00";
    time_pattern[0] = (minn / 10) + '0';
    time_pattern[1] = (minn % 10) + '0';
    
    time_pattern[3] = (sec / 10) + '0';
    time_pattern[4] = (sec % 10) + '0';
    return time_pattern;
  }
}

void save_Clock2_to_EEPROM(){
  EEPROM.put(eeAddress + (1 * sizeof(Time)), Clock2);
  Serial.println("___________saved2___________");
  String time_now = get_time_pattern(Clock2.hour ,Clock2.minn ,Clock2.sec);
  Serial.println(time_now);
}

void save_Clock3_to_EEPROM(){
  EEPROM.put(eeAddress + (2 * sizeof(Time)), Clock3);
  Serial.println("___________saved3___________");
  String time_now = get_time_pattern(Clock3.hour ,Clock3.minn ,Clock3.sec);
  Serial.println(time_now);
}

void display_slct_time2(){
  display.clearDisplay();

  String time_now = get_time_pattern(0 ,countdownTime ,0);
  display.drawRect(0, 0, 128, 32, WHITE); 
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(40, 8); 
  display.println(time_now);
  display.dim(digitalRead(LDR));
  display.display();
  
}

void Clock3_init(int minn){
  
  Clock3.hour = Clock.hour;
  Clock3.minn = Clock.minn;
  Clock3.sec = Clock.sec;
      
  Clock3.sec = Clock.sec;
  Clock3.minn += minn;
  Clock3.hour += Clock3.minn / 60;
  Clock3.sec %= 60;
  Clock3.minn %= 60;
  Clock3.hour %= 24;
  
}

void update_ClockCD(){
  ClockCD.hour = Clock3.hour - Clock.hour;
  ClockCD.minn = Clock3.minn - Clock.minn;
  ClockCD.sec = Clock3.sec - Clock.sec;
  if (ClockCD.sec < 0){
    ClockCD.minn -= 1;
    ClockCD.sec += 60;
  }
  if (ClockCD.minn < 0){
    ClockCD.hour -= 1;
    ClockCD.minn += 60;
  }
  if (ClockCD.hour < 0){
    ClockCD.hour += 24;
  }
}

void display_run_countdown(){
  display.clearDisplay();

  String time_now = get_time_pattern(ClockCD.hour ,ClockCD.minn ,ClockCD.sec); //ผลต่าง
  String time_set = get_time_pattern(0 ,countdownTime ,0); // 00:05:00
  
  display.drawRect(0, 0, 128, 32, WHITE); 
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);   
  display.setCursor(36, 5); 
  display.println(time_now);
  
  display.setTextSize(1);   
  display.setCursor(50, 22); 
  display.println(time_set);
  
  display.dim(digitalRead(LDR));
  display.display();
}

void display_finished_setting(){
  display.clearDisplay();

  display.drawRect(0, 0, 128, 32, WHITE); 
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);   
  display.setCursor(5, 8); 
  display.println("Clock Set!");
  display.dim(digitalRead(LDR));
  display.display();
}

void cancel(){
  modeStep[1] = 4;
  modeStep[2] = 2;
  modeStep[3] = 4;
  beeping = 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
    Serial.begin(9600);
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
      Serial.println("SSD1306 allocation failed");
    } 
    else {
      Serial.println("All OLED Start Work !!!");
    }
    for(int i = 0 ; i < 5 ; i++){
      pinMode(button[i] , INPUT); 
    }
    pinMode(BUZZER ,OUTPUT);
    open_clock();
    display_time(Clock);
    Timer1.initialize(1000000);
    Timer1.attachInterrupt(run_time);
}

void loop(){
  for (int i = 0; i < 5; i++) {
    readings[i] = digitalRead(button[i]);
  }
  
  if (debounce(0) == 1){
    EEPROM.put(eeAddress, Clock);
    EEPROM.put(eeAddress + (1 * sizeof(Time)), Clock2);
    EEPROM.put(eeAddress + (2 * sizeof(Time)), Clock3);
    EEPROM.put(eeAddress + (3 * sizeof(Time)), ClockCD);
    EEPROM.put(eeAddress + (4 * sizeof(Time)), mode);
    EEPROM.put(eeAddress + (4 * sizeof(Time) + 1*sizeof(int)), modeStep[mode]);
    EEPROM.put(eeAddress + (4 * sizeof(Time) + 2*sizeof(int)), countdownTime);
    Serial.println("___________saved___________");
    Serial.println(String(Clock.hour) + ":" + String(Clock.minn) + ":" + String(Clock.sec));
    Serial.println("mode:" + String(mode) + "   modeStep:" + String(modeStep[mode]));
  }
  if ((debounce(1) == 1) && (mode == 0 || mode == 1)){ mode = 1; //alarm
    modeStep[mode] = (modeStep[mode] + 1 ) % modeRound[mode];
    if (modeStep[mode] == 4){ 
      save_Clock2_to_EEPROM(); 
    }
  }
  if ((debounce(2) == 1) && (mode == 0 || mode == 2)){ mode = 2; //countdown
    modeStep[mode] = (modeStep[mode] + 1 ) % modeRound[mode];
    if (modeStep[mode] == 2){
      save_Clock3_to_EEPROM();
    }
  }
  if ((debounce(3) == 1) && (mode == 0 || mode == 3)){ mode = 3; //setting
    modeStep[mode] = (modeStep[mode] + 1 ) % modeRound[mode];
    if (modeStep[mode] == 4){
      success_setting = millis();
    }
  }
  if (debounce(4) == 1){ mode = 0;
    cancel();
   }

  
// working in mode

  if (mode == 0){
    display_time(Clock);
  }
  if (mode == 1){
    alarm_mode();
  }
  if (mode == 2){
    countdown();
  }
  if (mode == 3){
    setting_mode();
  }
  
  if (beeping == 1){
      if (millis() - song_time < (period[song_index])){
          tone(BUZZER, note[song_index]);
      }
      else{
          noTone(BUZZER);
          song_time = millis();
          song_index = (song_index + 1) % 54;
      }
  }
  else if (beeping == 0)
  {
      noTone(BUZZER);
      song_index = 0;
  }
}
