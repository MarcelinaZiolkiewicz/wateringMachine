#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

//relays
const byte RELAY_PIN_1 = 11;
const byte RELAY_PIN_2 = 12;
//buttons
const byte BTT_TOP = 2;
const byte BTT_LEFT = 4;
const byte BTT_RIGHT = 5;
const byte BTT_CENTER = 6;
const byte BTT_BOTTOM = 3;

bool waterLevel = true;
bool waterContainerOpen = false;
bool plants[] = {false, true, true, false, false};
bool enableWatering = false;
bool firstRun = true;

unsigned long buttonPushedMillis;
unsigned long longestInterval;
unsigned long firstRealySavedTime = 0;
unsigned long secondRealySavedTime = 0;

unsigned long intervals[] = {5000, 4500, 3000, 1000, 7000};


void showMessage(String message) {
   lcd.clear();
   lcd.setCursor(0,2);
   lcd.print(message);
}

void showMenu() {
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

  int counter = 0;
  for(byte i = 0; i <= 4; i++) {
    if (!plants[i]) {
      counter++;
    }
  }
  lcd.print(counter);

  lcd.setCursor(0,2);
  lcd.print("Otworz menu -> ");
  lcd.setCursor(16,2);
  lcd.print("(OK)");

}

void checkContainerMessage() {
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

    if (!plants[0] || waterAll) {
      Serial.println(!plants[0]);
      Serial.println(waterAll);
      digitalWrite(RELAY_PIN_1, LOW);
    }
    if (!plants[1] || waterAll) {
      Serial.println(!plants[1]);
      Serial.println(waterAll);
      digitalWrite(RELAY_PIN_2, LOW);
    }
  
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("OTWIERAM ZAWORY");
    lcd.setCursor(0,2);
    lcd.print("Podlewanie w toku");

    Serial.println("First run");

    firstRun = false;
  }

  if (currentMillis - buttonPushedMillis >= intervals[0]) {
    digitalWrite(RELAY_PIN_1, HIGH);
  }

  if (currentMillis - buttonPushedMillis >= intervals[1]) {
    digitalWrite(RELAY_PIN_2, HIGH);
  }

  if (currentMillis - buttonPushedMillis >= longestInterval) {
    Serial.println("Zamykam");
    enableWatering = false;
    firstRun = true;
    showMenu();
  }


}

void setup() {
  lcd.init(); 
  lcd.backlight();
  showMenu();

  //initialization
  Serial.begin(9600);
  pinMode(BTT_TOP, INPUT_PULLUP);
  pinMode(BTT_LEFT, INPUT_PULLUP);
  pinMode(BTT_RIGHT, INPUT_PULLUP);
  pinMode(BTT_CENTER, INPUT_PULLUP);
  pinMode(BTT_BOTTOM, INPUT_PULLUP);
  
  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);

  digitalWrite(RELAY_PIN_2, HIGH);
  digitalWrite(RELAY_PIN_1, HIGH);
  Serial.println("Connected");
}

void loop() {  
  int btt_state_top = digitalRead(BTT_TOP); //
  int btt_state_left = digitalRead(BTT_LEFT);
  int btt_state_right = digitalRead(BTT_RIGHT);
  int btt_state_center = digitalRead(BTT_CENTER);
  int btt_state_bottom = digitalRead(BTT_BOTTOM);

  if (btt_state_top == LOW) {
    if(waterLevel && !waterContainerOpen){
      openRelay(false);
    } else {
      checkContainerMessage();
    }
  }

  if (btt_state_left == LOW) {
    Serial.println("left");
    showMessage("LEFT");
  }
  if (btt_state_right == LOW) {
    Serial.println("right");
    showMessage("RIGHT");
  }
  if (btt_state_center == LOW) {
    Serial.println("center");
    showMenu();
  }
  if (btt_state_bottom == LOW || enableWatering) {
    if(waterLevel && !waterContainerOpen){
      openRelay(true);
    } else {
      checkContainerMessage();
    }
  }
}
