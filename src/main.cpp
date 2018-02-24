#include <Arduino.h>
#include <limits.h>
#include <LiquidCrystal_I2C.h>

// Symbols / preferences / etc
#define SRVI_BUTTON 3
#define SRVI_MAX_STATES 8
#define SRVI_DEBOUNCE_MS 100
#define SRVI_SERIAL_RX_MAX 64
#define SRVI_SERIAL_BAUD 115200
#define SRVI_HELLO "Monitor is on"

// Logic variables
volatile uint8_t currentState = 0;
volatile uint32_t lastButtonTime = 0;
volatile boolean stringComplete = false;
char cnv_buf[8];
String inputString = ""; 

// Hardware variables
LiquidCrystal_I2C lcd(0x3F, 16, 2);

// Functions
void buttonISR();

void setup() {
    Serial.begin(SRVI_SERIAL_BAUD);
    inputString.reserve(SRVI_SERIAL_RX_MAX);

    pinMode(SRVI_BUTTON, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(SRVI_BUTTON), buttonISR, RISING);

    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(SRVI_HELLO);
}

void loop() {
    lcd.setCursor(1, 1);
    lcd.print(currentState);

    if (stringComplete) {
        lcd.setCursor(0, 0);
        lcd.print(inputString);

        for (uint8_t idx = 0; idx < inputString.length(); ++idx) {
            Serial.print(itoa((uint8_t)inputString[idx], cnv_buf, 10));
            Serial.print(' ');
        }
        Serial.println('\n');

        inputString = "";
        stringComplete = false;
    }

    delay(100);
}

void buttonISR() {
    uint32_t ms = millis();
    if (min(ms - lastButtonTime, ULONG_MAX - lastButtonTime + ms) <= SRVI_DEBOUNCE_MS) {
        return;
    }

    lastButtonTime = ms;
    currentState += 1;
    if (currentState >= SRVI_MAX_STATES) {
        currentState = 0;
    }
}

void serialEvent() {
  while (!stringComplete && Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    if (inChar != '\n' && inChar != (char)10) {
        inputString += inChar;
    }
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}