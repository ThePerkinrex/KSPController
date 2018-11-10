
//---------------------------------
// COMMS IDS
// OUT
#define digital 0 //0:<word(2 bytes)>
#define analog 1  //1:1023;10;0;219
#define modeUp 2
#define modeDown 3
#define startedUp 4

// IN
#define fuelLevels 0
#define lcdtxt 1
#define manouver 2
#define startingUp 3
#define connectionEnded 4
//-----------------------------------
// PINS
// DIGITAL
// I/O CUSTOM
#define MODE_CLOCK_PIN 2
#define MODE_DATA_PIN 3
#define LVL_DATA_PIN 4
#define LVL_LATCH_PIN 5
#define LVL_CLOCK_PIN 6
// LEDS (OUTPUT)
#define CONNECTED_LED_PIN 7
#define RCS_LED_PIN 8
#define SAS1_LED_PIN 9
#define SAS2_LED_PIN 10
#define MANOUVER_LED_PIN 11
// SWITCHES & BUTTONS (INPUT)
#define MODE_SW_PIN 12 //! Not used
#define XY_JOY_SW_PIN 22
#define Z_JOY_SW_PIN 23
// SWITCHES & BUTTONS (INPUT_PULLUP)
#define SAS_BTN_1_PIN 40
#define SAS_BTN_2_PIN 41
#define SAS_BTN_3_PIN 42
#define SAS_BTN_4_PIN 43
#define SAS_BTN_5_PIN 44
#define SAS_BTN_6_PIN 45
#define SAS_BTN_7_PIN 46
#define SAS_BTN_8_PIN 47
#define STAGE_PIN 48
#define LIGHTS_PIN 49
#define GEAR_PIN 50
#define BRAKES_PIN 51
#define SAS_SW_PIN 52
#define RCS_SW_PIN 53
// ------------------------------
// ANALOG
#define THROTTLE_PIN A0
#define X_JOY_PIN A1
#define Y_JOY_PIN A2
#define Z_JOY_PIN A3
#define P_JOY_PIN A4
// CONSTANTS
#define ANALOG_VALS_LEN 5

#include <BitbloqLiquidCrystal.h>

LiquidCrystal lcd(0);
byte modeState = 0;
// 0 -> NOP
// 1 -> UP
// 2 -> DOWN


void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.clear();
  // PIN MODES
  pinMode(MODE_CLOCK_PIN,INPUT_PULLUP);
  pinMode(MODE_DATA_PIN,INPUT_PULLUP);

  pinMode(LVL_DATA_PIN, OUTPUT);
  pinMode(LVL_CLOCK_PIN, OUTPUT);
  pinMode(LVL_LATCH_PIN, OUTPUT);

  pinMode(CONNECTED_LED_PIN, OUTPUT);
  pinMode(RCS_LED_PIN, OUTPUT);
  pinMode(SAS1_LED_PIN, OUTPUT);
  pinMode(SAS2_LED_PIN, OUTPUT);
  pinMode(MANOUVER_LED_PIN, OUTPUT);

  pinMode(MODE_SW_PIN, INPUT);
  pinMode(XY_JOY_SW_PIN, INPUT);
  pinMode(Z_JOY_SW_PIN, INPUT);

  pinMode(SAS_BTN_1_PIN, INPUT_PULLUP); // Stability assist
  pinMode(SAS_BTN_2_PIN, INPUT_PULLUP); // Manouver
  pinMode(SAS_BTN_3_PIN, INPUT_PULLUP); // Prograde
  pinMode(SAS_BTN_4_PIN, INPUT_PULLUP); // Retrograde
  pinMode(SAS_BTN_5_PIN, INPUT_PULLUP); // Normal
  pinMode(SAS_BTN_6_PIN, INPUT_PULLUP); // Anti-normal
  pinMode(SAS_BTN_7_PIN, INPUT_PULLUP); // Radial
  pinMode(SAS_BTN_8_PIN, INPUT_PULLUP); // Anti-radial
  pinMode(STAGE_PIN, INPUT_PULLUP);     // Stage
  pinMode(LIGHTS_PIN, INPUT_PULLUP);    // Ligjts
  pinMode(GEAR_PIN, INPUT_PULLUP);      // Gear
  pinMode(BRAKES_PIN, INPUT_PULLUP);    // Brakes
  pinMode(SAS_SW_PIN, INPUT_PULLUP);    // SAS
  pinMode(RCS_SW_PIN, INPUT_PULLUP);    // RCS
  // PIN SETUPS
  digitalWrite(CONNECTED_LED_PIN, LOW);
  digitalWrite(LVL_CLOCK_PIN, LOW);
  digitalWrite(LVL_LATCH_PIN, LOW);
  // INTERRUPTS
  attachInterrupt(digitalPinToInterrupt(MODE_CLOCK_PIN), doEncode, CHANGE);
}

String tok = "";
int commNtmp = -1;
String argvtmp[5];
int argctmp=0;
void loop() {
  if(Serial.available()>0){
    char readChar = Serial.read();
    if(readChar == '\n'){
      argvtmp[argctmp] = tok;
      argctmp++;
      tok = "";
      handleCommand(commNtmp, argctmp, argvtmp);
      commNtmp=-1;
      argctmp=0;
    }else{
      if(readChar != ':' && readChar != ';'){
        if(commNtmp == -1){
          commNtmp = int(readChar)-48;
        }else{
          tok.concat(readChar);
        }
      }else if(readChar == ';'){
        argvtmp[argctmp] = tok;
        argctmp++;
        tok = "";
      }
    }
  }

  sendData();
}

void handleCommand(int commandN, int argc, String argv[]){
  if(commandN==startingUp){
    digitalWrite(CONNECTED_LED_PIN, HIGH);
  }else if(commandN==connectionEnded){
    digitalWrite(CONNECTED_LED_PIN, LOW);
    lcd.clear();
    levelShiftOut(0,0,0,0,0);
  }else if(commandN==lcdtxt&&argc==2){
    lcd.clear();
    lcd.p(argv[0].c_str());
    lcd.setCursor(0,1);
    lcd.p(argv[1].c_str());
  }else if(commandN==fuelLevels&&argc==5){
    levelShiftOut(byte(argv[0].toInt()),byte(argv[1].toInt()),byte(argv[2].toInt()),byte(argv[3].toInt()),byte(argv[4].toInt()));
  }else if(commandN==manouver&&argc==1){
    digitalWrite(MANOUVER_LED_PIN, argv[0]);
  }
}

word getLastDigitalValues() {
  word r = 0;
  bitWrite(r, 0, digitalRead(SAS_BTN_1_PIN));
  bitWrite(r, 1, digitalRead(SAS_BTN_2_PIN));
  bitWrite(r, 2, digitalRead(SAS_BTN_3_PIN));
  bitWrite(r, 3, digitalRead(SAS_BTN_4_PIN));
  bitWrite(r, 4, digitalRead(SAS_BTN_5_PIN));
  bitWrite(r, 5, digitalRead(SAS_BTN_6_PIN));
  bitWrite(r, 0, digitalRead(SAS_BTN_7_PIN));
  bitWrite(r, 7, digitalRead(SAS_BTN_8_PIN));
  bitWrite(r, 8, digitalRead(STAGE_PIN));
  bitWrite(r, 9, digitalRead(LIGHTS_PIN));
  bitWrite(r, 10, digitalRead(GEAR_PIN));
  bitWrite(r, 11, digitalRead(BRAKES_PIN));
  bitWrite(r, 12, digitalRead(SAS_SW_PIN));
  bitWrite(r, 13, digitalRead(RCS_SW_PIN));

  bitWrite(r, 14, digitalRead(XY_JOY_SW_PIN));
  bitWrite(r, 15, digitalRead(Z_JOY_SW_PIN));

  return r;
}

int * getLastAnalogValues() {
  int r[5];
  r[0] = analogRead(THROTTLE_PIN);
  r[1] = analogRead(X_JOY_PIN);
  r[2] = analogRead(Y_JOY_PIN);
  r[3] = analogRead(Z_JOY_PIN);
  r[4] = analogRead(P_JOY_PIN); //! Functionality not defined
  return r;
}

int * lastAnalogValues;
word lastDigitalValues = 0;

void sendData(){
  if(modeState == 1){
    Serial.println(modeUp);
  }else if(modeState == 2){
    Serial.println(modeDown);
  }
  word digitalValues = getLastDigitalValues();
  if(lastDigitalValues != digitalValues){
    Serial.print(digital);
    Serial.print(':');
    Serial.println(digitalValues);
  }
  lastDigitalValues = digitalValues;
  int * analogValues;
  analogValues = getLastAnalogValues();
  if(lastAnalogValues != analogValues){
    Serial.print(analog);
    Serial.print(':');
    for(int i = 0; i<ANALOG_VALS_LEN; i++){
      Serial.print(analogValues[i]);
      if(i!=ANALOG_VALS_LEN-1){
        Serial.print(';');
      }
    }
    Serial.println();
  }
  lastAnalogValues = analogValues;
}

void levelShiftOut(byte lf, byte ox, byte sf, byte mp, byte ec){
  short levels[5];
  levels[0] = short(round(pow(2, constrain(ec, 0, 10))-1)); // First level
  levels[1] = short(round(pow(2, constrain(mp, 0, 10))-1)); // Second level
  levels[2] = short(round(pow(2, constrain(sf, 0, 10))-1)); // Third level
  levels[3] = short(round(pow(2, constrain(ox, 0, 10))-1)); // Fourth level
  levels[4] = short(round(pow(2, constrain(lf, 0, 10))-1)); // Fifth level
  for(int i=0;i<5;i++){ // For each level
    for(int j=9;j>=0;j--){ // For each bit of the 10 bits of each level
      digitalWrite(LVL_DATA_PIN, bitRead(levels[i], j)); // Write the bit
      digitalWrite(LVL_CLOCK_PIN, HIGH); // Pulse the clock
      delay(1);
      digitalWrite(LVL_CLOCK_PIN, LOW);
    }
  }
  digitalWrite(LVL_LATCH_PIN, HIGH); // Latch the data
  delay(1);
  digitalWrite(LVL_LATCH_PIN, LOW);
}


const int timeThreshold = 5;
long timeCounter = 0;

void doEncode(){
   if (millis() > timeCounter + timeThreshold){
      if (digitalRead(MODE_CLOCK_PIN) == digitalRead(MODE_DATA_PIN)){
        modeState = 1;
      }else{
        modeState = 2;
      }
      timeCounter = millis();
   }
}
