//EE356-Product Design


//Libraries
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//Define constants
#define I2C_ADDR 0x27
#define BACKLIGHT_PIN 3
#define LCD_COLS 16
#define LCD_ROWS 2

LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLS, LCD_ROWS);

//Define pins
const int switchPin = 2;    // Mode changing switch
const int buttonIncrement = 4;   //Increse digit value
const int buttonEnter = 5;    //Enter the digit value

bool manualMode = false;

// Variables for digit values and cursor position for manual mode temperature
int digit1 = 0, digit2 = 0, digit3 = 0;
int cursorPosition = 0;
bool blinkCursor = true;
unsigned long lastBlinkTime = 0;
const unsigned long blinkInterval = 500; // Blinking interval in milliseconds
bool finalValueDisplayed = false; // Flag to track if final value is displayed

//LCD initialization
void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(switchPin, INPUT_PULLUP);         //no need external resistance
  pinMode(buttonIncrement, INPUT_PULLUP);
  pinMode(buttonEnter, INPUT_PULLUP);

  // Initialize I2C communication
  Wire.begin(21, 22);   // SDA = 21, SCL = 22
}

void loop() {
  // Check the switch status
  bool switchState = digitalRead(switchPin);

  if (switchState) { // Switch is ON (Manual Mode)
    if (!manualMode) {
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("MANUAL MODE");
      lcd.setCursor(0, 1);
      lcd.print("TURNING ON");
      delay(500); lcd.setCursor(10, 1); lcd.print(".");
      delay(500); lcd.setCursor(11, 1); lcd.print(".");
      delay(500); lcd.setCursor(12, 1); lcd.print(".");
      delay(500); lcd.setCursor(13, 1); lcd.print(".");
      delay(500); lcd.setCursor(14, 1); lcd.print(".");
      delay(2000); 
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("ENTER");
      lcd.setCursor(0, 1); lcd.print("TEMPERATURE:");
      lcd.setCursor(13, 1); lcd.print("000");    // Display initial 3 zeroes
      manualMode = true;
      finalValueDisplayed = false; // Reset the flag
      resetInputValue(); // Reset the input value
    }

    // Check if increment button is pressed
    int incrementCount = 0;
    while (digitalRead(buttonIncrement) == LOW) {
      incrementCount++;
      delay(200); // Debounce delay
    }
    if (incrementCount > 0 && !finalValueDisplayed) {
      incrementDigit(incrementCount);
    }

    // Check if enter button is pressed
    if (digitalRead(buttonEnter) == LOW && !finalValueDisplayed) {
      moveToNextDigit();
      delay(200); // Debounce delay
    }

    // Update LCD display
    updateLCDDisplay();
  } 
    else { // Switch is OFF (Auto Mode)
    if (manualMode) {
      lcd.clear();
      lcd.setCursor(4, 0); lcd.print("AUTO MODE");
      lcd.setCursor(0, 1); lcd.print("TURNING ON");
      delay(500); lcd.setCursor(10, 1); lcd.print(".");
      delay(500); lcd.setCursor(11, 1); lcd.print(".");
      delay(500); lcd.setCursor(12, 1); lcd.print(".");
      delay(500); lcd.setCursor(13, 1); lcd.print(".");
      delay(500); lcd.setCursor(14, 1); lcd.print(".");
      delay(1000); // Display the message for 1 second
      lcd.clear();
      lcd.setCursor(4, 0); lcd.print("AUTO MODE");
      manualMode = false;
      resetInputValue(); // Reset the input value
    }
  }
  // Add any additional logic here based on manualMode if needed
}

void resetInputValue() {
  digit1 = 0;
  digit2 = 0;
  digit3 = 0;
  cursorPosition = 0;
  finalValueDisplayed = false;
}

//When press pushbutton 1
void incrementDigit(int count) {
  switch (cursorPosition) {
    case 0:
      digit1 = (digit1 + count) % 10;
      break;
    case 1:
      digit2 = (digit2 + count) % 10;
      break;
    case 2:
      digit3 = (digit3 + count) % 10;
      break;
  }
}

//When press pushbutton 2
void moveToNextDigit() {
  if (cursorPosition == 2) {
    // All three digits have been entered
    displayFinalValue();
    finalValueDisplayed = true; // Set the flag to true
  } else {
    cursorPosition = (cursorPosition + 1) % 3;
    blinkCursor = true;
  }
}

void displayFinalValue() {
  int finalValue = (digit1 * 100) + (digit2 * 10) + digit3;
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("MAX Temperature:");
  lcd.setCursor(0, 1); lcd.print(finalValue);
}

void updateLCDDisplay() {
  unsigned long currentTime = millis();
  if (currentTime - lastBlinkTime >= blinkInterval) {
    lastBlinkTime = currentTime;
    blinkCursor = !blinkCursor;
  }

  if (!finalValueDisplayed) { // Only display blinking cursor if final value is not displayed
    lcd.setCursor(13, 1);
    if (blinkCursor && cursorPosition == 0) {
      lcd.print("_");
    } else {
      lcd.print(digit1);
    }

    lcd.setCursor(14, 1);
    if (blinkCursor && cursorPosition == 1) {
      lcd.print("_");
    } else {
      lcd.print(digit2);
    }

    lcd.setCursor(15, 1);
    if (blinkCursor && cursorPosition == 2) {
      lcd.print("_");
    } else {
      lcd.print(digit3);
    }
  }
}