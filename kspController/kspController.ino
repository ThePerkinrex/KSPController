// OUT
#define digital 0
#define analog 1
#define modeUp 2
#define modeDown 3
#define startedUp 4

// IN
#define fuelLevels 0
#define lcdtxt 1
#define manouver 2
#define startingUp 3
#define connectionEnded 4

// PINS
#define DATA_PIN 2
#define LATCH_PIN 3
#define CLOCK_PIN 4
#define CONNECTED_LED_PIN 5

#include "BitbloqLiquidCrystal.h"

LiquidCrystal lcd(0);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(16, 2); //inicializamos el LCD, 16 columnas, 2 filas
  lcd.clear(); //borramos cualquier contenido residual
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CONNECTED_LED_PIN, OUTPUT);
  digitalWrite(CONNECTED_LED_PIN, LOW);
  digitalWrite(CLOCK_PIN, LOW);
  digitalWrite(LATCH_PIN, LOW);
  Serial.println(startedUp);
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
  Serial.print(commandN);
  Serial.print(";");
  Serial.print(argc);
  Serial.print(";[");
  for(int i=0;i<argc;i++){
    Serial.print(argv[i]);
    if (i!=argc-1){
      Serial.print(",");
    }
  }
  Serial.println("]");
  if(commandN==startingUp){
    digitalWrite(CONNECTED_LED_PIN, HIGH);
  }
  if(commandN==connectionEnded){
    digitalWrite(CONNECTED_LED_PIN, LOW);
    lcd.clear();
    levelShiftOut(0,0,0,0,0);
  }


  if(commandN==lcdtxt&&argc==2){
    lcd.clear();
    lcd.p(argv[0].c_str());
    lcd.setCursor(0,1);
    lcd.p(argv[1].c_str());
  }else if(commandN==fuelLevels&&argc==5){
    levelShiftOut(byte(argv[0].toInt()),byte(argv[1].toInt()),byte(argv[2].toInt()),byte(argv[3].toInt()),byte(argv[4].toInt()));
  }
}

void sendData(){}

void levelShiftOut(byte lf, byte ox, byte sf, byte mp, byte ec){
  short levels[5];
  levels[0] = short(round(pow(2, constrain(ec, 0, 10))-1)); // First level
  levels[1] = short(round(pow(2, constrain(mp, 0, 10))-1)); // Second level
  levels[2] = short(round(pow(2, constrain(sf, 0, 10))-1)); // Third level
  levels[3] = short(round(pow(2, constrain(ox, 0, 10))-1)); // Fourth level
  levels[4] = short(round(pow(2, constrain(lf, 0, 10))-1)); // Fifth level
  for(int i=0;i<5;i++){ // For each level
    Serial.print("Level "); Serial.println(i);
    for(int j=9;j>=0;j--){ // For each bit of the 10 bits of each level
      Serial.print(bitRead(levels[i], j));
      digitalWrite(DATA_PIN, bitRead(levels[i], j)); // Write the bit
      digitalWrite(CLOCK_PIN, HIGH); // Pulse the clock
      delay(1);
      digitalWrite(CLOCK_PIN, LOW);
    }
    Serial.println();
  }
  digitalWrite(LATCH_PIN, HIGH); // Latch the data
  delay(1);
  digitalWrite(LATCH_PIN, LOW);
}

