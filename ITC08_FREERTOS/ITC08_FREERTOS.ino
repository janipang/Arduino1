#include <Arduino_FreeRTOS.h>
#include "queue.h"

#define SW_R 2
#define SW_Y 3
#define SW_G 4

#define LED_R 5
#define LED_Y 6
#define LED_G 7

int buttonState[3];
int lastButtonState[3] = {LOW, LOW, LOW};
unsigned long lastDebounceTime[3];
long debounceDelay = 50;

QueueHandle_t Queue_R; //create a queue variable,must be global
QueueHandle_t Queue_Y;
QueueHandle_t Queue_G;

const uint16_t time_delay[3] = {1,1,1};

void setup()
{
    Serial.begin(9600);
    Queue_R = xQueueCreate(5, sizeof(int32_t)); //give a queue property
    Queue_Y = xQueueCreate(5, sizeof(int32_t));
    Queue_G = xQueueCreate(5, sizeof(int32_t));

    xTaskCreate(sendInput, "Sender Task", 100, (void *)&time_delay, 1, NULL);

    xTaskCreate(RreceiveTask, "R Receiver Task", 100, (void *)&time_delay, 1, NULL);
    xTaskCreate(YreceiveTask, "Y Receiver Task", 100, (void *)&time_delay, 1, NULL);
    xTaskCreate(GreceiveTask, "G Receiver Task", 100, (void *)&time_delay, 1, NULL);
}

void sendInput(void *val){ //first I have 3 tasks for sending the input ,So i have total 6 tasks(send and receive) ,I think my Arduino cant handle it and... YES! It bug, that's why I have only 1 input task
    int *timer = val;
    int pressed[3] = {0,0,0};
    pinMode(SW_R, INPUT);
    pinMode(SW_Y, INPUT);
    pinMode(SW_G, INPUT);
    while(1){
        
        pressed[0] = debounce(0 , digitalRead(SW_R)); //send the reading value to debounce and it will return 0(dont pressed) or 1(pressed)
        xQueueSend(Queue_R, &pressed[0], 0); //send value to each queue
        
        pressed[1] = debounce(1 , digitalRead(SW_Y));
        xQueueSend(Queue_Y, &pressed[1], 0);
        
        pressed[2] = debounce(2 , digitalRead(SW_G));
        xQueueSend(Queue_G, &pressed[2], 0);
        
        vTaskDelay(time_delay[0]);
    }
}

int debounce(int button,int pressed) //steal debounce function from Gym
{
  if (pressed != lastButtonState[button])
  {
    lastDebounceTime[button] = millis();
  }
  if ((millis() - lastDebounceTime[button]) > debounceDelay)
  {
    if (pressed != buttonState[button])
    {
      buttonState[button] = pressed;

      if (buttonState[button] == HIGH)
      {
        return 1;
      }
    }
  }
  lastButtonState[button] = pressed;
  return 0;
}

//TURN ON FOR 3 SEC AFTER PRESS
void RreceiveTask(void *pvParameters){
    int pressed;
    uint64_t time_count;
    const TickType_t wait = pdMS_TO_TICKS(100); //what is this i dont know
    pinMode(LED_R, OUTPUT);
    while(1){
      xQueueReceive(Queue_R,&pressed,wait);
      if (pressed == 1){
        time_count = millis();
        Serial.println("red func ON");
      }
      if (millis() - time_count < 3000){
        digitalWrite(LED_R, !HIGH);
      }
      else{
        digitalWrite(LED_R, !LOW);
      }
      vTaskDelay(time_delay[0]);
    }
}

//BLINK UNTIL PRESS AGAIN
void YreceiveTask(void *pvParameters){
    int pressed;
    int blink_state = 0;
    uint32_t time_count; //i've tried to use uint64_t like others but it will turn on led 0.000001 sec and turn off so fast ,but 32_t is good
    const TickType_t wait = pdMS_TO_TICKS(100);
    pinMode(LED_Y, OUTPUT);
    while(1){
      xQueueReceive(Queue_Y,&pressed,wait);
      if (pressed == 1){
        time_count = millis();
        blink_state = !blink_state;
        Serial.println("yellow func ON");
      }
      if (blink_state == 1){
        if ((millis() - time_count) % 1000 < 500) {
          digitalWrite(LED_Y, !HIGH);
        }
        else {
          digitalWrite(LED_Y, !LOW);
        }
      }
      else{
          digitalWrite(LED_Y, !LOW);
      }
      vTaskDelay(time_delay[1]);
    }
}

//BLINK 3 TIMES
void GreceiveTask(void *pvParameters){ 
    int pressed;
    int blink_state = 0;
    uint64_t time_count;
    const TickType_t wait = pdMS_TO_TICKS(100);
    pinMode(LED_G, OUTPUT);
    while(1){
      xQueueReceive(Queue_G,&pressed,wait);
      if (pressed == 1 && blink_state == 0){
        time_count = millis();
        blink_state = 1;
        Serial.println("green func ON");
      }
      
      if (blink_state == 1 && millis() - time_count < 3000){
        if ((millis() - time_count) % 1000 < 500){
          digitalWrite(LED_G, !HIGH);
        }
        else{
          digitalWrite(LED_G, !LOW);
        }  
      }
      else{
        digitalWrite(LED_G, !LOW);
        blink_state = 0;
      }  
    
      vTaskDelay(time_delay[2]);
    }
}
void loop() {}
