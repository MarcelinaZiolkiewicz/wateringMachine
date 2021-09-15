#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

bool waterLevel = true;
bool waterContainerOpen = false;
bool plants[] = {false, true, true, false, true};
bool enableWatering = false;

unsigned long previousMillis = 0;
const long interval = 5000;

//relays
const int RELAY_PIN_1 = 11;
const int RELAY_PIN_2 = 12;

int RELAY_STATE = HIGH; 

//buttons
const int BTT_TOP = 2;
const int BTT_LEFT = 4;
const int BTT_RIGHT = 5;
const int BTT_CENTER = 6;
const int BTT_BOTTOM = 3;

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

  int counter;
  for(byte i = 0; i < 4; i=i+1) {
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

void checkContainer() {
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

void openRelay() {
  unsigned long currentMillis = millis();
  enableWatering = true;

  digitalWrite(RELAY_PIN_2, LOW);

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    digitalWrite(RELAY_PIN_2, HIGH);
    enableWatering = false;
  }

  // if (currentMillis - previousMillis >= 1000) {
  //   previousMillis = currentMillis;
  //   digitalWrite(RELAY_PIN_2, HIGH);
  //   enableWatering = false;
  //   showMenu();
  // }

  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("OTWIERAM ZAWORY");
  lcd.setCursor(0,2);
  lcd.print("Podlewanie w toku");
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
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.print("OTWIERAM ZAWORY");
      lcd.setCursor(0,2);
      lcd.print("Podlewanie w toku");

      digitalWrite(RELAY_PIN_2, LOW);
      delay(5000);
      digitalWrite(RELAY_PIN_2, HIGH);
      showMenu();
    } else {
      checkContainer();
    }
  }
  else if(btt_state_top == HIGH){
    digitalWrite(RELAY_PIN_2, HIGH);
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
    Serial.println("bottom");
    openRelay();
  }
}
