#define LED_W_R 4
#define LED_W_Y 5
#define LED_W_G 6
#define WEST_BUTTON_PIN 2

#define LED_S_R 8
#define LED_S_Y 9
#define LED_S_G 10
#define SOUTH_BUTTON_PIN 3

#define LED_WALKING_R 11
#define LED_WALKING_G 12
#define WALKING_BUTTON_PIN 7

#define goSnoWalk 0
#define waitSnoWalk 1
#define goWnoWalk 2
#define waitWnoWalk 3
#define walk 4
#define stopWalk1 5
#define stopWalk2 6
#define stopWalk3 7
#define stopWalk4 8
#define stopWalk5 9
#define stopWalk6 10

struct State
{
    unsigned long ST_Out; // 8-bit pattern to street output
    unsigned long Time;   // delay in ms units
    unsigned long Next[8];
}; // next state for inputs 0,1,2,3,4,5,6,7

typedef const struct State SType;

// input: 000, 001, 010, 011, 100, 101, 110, 111 //walk South West
// output: B10001100 //walk South West
SType FSM[11] = {
    {B10001100, 3000, {goSnoWalk, waitSnoWalk, goSnoWalk, waitSnoWalk, waitSnoWalk, waitSnoWalk, waitSnoWalk, waitSnoWalk}},    //goSnoWalk
    {B10010100, 500, {goWnoWalk, goWnoWalk, goWnoWalk, goWnoWalk, walk, walk, walk, walk}},                                     //waitSnoWalk
    {B10100001, 3000, {goWnoWalk, goWnoWalk, waitWnoWalk, waitWnoWalk, waitWnoWalk, waitWnoWalk, waitWnoWalk, waitWnoWalk}},    //goWnoWalk
    {B10100010, 500, {goSnoWalk, goSnoWalk, goSnoWalk, goSnoWalk, walk, walk, walk, goSnoWalk}},                                //waitWnoWalk
    {B01100100, 3000, {walk, stopWalk1, stopWalk1, stopWalk1, walk, stopWalk1, stopWalk1, stopWalk1}},                          //walk
    {B00100100, 200, {stopWalk2, stopWalk2, stopWalk2, stopWalk2, stopWalk2, stopWalk2, stopWalk2, stopWalk2}}, //ดับ stopwalk1
    {B01100100, 200, {stopWalk3, stopWalk3, stopWalk3, stopWalk3, stopWalk3, stopWalk3, stopWalk3, stopWalk3}}, //ติด stopwalk2
    {B00100100, 200, {stopWalk4, stopWalk4, stopWalk4, stopWalk4, stopWalk4, stopWalk4, stopWalk4, stopWalk4}}, //ดับ stopwalk3
    {B01100100, 200, {stopWalk5, stopWalk5, stopWalk5, stopWalk5, stopWalk5, stopWalk5, stopWalk5, stopWalk5}}, //ติด stopwalk4
    {B00100100, 200, {stopWalk6, stopWalk6, stopWalk6, stopWalk6, stopWalk6, stopWalk6, stopWalk6, stopWalk6}}, //ดับ stopwalk5
    {B01100100, 200, {goSnoWalk, goWnoWalk, goSnoWalk, goWnoWalk, goSnoWalk, goWnoWalk, goSnoWalk, goWnoWalk}}}; //ติด stopwalk6

unsigned long S = 0; // index to the current state

void setup()
{
    Serial.begin(9600);
    pinMode(LED_W_R, OUTPUT);
    pinMode(LED_W_Y, OUTPUT);
    pinMode(LED_W_G, OUTPUT);
    pinMode(WEST_BUTTON_PIN, INPUT);

    pinMode(LED_S_R, OUTPUT);
    pinMode(LED_S_Y, OUTPUT);
    pinMode(LED_S_G, OUTPUT);
    pinMode(SOUTH_BUTTON_PIN, INPUT);

    pinMode(LED_WALKING_R, OUTPUT);
    pinMode(LED_WALKING_G, OUTPUT);
    pinMode(WALKING_BUTTON_PIN, INPUT);
}

int input, input1, input2, input3;

void loop()
{

    digitalWrite(LED_S_R, !(FSM[S].ST_Out & B00100000));
    digitalWrite(LED_S_Y, !(FSM[S].ST_Out & B00010000));
    digitalWrite(LED_S_G, !(FSM[S].ST_Out & B00001000));

    digitalWrite(LED_W_R, !(FSM[S].ST_Out & B00000100));
    digitalWrite(LED_W_Y, !(FSM[S].ST_Out & B00000010));
    digitalWrite(LED_W_G, !(FSM[S].ST_Out & B00000001));

    digitalWrite(LED_WALKING_R, !(FSM[S].ST_Out & B10000000));
    digitalWrite(LED_WALKING_G, !(FSM[S].ST_Out & B01000000));

    delay(FSM[S].Time);

    input1 = digitalRead(WEST_BUTTON_PIN);
    input2 = digitalRead(SOUTH_BUTTON_PIN);
    input3 = digitalRead(WALKING_BUTTON_PIN);

    Serial.print(input1);
    Serial.print(", ");
    Serial.print(input2);
    Serial.print(", ");
    Serial.println(input3);
    Serial.println("--WEST,SOUTH,WALK----");

    input = input3 * 4 + input2 * 2 + input1;
    S = FSM[S].Next[input];
    
}
