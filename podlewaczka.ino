#include <Arduino.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

//TODO
/*
  WAŻNE
 - podpiąć czujnik zamknięcia i sprawdzaź przed pompowaniem 
 - zapisywanie trybu w którym pracuje stacja
  
  MNIEJ WAŻNE
 - priorytet ręcznego podlewania nad automatycznym
 - Animacja z trzena kropkami kiedy otwarte są zawory 
 - wygaszanie ekranu co jakiś czas dla oszczędzania energii
 - wyświetlanie odliczania w dół pod napisem o otwartych zaworach 
*/

//relays
const byte RELAYS[] = {11,12,13,9,8};

//buttons
const byte BTT_TOP = 2;
const byte BTT_LEFT = 4;
const byte BTT_RIGHT = 5;
const byte BTT_CENTER = 6;
const byte BTT_BOTTOM = 3;
byte workMode = 0;
byte addressesINT[] = {0,4,8,12,16}; 
byte addressesWAT[] = {20,24,28,32,36};
byte addressesWORK[] = {40,41,42,43,44};

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
bool relayToOpen[] = {false,false,false,false,false};
bool workingRelay[] = {true,true,true,true,true};
bool firstTimeLoop = false;

unsigned long buttonPushedMillis;
unsigned long wateringTimeMillis[] = {0,0,0,0,0};
unsigned long longestInterval;
unsigned long firstRealySavedTime = 0;
unsigned long secondRealySavedTime = 0;
unsigned long intervals[] = {5000, 2000, 3000, 40000, 50000};
unsigned long wateringTimes[] = {10000, 12000, 8000, 60000, 60000};
unsigned long savedTime[] = {0,0,0,0,0};

int counter;
int menuPosition = 0;
int intervalsMenuPosition = 0;
int wateringMenuPosition = 0;

void showMessage(String message) {
   lcd.clear();
   lcd.setCursor(0,1);
   lcd.print(message);
}

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

void showMenu() {
  isMenuOpen = true;

  lcd.clear();
  lcd.setCursor(0,0);

  switch (menuPosition)
  {
  case 7:
  case 0: 
    lcd.print("-> Podlej wszystkie");
    lcd.setCursor(0,1);
    lcd.print("Podlej suche");
    lcd.setCursor(0,2);
    lcd.print("Ustaw interwaly");
    lcd.setCursor(0,3);
    lcd.print("Ustaw automat");
    break;
  case 1:
    lcd.print("-> Podlej suche");
    lcd.setCursor(0,1);
    lcd.print("Ustaw interwaly");
    lcd.setCursor(0,2);
    lcd.print("Ustaw automat");
    lcd.setCursor(0,3);
    lcd.print("Testuj");
    break;
  case 2:
    lcd.print("-> Ustaw interwaly");
    lcd.setCursor(0,1);
    lcd.print("Ustaw automat");
    lcd.setCursor(0,2);
    lcd.print("Testuj");
    lcd.setCursor(0,3);
    lcd.print("Tryb: " + showWorkMode());
    break;
  case 3:
    lcd.print("-> Ustaw automat");
    lcd.setCursor(0,1);
    lcd.print("Testuj");
    lcd.setCursor(0,2);
    lcd.print("Tryb: " + showWorkMode());
    lcd.setCursor(0,3);
    lcd.print("Cofnij");
    break;
  case 4:
    lcd.print("-> Testuj");
    lcd.setCursor(0,1);
    lcd.print("Tryb: " + showWorkMode());
    lcd.setCursor(0,2);
    lcd.print("Cofnij");
    lcd.setCursor(0,3);
    lcd.print("Podlej wszystkie");
    break;
  case 5:
    lcd.print("-> Tryb: " + showWorkMode());
    lcd.setCursor(0,1);
    lcd.print("Cofnij");
    lcd.setCursor(0,2);
    lcd.print("Podlej wszystkie");
    lcd.setCursor(0,3);
    lcd.print("Podlej suche");
    break;
  case -1:
  case 6:
    lcd.print("-> Cofnij");
    lcd.setCursor(0,1);
    lcd.print("Podlej wszystkie");
    lcd.setCursor(0,2);
    lcd.print("Podlej suche");
    lcd.setCursor(0,3);
    lcd.print("Ustaw interwaly");
    break;

  default:
    Serial.println("Błąd");
    break;
  }
}

void intervalsMenu() {
  isIntervalMenuOpen = true;
  
  lcd.clear();
  lcd.setCursor(0,0);

  switch (intervalsMenuPosition)
  {
  case 7:
  case 0: 
    lcd.print("-> Roslina 1: " + calculateTime(intervals[0], 0));
    lcd.setCursor(0,1);
    lcd.print("Roslina 2: " + calculateTime(intervals[1], 1));
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

void showStartMenu() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Poziom wody: ");
  lcd.setCursor(13,0);
  
  if (waterLevel) {
    lcd.print("OK");
  } else {
    lcd.print("PUSTO!");
  }

  lcd.setCursor(0,1);
  lcd.print("Do podlania: ");

  counter = 0;
  for(byte i = 0; i <= 4; i++) {
    if (!plants[i]) {
      counter++;
    }
  }
  lcd.print(counter);

  
  lcd.setCursor(0,2);
  lcd.print("Tryb: " + showWorkMode());

  lcd.setCursor(0,3);
  lcd.print("Otworz menu -> ");
  lcd.setCursor(16,3);
  lcd.print("(OK)");

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
        lcd.setCursor(0,1);
        lcd.print("Zamknij zbiornik!");
        lcd.setCursor(0,2);
        lcd.print("Uzupelnij wode!");
    } else if (waterContainerOpen){
      showMessage("Zamknij zbiornik!");
    } else {
      showMessage("Uzupelnij wode!");
    }

    firstRun = false;
  }

  if (currentMillis - buttonPushedMillis >= 2500) {
    enableWatering = false;
    firstRun = true;
    showMenu();
  }
}

void thereIsNoPlants() {
  unsigned long currentMillis = millis();
  enableWatering = true;

  while (firstRun)
  {
    buttonPushedMillis = currentMillis;

    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("Rosliny sa juz");
    lcd.setCursor(0,2);
    lcd.print("podlane");

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
        longestInterval = intervals[i];
      }
    }

    for(byte i = 0; i < 5; i++){
      if (!plants[i] || waterAll) {
      digitalWrite(RELAYS[i], HIGH);
      }
    } 
  
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("OTWIERAM ZAWORY");
    lcd.setCursor(0,2);
    lcd.print("Podlewanie w toku");

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

void setup() {
  lcd.init(); 
  lcd.backlight();
  showStartMenu();

  //initialization
  Serial.begin(9600);
  pinMode(BTT_TOP, INPUT_PULLUP);
  pinMode(BTT_LEFT, INPUT_PULLUP);
  pinMode(BTT_RIGHT, INPUT_PULLUP);
  pinMode(BTT_CENTER, INPUT_PULLUP);
  pinMode(BTT_BOTTOM, INPUT_PULLUP);
  
  pinMode(RELAYS[0], OUTPUT);
  pinMode(RELAYS[1], OUTPUT);
  pinMode(RELAYS[2], OUTPUT);
  pinMode(RELAYS[3], OUTPUT);
  pinMode(RELAYS[4], OUTPUT);

  

  Serial.println("Connected");
  Serial.println("Wczytuje z EEPROM: ");

  for(byte i = 0; i < 5; i++){
    intervals[i] = readValueFromEEPROM(addressesINT[i]);
  };

  for(byte i = 0; i < 5; i++){
    wateringTimes[i] = readValueFromEEPROM(addressesWAT[i]);
  };

  for(byte i = 0; i < 5; i++){
    workingRelay[i] = readValueFromEEPROMBool(addressesWORK[i]);
  };
  Serial.println("Wczytano");
}

void loop() {  
  unsigned long globalMillis = millis();;

  if(workMode == 2) {

    resetMillis(globalMillis);
 
    for(byte i = 0; i < 5; i++){
      if (globalMillis - savedTime[i] >= wateringTimes[i] && workingRelay[i]) {
        savedTime[i] = globalMillis;
        openSingleRelay(i);
      }    
    }
  }
  
  closeSingleRelay();

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
      menuPosition = 6;
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
    
    
  }
  //W dół
  if (btt_state_bottom == LOW) {
    if(isMenuOpen) {
      if(menuPosition > 6){
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
  }
  //zmiana interwałów w dół
  if (btt_state_left == LOW ) {
    if (isIntervalMenuOpen)
    {
      for (byte i = 0; i <= 6; i++)
      {
        if (intervalsMenuPosition == i ) 
        {
          if (i == 6 && intervals[0] > 500) {
            intervals[0] = intervals[0] - 500;
          } 
          else if (i != 6 && intervals[i] > 500)
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
  }
  //zmiana interwałów w góre
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
  }
  // potwierdzanie akcji
  if (btt_state_center == LOW || enableWatering) {
    if (!isMenuOpen && !isIntervalMenuOpen && !isWateringMenuOpen)
    {    
      showMenu();
      delay(300L);
    } 
    else if (isMenuOpen && !isIntervalMenuOpen && !isWateringMenuOpen)
     {
      if (menuPosition == 0 || menuPosition == 7)
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
          delay(200L); 
        }
      if (menuPosition == 5)
        {
          if (workMode == 1){
            firstTimeLoop = true;
          }
          if (workMode == 2) {
            workMode = 0;
            Serial.println("ustawione 0");
          } else {
            workMode++;
          }
          
          showMenu();
          delay(200L); 
        }
      if (menuPosition == 6 || menuPosition == -1)
        {
          Serial.println("cofam do home screen");
          isMenuOpen = false;
          menuPosition = 0;
          showStartMenu();
          delay(200L); 
        }
    }
    else if (isIntervalMenuOpen && !isMenuOpen && !isWateringMenuOpen)
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
    else if (isWateringMenuOpen && !isMenuOpen && !isIntervalMenuOpen)
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
  }
}
