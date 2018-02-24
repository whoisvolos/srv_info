#include <Arduino.h>
#include <limits.h>
#include <LiquidCrystal_I2C.h>

// Symbols / preferences / etc
#define IS_BUTTON 3
#define MAX_STATES 8
#define DEBOUNCE_MS 300

// Logic variables
uint8_t currentState = 0;
uint32_t lastButtonTime = 0;

// Hardware variables
LiquidCrystal_I2C lcd(0x3F, 16, 2);

void setup() {
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Hello world!");
    lcd.setCursor(1, 1);
    lcd.print(currentState);

    Serial.begin(115200);

    pinMode(IS_BUTTON, INPUT_PULLUP);
}

uint8_t checkButtonPressed() {
    uint32_t ms = millis();
    //Serial.println(ms);
    if (lastButtonTime == 0) {
        lastButtonTime = ms;
        return HIGH;
    }
    if (min(ms - lastButtonTime, ULONG_MAX - lastButtonTime + ms) > DEBOUNCE_MS) {
        lastButtonTime = ms;
        return HIGH;
    } else {
        return LOW;
    }
}

void loop() {
    if (digitalRead(IS_BUTTON) == LOW) {
        if (checkButtonPressed()) {
            currentState += 1;
            if (currentState >= MAX_STATES) {
                currentState = 0;
            }
            lcd.setCursor(1, 1);
            lcd.print(currentState);
        }
    }
    
    delay(10);
}