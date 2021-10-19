#include <Arduino.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
// Adafruit_NeoPixel pixels(8, A0, NEO_GRB + NEO_KHZ800);

//relays
const byte RELAYS[] = {8,9,10,11,12};

//buttons
const byte BTT_TOP = 2;
const byte BTT_LEFT = 4;
const byte BTT_RIGHT = 5;
const byte BTT_CENTER = 6;
const byte BTT_BOTTOM = 3;
const byte CLOSURE_SENSOR = 0;

//DHT22
const byte DHT_PIN = 7;
#define DHTTYPE DHT11
DHT dht(DHT_PIN, DHTTYPE);

//wateringSensors
const int sensors[] = {A1,A2,A3,A4,A5};
int sensorValues[] = {0,0,0,0,0};

const byte addressesINT[] = {0,4,8,12,16}; 
const byte addressesWAT[] = {20,24,28,32,36};
const byte addressesWORK[] = {40,41,42,43,44};
const byte addressesOPT[] = {45,46,47,48,49};

//test
bool waterLevel = true;

bool waterContainerOpen = false;
bool plants[] = {false, false, false, true, true};
bool enableWatering = false;
bool firstRun = true;
bool firstLoop[] = {true,true,true,true,true};
bool isMenuOpen = false;
bool isIntervalMenuOpen = false;
bool isWateringMenuOpen = false;
bool isOptionsMenuOpen = false;
bool relayToOpen[] = {false,false,false,false,false};
bool workingRelay[] = {true,true,true,true,true,true};
bool firstTimeLoop = false;

//settings
bool isLedOn = true;
bool resetTime = true;
bool soundEnable = true;
byte workMode = 0;
unsigned long screenLightTime = 20000;


unsigned long buttonPushedMillis;
unsigned long wateringTimeMillis[] = {0,0,0,0,0};
unsigned long longestInterval;
unsigned long firstRealySavedTime = 0;
unsigned long secondRealySavedTime = 0;
unsigned long intervals[] = {5000, 2000, 3000, 10000, 800};
unsigned long wateringTimes[] = {10000, 12000, 8000, 6000, 6000};
unsigned long savedTime[] = {0,0,0,0,0};

int counter;
int menuPosition = 0;
int intervalsMenuPosition = 0;
int wateringMenuPosition = 0;
int optionsMenuPosition = 0;

String errors[] = {};

String calculateTime(unsigned long number, byte i) {
 
  //godzina - 3 600 000
  //12h - 43 200 000
  //24h - 86 400 000
  //48h - 172 800 000
  //96h - 345 600 000
  //168h - tydzien - 604 800 000
  // ul max -  4,294,967,295

  String convertedTime;
  if (workingRelay[i]){
    
    if(number < 60000){
      double newVal = double(number);
      newVal = newVal / 1000;
      convertedTime = String(newVal, DEC);
      return convertedTime.substring(0,4) + "s";
    }
    else if(number < 3600000){
      number = number / 60000;
      return convertedTime = String(number, DEC) + "min";
    } else if (number < 86400000){
      number = number / 3600000;
      return convertedTime = String(number, DEC) + "h";
    } else {
      number = number / 86400000;
      return convertedTime = String(number, DEC) + " dni";
    }
  } else {
    return "X";
  }
}

String showWorkMode() {
    if (workMode == 0){
    return "reczny";
  } else if(workMode == 1) {
    return "automat";
  } else if (workMode == 2){
    return "czasowy";
  } else {
    return "blad";
  }
}

String ifTrueSaidOn(bool value){
  if (value){
    return "tak";
  } else {
    return "nie";
  }
}

void lcdPrint(int cursor, int line, String meessage){
  lcd.setCursor(cursor, line);
  lcd.print(meessage);
}

void showMenu() {
  isMenuOpen = true;
  lcd.clear();

  switch (menuPosition)
  {
  case 8:
  case 0: 
    lcdPrint(0,0, "-> Podlej wszystkie");
    lcdPrint(0,1, "Podlej suche");
    lcdPrint(0,2, "Ustaw interwaly");
    lcdPrint(0,3, "Ustaw automat");
    break;
  case 1:
    lcdPrint(0,0,"-> Podlej suche");
    lcdPrint(0,1,"Ustaw interwaly");
    lcdPrint(0,2,"Ustaw automat");
    lcdPrint(0,3,"Testuj");
    break;
  case 2:
    lcdPrint(0,0,"-> Ustaw interwaly");
    lcdPrint(0,1,"Ustaw automat");
    lcdPrint(0,2,"Testuj");
    lcdPrint(0,3,"Wilgotnosc ziemi");
    break;
  case 3:
    lcdPrint(0,0,"-> Ustaw automat");
    lcdPrint(0,1,"Testuj");
    lcdPrint(0,2,"Wilgotnosc ziemi");
    lcdPrint(0,3,"Ustawienia");
    break;
  case 4:
    lcdPrint(0,0,"-> Testuj");
    lcdPrint(0,1,"Wilgotnosc ziemi");
    lcdPrint(0,2,"Ustawienia");
    lcdPrint(0,3,"Cofnij");
    break;
  case 5:
    lcdPrint(0,0,"-> Wilgotnosc ziemi");
    lcdPrint(0,1,"Ustawienia");
    lcdPrint(0,2,"Cofnij");
    lcdPrint(0,3,"Podlej wszystkie");
    break;
  case 6:
    lcdPrint(0,0,"-> Ustawienia");
    lcdPrint(0,1,"Cofnij");
    lcdPrint(0,2,"Podlej wszystkie");
    lcdPrint(0,3,"Podlej suche");
    break;
  case -1:
  case 7:
    lcdPrint(0,0,"-> Cofnij");
    lcdPrint(0,1,"Podlej wszystkie");
    lcdPrint(0,2,"Podlej suche");
    lcdPrint(0,3,"Ustaw interwaly");
    break;

  default:
    Serial.println("Błąd");
    break;
  }
}

//do poprawy
void intervalsMenu() {
  isIntervalMenuOpen = true;
  
  lcd.clear();
  lcd.setCursor(0,0);

  switch (intervalsMenuPosition)
  {
  case 7:
  case 0: 
    lcd.print("-> Roslina 1: " + calculateTime(intervals[0],0));
    lcd.setCursor(0,1);
    lcd.print("Roslina 2: " + calculateTime(intervals[1],1));
    lcd.setCursor(0,2);
    lcd.print("Roslina 3: " + calculateTime(intervals[2],2));
    lcd.setCursor(0,3);
    lcd.print("Roslina 4: " + calculateTime(intervals[3],3));
    break;
  case 1: 
    lcd.print("-> Roslina 2: " + calculateTime(intervals[1],1));
    lcd.setCursor(0,1);
    lcd.print("Roslina 3: " + calculateTime(intervals[2],2));
    lcd.setCursor(0,2);
    lcd.print("Roslina 4: " + calculateTime(intervals[3],3));
    lcd.setCursor(0,3);
    lcd.print("Roslina 5: " + calculateTime(intervals[4],4));
    break;
  case 2: 
    lcd.print("-> Roslina 3: " + calculateTime(intervals[2],2));
    lcd.setCursor(0,1);
    lcd.print("Roslina 4: " + calculateTime(intervals[3],3));
    lcd.setCursor(0,2);
    lcd.print("Roslina 5: " + calculateTime(intervals[4],4));
    lcd.setCursor(0,3);
    lcd.print("Zapisz");
    break;
  case 3: 
    lcd.print("-> Roslina 4: " + calculateTime(intervals[3],3));
    lcd.setCursor(0,1);
    lcd.print("Roslina 5: " + calculateTime(intervals[4],4));
    lcd.setCursor(0,2);
    lcd.print("Zapisz");
    lcd.setCursor(0,3);
    lcd.print("Cofnij");
    break;
  case 4: 
    lcd.print("-> Roslina 5: " + calculateTime(intervals[4],4));
    lcd.setCursor(0,1);
    lcd.print("Zapisz");
    lcd.setCursor(0,2);
    lcd.print("Cofnij");
    lcd.setCursor(0,3);
    lcd.print("Roslina 1: " + calculateTime(intervals[0],0));
    break;
  case 5: 
    lcd.print("-> Zapisz");
    lcd.setCursor(0,1);
    lcd.print("Cofnij");
    lcd.setCursor(0,2);
    lcd.print("Roslina 1: " + calculateTime(intervals[0],0));
    lcd.setCursor(0,3);
    lcd.print("Roslina 2: " + calculateTime(intervals[1],1));
    break;
  case -1:
  case 6: 
    lcd.print("-> Cofinij");
    lcd.setCursor(0,1);
    lcd.print("Roslina 1: " + calculateTime(intervals[0],0));
    lcd.setCursor(0,2);
    lcd.print("Roslina 2: " + calculateTime(intervals[1],1));
    lcd.setCursor(0,3);
    lcd.print("Roslina 3: " + calculateTime(intervals[2],2));
    break;
  default:
    Serial.println("Błąd");
    break;
  }
}

void optionsMenu() {
  isOptionsMenuOpen = true;
  lcd.clear();

  switch (optionsMenuPosition)
  {
  case 7:
  case 0:
    lcdPrint(0,0, "-> Tryb: " + showWorkMode());
    lcdPrint(0,1, "Dzwiek: " + ifTrueSaidOn(soundEnable));
    lcdPrint(0,2, "LED: " + ifTrueSaidOn(isLedOn));
    lcdPrint(0,3, "Resetowanie: " + ifTrueSaidOn(resetTime));
    break;
  
  case 1:
    lcdPrint(0,0, "-> Dzwiek: " + ifTrueSaidOn(soundEnable));
    lcdPrint(0,1, "LED: " + ifTrueSaidOn(isLedOn));
    lcdPrint(0,2, "Resetowanie: " + ifTrueSaidOn(resetTime));
    lcdPrint(0,3, "Ekran: " + calculateTime(screenLightTime, workingRelay[5]));
    break;
  
  case 2:
    lcdPrint(0,0, "-> LED: " + ifTrueSaidOn(isLedOn));
    lcdPrint(0,1, "Resetowanie: " + ifTrueSaidOn(resetTime));
    lcdPrint(0,2, "Ekran: " + calculateTime(screenLightTime, workingRelay[5]));
    lcdPrint(0,3, "Zapisz");
    break;
  
  case 3:
    lcdPrint(0,0, "-> Resetowanie: " + ifTrueSaidOn(resetTime));
    lcdPrint(0,1, "Ekran: " + calculateTime(screenLightTime, workingRelay[5]));
    lcdPrint(0,2, "Zapisz");
    lcdPrint(0,3, "Cofnij");
    break;
  
  case 4:
    lcdPrint(0,0, "-> Ekran: " + calculateTime(screenLightTime, workingRelay[5]));
    lcdPrint(0,1, "Zapisz");
    lcdPrint(0,2, "Cofnij");
    lcdPrint(0,3, "Tryb: " + showWorkMode());
    break;
  case 5:
    lcdPrint(0,0, "-> Zapisz");
    lcdPrint(0,1, "Cofnij");
    lcdPrint(0,2, "Tryb: " + showWorkMode());
    lcdPrint(0,3, "Dzwiek: " + ifTrueSaidOn(soundEnable));
    break;
  case -1:
  case 6:
    lcdPrint(0,0, "-> Cofnij");
    lcdPrint(0,1, "Tryb: " + showWorkMode());
    lcdPrint(0,2, "Dzwiek: " + ifTrueSaidOn(soundEnable));
    lcdPrint(0,3, "LED: " + ifTrueSaidOn(isLedOn));
    break;
  default:
    lcdPrint(0,1, "Blad x1");
    break;
  }
}

//do poprawy
void wateringTime() {
  isWateringMenuOpen = true;
  
  lcd.clear();
  lcd.setCursor(0,0);

  switch (wateringMenuPosition)
  {
  case 7:
  case 0: 
    lcd.print("-> Roslina 1: " + calculateTime(wateringTimes[0],0));
    lcd.setCursor(0,1);
    lcd.print("Roslina 2: " + calculateTime(wateringTimes[1],1));
    lcd.setCursor(0,2);
    lcd.print("Roslina 3: " + calculateTime(wateringTimes[2],2));
    lcd.setCursor(0,3);
    lcd.print("Roslina 4: " + calculateTime(wateringTimes[3],3));
    break;
  case 1: 
    lcd.print("-> Roslina 2: " + calculateTime(wateringTimes[1],1));
    lcd.setCursor(0,1);
    lcd.print("Roslina 3: " + calculateTime(wateringTimes[2],2));
    lcd.setCursor(0,2);
    lcd.print("Roslina 4: " + calculateTime(wateringTimes[3],3));
    lcd.setCursor(0,3);
    lcd.print("Roslina 5: " + calculateTime(wateringTimes[4],4));
    break;
  case 2: 
    lcd.print("-> Roslina 3: " + calculateTime(wateringTimes[2],2));
    lcd.setCursor(0,1);
    lcd.print("Roslina 4: " + calculateTime(wateringTimes[3],3));
    lcd.setCursor(0,2);
    lcd.print("Roslina 5: " + calculateTime(wateringTimes[4],4));
    lcd.setCursor(0,3);
    lcd.print("Zapisz");
    break;
  case 3: 
    lcd.print("-> Roslina 4: " + calculateTime(wateringTimes[3],3));
    lcd.setCursor(0,1);
    lcd.print("Roslina 5: " + calculateTime(wateringTimes[4],4));
    lcd.setCursor(0,2);
    lcd.print("Zapisz");
    lcd.setCursor(0,3);
    lcd.print("Cofnij");
    break;
  case 4: 
    lcd.print("-> Roslina 5: " + calculateTime(wateringTimes[4],4));
    lcd.setCursor(0,1);
    lcd.print("Zapisz");
    lcd.setCursor(0,2);
    lcd.print("Cofnij");
    lcd.setCursor(0,3);
    lcd.print("Roslina 1: " + calculateTime(wateringTimes[0],0));
    break;
  case 5: 
    lcd.print("-> Zapisz");
    lcd.setCursor(0,1);
    lcd.print("Cofnij");
    lcd.setCursor(0,2);
    lcd.print("Roslina 1: " + calculateTime(wateringTimes[0],0));
    lcd.setCursor(0,3);
    lcd.print("Roslina 2: " + calculateTime(wateringTimes[1],1));
    break;
  case -1:
  case 6: 
    lcd.print("-> Cofinij");
    lcd.setCursor(0,1);
    lcd.print("Roslina 1: " + calculateTime(wateringTimes[0],0));
    lcd.setCursor(0,2);
    lcd.print("Roslina 2: " + calculateTime(wateringTimes[1],1));
    lcd.setCursor(0,3);
    lcd.print("Roslina 3: " + calculateTime(wateringTimes[2],2));
    break;
  default:
    Serial.println("Błąd");
    break;
  }
}
 
String waterLevelMsg() {
  if (waterLevel) {
    return "OK";
  } else {
    return "PUSTO!";
  }
}

void showStartMenu() {
  lcd.clear();
  lcdPrint(0,0,"Poziom wody: " + waterLevelMsg());

  lcdPrint(0,1,"Do podlania: ");

  counter = 0;
  for(byte i = 0; i <= 4; i++) {
    if (!plants[i]) {
      counter++;
    }
  }
  lcd.print(counter);

  lcdPrint(0,2,"Tryb: " + showWorkMode());
  lcdPrint(0,3,"Otworz menu -> ");
  lcdPrint(16,3,"(OK)");

}

unsigned long readValueFromEEPROM(int address) {
  unsigned long readedData = 0;
  EEPROM.get(address, readedData);
  return readedData;
}

bool readValueFromEEPROMBool(int address) {
  bool readedData ;
  EEPROM.get(address, readedData);
  return readedData;
}

void checkContainerMessage() {
 unsigned long currentMillis = millis();
  enableWatering = true;

  while (firstRun)
  {
    buttonPushedMillis = currentMillis;

    if(waterContainerOpen && !waterLevel) {
        lcd.clear();
        lcdPrint(0,1,"Zamknij zbiornik!");
        lcdPrint(0,2,"Uzupelnij wode!");
    } else if (waterContainerOpen){
      lcdPrint(0,2,"Zamknij zbiornik!");
    } else {
      lcdPrint(0,2,"Uzupelnij wode!");
    }

    firstRun = false;
  }

  if (currentMillis - buttonPushedMillis >= 2500) {
    enableWatering = false;
    firstRun = true;
    showMenu();
  }
}

//spróbować z przewijaniem komunikatu
void thereIsNoPlants() {
  unsigned long currentMillis = millis();
  enableWatering = true;

  while (firstRun)
  {
    buttonPushedMillis = currentMillis;

    lcd.clear();
    lcdPrint(0,1,"Rosliny sa juz");
    lcdPrint(0,2,"podlane");

    firstRun = false;
  }

  if (currentMillis - buttonPushedMillis >= 2000) {
    Serial.println("Poka menu");
    enableWatering = false;
    firstRun = true;
    showMenu();
  }
}

void openRelay(bool waterAll) {
  unsigned long currentMillis = millis();
  enableWatering = true;

  while(firstRun) {
    buttonPushedMillis = currentMillis;

    for(byte i = 0; i < 5; i++){
      if (longestInterval < intervals[i]){
        if(workingRelay[i]){
          longestInterval = intervals[i];
        }
      }
    }

    for(byte i = 0; i < 5; i++){
      if (!plants[i] || waterAll) {
        if (workingRelay[i]){
          digitalWrite(RELAYS[i], HIGH);
        }
      }
    } 
  
    lcd.clear();
    lcdPrint(0,1, "OTWIERAM ZAWORY");
    lcdPrint(0,2, "Podlewanie w toku");

    firstRun = false;
  }


  //Closing relays 
  for(byte i = 0; i < 5; i++){
    if (currentMillis - buttonPushedMillis >= intervals[i]) {
    plants[i] = true;
    digitalWrite(RELAYS[i], LOW);
  }    
  }

  if (currentMillis - buttonPushedMillis >= longestInterval) {
    Serial.println("Zamykam");
    enableWatering = false;
    firstRun = true;
    isMenuOpen = false;
    menuPosition = 0;
    showStartMenu();
  }
}

void openSingleRelay(byte num) {
  unsigned long currentMillis = millis();
  Serial.println("OTW: " + String(num+1));
  digitalWrite(RELAYS[num], HIGH);
  relayToOpen[num] = true;
  wateringTimeMillis[num] = currentMillis;
}

void closeSingleRelay() {
  unsigned long currentMillis = millis();

  for(byte i = 0; i < 5; i++){
    if (relayToOpen[i]) {
      if(currentMillis - wateringTimeMillis[i] >= intervals[i]){
        Serial.println("ZAM " + String(i+1));
        Serial.println("Inte: " + String(intervals[i]/1000) + "s");
        digitalWrite(RELAYS[i], LOW);
        plants[i] = true;
        relayToOpen[i] = false;

        if(!isMenuOpen && !isIntervalMenuOpen && !isWateringMenuOpen){
          showStartMenu();
        }
      }
    } 
  }
}

void resetMillis(unsigned long globalMillis) {
  if(firstTimeLoop){
    for(byte i = 0; i < 5; i++){ 
      savedTime[i] = globalMillis; 
    }
    firstTimeLoop = false;
  }

}

void readAllDataFromEEPROM() {
  Serial.println("Wczytuje z EEPROM: ");

  // for(byte i = 0; i < 5; i++){
  //   intervals[i] = readValueFromEEPROM(addressesINT[i]);
  // };

  // for(byte i = 0; i < 5; i++){
  //   wateringTimes[i] = readValueFromEEPROM(addressesWAT[i]);
  // };

  // for(byte i = 0; i < 5; i++){
  //   workingRelay[i] = readValueFromEEPROMBool(addressesWORK[i]);
  // };

  // workMode = readValueFromEEPROMBool(addressesOPT[0]);
  soundEnable =  readValueFromEEPROMBool(addressesOPT[1]);
  isLedOn = readValueFromEEPROMBool(addressesOPT[2]);
  resetTime = readValueFromEEPROMBool(addressesOPT[3]); 
  screenLightTime = readValueFromEEPROM(addressesOPT[4]);

  Serial.println("Wczytano");
}

void setup() {
  lcd.init(); 
  lcd.backlight();
  dht.begin();
  // pixels.begin();
  // pixels.show();

  //initialization
  Serial.begin(9600);
  pinMode(BTT_TOP, INPUT_PULLUP);
  pinMode(BTT_LEFT, INPUT_PULLUP);
  pinMode(BTT_RIGHT, INPUT_PULLUP);
  pinMode(BTT_CENTER, INPUT_PULLUP);
  pinMode(BTT_BOTTOM, INPUT_PULLUP);

  pinMode(CLOSURE_SENSOR, INPUT_PULLUP);
  
  pinMode(RELAYS[0], OUTPUT);
  pinMode(RELAYS[1], OUTPUT);
  pinMode(RELAYS[2], OUTPUT);
  pinMode(RELAYS[3], OUTPUT);
  pinMode(RELAYS[4], OUTPUT);


  Serial.println("Board connected");

  readAllDataFromEEPROM();
  showStartMenu();
}

void loop() {  
  unsigned long globalMillis = millis();;

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

delay(2000);
  Serial.println(humidity);
  Serial.println(temperature);

  if(workMode == 2 ) {

    resetMillis(globalMillis);
 
    for(byte i = 0; i < 5; i++){
      if (globalMillis - savedTime[i] >= wateringTimes[i] && workingRelay[i]) {
        savedTime[i] = globalMillis;
        openSingleRelay(i);
      }    
    }
  }
  
  if(digitalRead(CLOSURE_SENSOR) == LOW){
    waterContainerOpen = false;
  } else {
    waterContainerOpen = true;
  }

  closeSingleRelay();

  sensorValues[4] = analogRead(sensors[4]);
  delay(150);
  Serial.print("Roślina 1: ");
  Serial.println(sensorValues[4]);


  int btt_state_top = digitalRead(BTT_TOP); 
  int btt_state_left = digitalRead(BTT_LEFT);
  int btt_state_right = digitalRead(BTT_RIGHT);
  int btt_state_center = digitalRead(BTT_CENTER);
  int btt_state_bottom = digitalRead(BTT_BOTTOM);

  //W góre
  if (btt_state_top == LOW) {
    if (isMenuOpen)
    {
      if(menuPosition < 0){
      menuPosition = 7;
      showMenu();
      } else {
        --menuPosition;
        delay(200L); 
        showMenu();
      }
    }
    if (isIntervalMenuOpen)
    {
      if(intervalsMenuPosition < 0){
      intervalsMenuPosition = 6;
      intervalsMenu();
      } else {
        --intervalsMenuPosition;
        delay(200L); 
        intervalsMenu();
      }
    }
    if (isWateringMenuOpen)
    {
      if(wateringMenuPosition < 0){
      wateringMenuPosition = 6;
      wateringTime();
      } else {
        --wateringMenuPosition;
        delay(200L); 
        wateringTime();
      }
    }
    if(isOptionsMenuOpen){
      if(optionsMenuPosition < 0){
      optionsMenuPosition = 6;
      optionsMenu();
      } else {
        --optionsMenuPosition;
        delay(200L); 
        optionsMenu();
      }
    }
    
  }
  
  //W dół
  if (btt_state_bottom == LOW) {
    if(isMenuOpen) {
      if(menuPosition > 7){
      menuPosition = 0;
      showMenu();
      } else {
        ++menuPosition;
        delay(200L); 
        showMenu();
      }
    }
    if(isIntervalMenuOpen) {
      if(intervalsMenuPosition > 6){
      intervalsMenuPosition = 0;
      intervalsMenu();
      } else {
        ++intervalsMenuPosition;
        delay(200L); 
        intervalsMenu();
      }
    } 
    if (isWateringMenuOpen)
    {
      if(wateringMenuPosition > 6){
      wateringMenuPosition = 0;
      wateringTime();
      } else {
        ++wateringMenuPosition;
        delay(200L); 
        wateringTime();
      }
    }
    if(isOptionsMenuOpen){
      if(optionsMenuPosition > 6){
      optionsMenuPosition = 0;
      optionsMenu();
      } else {
        ++optionsMenuPosition;
        delay(200L); 
        optionsMenu();
      }
    }
  }
  
  //lewo
  if (btt_state_left == LOW ) {
    if (isIntervalMenuOpen)
    {
      for (byte i = 0; i <= 6; i++)
      {
        if (intervalsMenuPosition == i ) 
        {
          if (i == 6 && intervals[0] > 10000) {
            intervals[0] = intervals[0] - 500;
          } 
          else if (i != 6 && intervals[i] > 10000)
          {
            intervals[i] = intervals[i] - 500;
          }
          intervalsMenu();
          delay(200L);
        }
      }
    }
    if (isWateringMenuOpen)
    {
      for (byte i = 0; i <= 6; i++)
      {
        if (wateringMenuPosition == i ) 
        {
          if (i == 6) {
            if(wateringTimes[0] >= 120000 && wateringTimes[0] <= 3600000){
              wateringTimes[0] = wateringTimes[0] - 60000;
            } else if (wateringTimes[0] > 3600000 && wateringTimes[0] <= 86400000) {
              wateringTimes[0] = wateringTimes[0] - 3600000;
            } else if(wateringTimes[0] > 86400000 && wateringTimes[0] < 700000000) {
              wateringTimes[0] = wateringTimes[0] - 86400000;
            }
          } 
          else if (i != 6)
          {
            if(wateringTimes[i] >= 120000 && wateringTimes[i] <= 3600000){
              wateringTimes[i] = wateringTimes[i] - 60000;
            } else if (wateringTimes[i] > 3600000 && wateringTimes[i] <= 86400000) {
              wateringTimes[i] = wateringTimes[i] - 3600000;
            } else if(wateringTimes[i] > 86400000 && wateringTimes[i] < 700000000) {
              wateringTimes[i] = wateringTimes[i] - 86400000;
            }
          }
          wateringTime();
          delay(200L);
        }
      }
    }   
    if (isOptionsMenuOpen)
    {
      if (optionsMenuPosition == 4 && screenLightTime > 3000) 
      {
        screenLightTime = screenLightTime - 500;
        optionsMenu();
        delay(200L);
      }
    }
  }
  
  //prawo
  if (btt_state_right == LOW ) {
    if (isIntervalMenuOpen)
    {
      for (byte i = 0; i <= 6; i++)
      {
        if (intervalsMenuPosition == i ) 
        {
          if (i == 6 && intervals[0] <= 45000) {
            intervals[0] = intervals[0] + 500;
          } 
          else if (i != 6 && intervals[i] <= 45000)
          {
            intervals[i] = intervals[i] + 500;
          }
          intervalsMenu();
          delay(200L);
        }
      }
    }
    if (isWateringMenuOpen)
    {
      for (byte i = 0; i <= 6; i++)
      {
        if (wateringMenuPosition == i ) 
        {
          if (i == 6) {
            if(wateringTimes[0] >= 60000 && wateringTimes[0] < 3600000){
              wateringTimes[0] = wateringTimes[0] + 60000;
            } else if (wateringTimes[0] >= 3600000 && wateringTimes[0] < 86400000) {
              wateringTimes[0] = wateringTimes[0] + 3600000;
            } else if(wateringTimes[0] >= 86400000 && wateringTimes[0] < 604800000) {
              wateringTimes[0] = wateringTimes[0] + 86400000;
            }
          } 
          else if (i != 6)
          {
            if(wateringTimes[i] >= 60000 && wateringTimes[i] < 3600000){
              wateringTimes[i] = wateringTimes[i] + 60000;
            } else if (wateringTimes[i] >= 3600000 && wateringTimes[i] < 86400000) {
              wateringTimes[i] = wateringTimes[i] + 3600000;
            } else if(wateringTimes[i] >= 86400000 && wateringTimes[i] < 604800000) {
              wateringTimes[i] = wateringTimes[i] + 86400000;
            }
          }
          wateringTime();
          delay(200L);
        }
      }
    }   
    if (isOptionsMenuOpen)
    {
      if (optionsMenuPosition == 4 && screenLightTime < 30000) 
      {
        screenLightTime = screenLightTime + 500;
        optionsMenu();
        delay(200L);
      }
    }
  }
  
  // potwierdzanie akcji
  if (btt_state_center == LOW || enableWatering) {
    if (!isMenuOpen && !isIntervalMenuOpen && !isWateringMenuOpen && !isOptionsMenuOpen)
    {    
      showMenu();
      delay(300L);
    } 
    else if (isMenuOpen && !isIntervalMenuOpen && !isWateringMenuOpen && !isOptionsMenuOpen)
     {
      if (menuPosition == 0 || menuPosition == 8)
        {
          Serial.println("podlej wszystkie");
          if(waterLevel && !waterContainerOpen){
            openRelay(true);
          } else {
            checkContainerMessage();
          }
          delay(200L); 
        }
      if (menuPosition == 1)
        {
          Serial.println("podlej suche");
          if(waterLevel && !waterContainerOpen){
            if (counter > 0) openRelay(false);
            else thereIsNoPlants();
          } else {
            checkContainerMessage();
          }
          delay(200L); 
        }
      if (menuPosition == 2)
        {
          Serial.println("ustaw interwały");
          intervalsMenu();
          isMenuOpen = false;
          delay(200L); 
        }
      if (menuPosition == 3)
        {
          Serial.println("ustaw automat");
          wateringTime();
          isMenuOpen = false;
          delay(200L); 
        }
      if (menuPosition == 4)
        {
          Serial.println("Testuj");
          // for(byte i = 0; i < 8; i++) {
          //   pixels.setPixelColor(i, pixels.Color(6, 255, 192));
          // }
          // pixels.show();
          // delay(200L); 
        }
      if (menuPosition == 5)
        {
          Serial.println("Menu z wilgotnoscia");

          
          
          showMenu();
          delay(200L); 
        }
      if (menuPosition == 6)
        {
          isMenuOpen = false;
          optionsMenu();
          delay(200L); 
        }
      if (menuPosition == 7 || menuPosition == -1)
        {
          Serial.println("cofam do home screen");
          isMenuOpen = false;
          menuPosition = 0;
          showStartMenu();
          delay(200L); 
        }
    }
    else if (isIntervalMenuOpen && !isMenuOpen && !isWateringMenuOpen && !isOptionsMenuOpen)
     {
      if (intervalsMenuPosition == 5)
      {
        Serial.println("Zapisuje do eeprom - INTERVALS");

        for(byte i = 0; i < 5; i++){
          if(readValueFromEEPROM(addressesINT[i]) != intervals[i]){
            Serial.println("Zapisuje: " + String(intervals[i]));
            Serial.println("Adres: " + String(addressesINT[i]));
            EEPROM.put(addressesINT[i], intervals[i]);
          }
        }
        delay(200L); 
      }
      if (intervalsMenuPosition == 6 || intervalsMenuPosition == -1)
      {
        isIntervalMenuOpen = false;
        intervalsMenuPosition = 0;
        showMenu();
        delay(200L); 
      }
      
    }
    else if (isWateringMenuOpen && !isMenuOpen && !isIntervalMenuOpen && !isOptionsMenuOpen)
     {

      for(byte i = 0; i < 5; i++){
        if (wateringMenuPosition == i){
          Serial.println(workingRelay[i]);
          workingRelay[i] = !workingRelay[i];
          savedTime[i] = globalMillis;

          wateringTime();
          delay(200L);
        }
      }

      if (wateringMenuPosition == 5)
      {
        Serial.println("Zapisuje do eeprom - WATERING");
        
        for(byte i = 0; i < 5; i++){
          if(readValueFromEEPROM(addressesWAT[i]) != wateringTimes[i]){
            Serial.println("Zapisuje: " + String(wateringTimes[i]));
            Serial.println("Adres: " + String(addressesWAT[i]));
            EEPROM.put(addressesWAT[i], wateringTimes[i]);
          }
          if(readValueFromEEPROMBool(addressesWORK[i]) != workingRelay[i]){
            Serial.println("Zapisuje: " + String(workingRelay[i]));
            Serial.println("Adres: " + String(addressesWORK[i]));
            EEPROM.put(addressesWORK[i], workingRelay[i]);
          }
        }
        delay(200L);  
      }
      if (wateringMenuPosition == 6 || wateringMenuPosition == -1)
      {
        isWateringMenuOpen = false;
        wateringMenuPosition = 0;
        showMenu();
        delay(200L); 
      }
      
    }
    else if (isOptionsMenuOpen && !isWateringMenuOpen && !isMenuOpen && !isIntervalMenuOpen){
      if(optionsMenuPosition == 0 || optionsMenuPosition == 7 ){
        Serial.println("Tryb pracy zmieniam");
          if (workMode == 1){
            firstTimeLoop = true;
          }
          if (workMode == 2) {
            workMode = 0;
          } else {
            workMode++;
          }

          optionsMenu();
          delay(200L);
      }
      if(optionsMenuPosition == 1){
        Serial.println("Dzwiek");
        soundEnable = !soundEnable;
        optionsMenu();
        delay(200L);
      }
      if(optionsMenuPosition == 2){
        Serial.println("LED");
        isLedOn = !isLedOn;
        optionsMenu();
        delay(200L);
      }
      if(optionsMenuPosition == 3){
        Serial.println("Resetuj czas");
        resetTime = !resetTime;
        optionsMenu();
        delay(200L);
      }
      if(optionsMenuPosition == 4){
        Serial.println("Ekran");

        optionsMenu();
        delay(200L);
      }
      if(optionsMenuPosition == 5){
        Serial.println("Zapisz");

        if(readValueFromEEPROMBool(addressesOPT[0]) != workMode){
            Serial.println("Zapisano tryb pracy");
            EEPROM.put(addressesOPT[0], workMode);
        }
        if(readValueFromEEPROMBool(addressesOPT[1]) != soundEnable){
            Serial.println("Zapisano dźwięk");
            EEPROM.put(addressesOPT[1], soundEnable);
        }
        if(readValueFromEEPROMBool(addressesOPT[2]) != isLedOn){
            Serial.println("Zapisano stan led");
            EEPROM.put(addressesOPT[2], isLedOn);
        }
        if(readValueFromEEPROMBool(addressesOPT[3]) != resetTime){
            Serial.println("Zapisano resetowanie");
            EEPROM.put(addressesOPT[3], resetTime);
        }
        if(readValueFromEEPROM(addressesOPT[4]) != screenLightTime){
            Serial.println("Zapisano długość podświetlenia");
            EEPROM.put(addressesOPT[4], screenLightTime);
        }
        lcdPrint(0,0, "-> Zapisano!");        
        delay(200L);
      }
      if(optionsMenuPosition == 6 || optionsMenuPosition == -1){
        Serial.println("cofam do menu");
        isOptionsMenuOpen = false;
        optionsMenuPosition = 0;
        showMenu();
        delay(200L); 
      }
    }
  }
}
 