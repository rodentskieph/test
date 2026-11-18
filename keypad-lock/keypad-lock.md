# Keypad Door Lock

**Platform:** Tinkercad Circuits · **Board:** Arduino Uno R3 · **Level:** Intermediate

---

## Word Problem

Sir Mark keeps the school's computer laboratory locked, but he keeps losing the key. He asks you to build a **keypad door lock** using an Arduino. Instead of a key, anyone who knows the correct 4-digit PIN can open the door.

The lock must work like this:

| Key / Event | What happens |
|---|---|
| `0` – `9` | Adds a digit to the PIN. The LCD shows `*` for each digit so no one can see the PIN. |
| `*` | Clears the digits typed so far. |
| `#` | Checks the PIN. |
| Correct PIN (`1234`) | The servo turns to 90° (unlocked), the green LED turns on, the buzzer plays a happy tone, and the LCD shows **Access Granted**. |
| Wrong PIN | The red LED blinks, the buzzer plays a low tone, and the LCD shows **Access Denied** with the number of tries left. |
| 3 wrong PINs in a row | The keypad is disabled for 10 seconds, and the LCD shows a countdown. |
| 5 seconds after unlocking, or pressing `D` | The door locks again automatically. |

Every key press should make a short beep so the user knows the key was pressed.

### Guide Questions

1. Why does the LCD show `*` instead of the actual digits?
2. What is the purpose of the `wrongAttempts` variable, and when is it reset back to 0?
3. Why do we use an I2C LCD instead of a regular 16×2 LCD in this project? (Hint: count the Arduino pins used.)
4. How would you change the code if Sir Mark wants the PIN to be `2580`?

---

## Components

This activity uses the following components in Tinkercad Circuits: one Arduino Uno R3 as the main microcontroller, one 4×4 Keypad for entering the PIN, one 16×2 LCD with I2C to display messages, one Micro Servo motor to act as the door lock, one green LED to show that the door is unlocked, one red LED to show that the door is locked or access was denied, two 220 Ω resistors to limit the current through the LEDs, one Piezo buzzer for key beeps and alarm tones, one small breadboard to hold and connect the components, and jumper wires to link everything to the Arduino.

---

## Pin Mapping

Connect the Arduino's 5V pin to the breadboard's positive rail and the GND pin to the negative rail. For the keypad, connect Row 1 to digital pin 9, Row 2 to digital pin 8, Row 3 to digital pin 7, Row 4 to digital pin 6, Column 1 to digital pin 5, Column 2 to digital pin 4, Column 3 to digital pin 3, and Column 4 to digital pin 2. For the I2C LCD, connect its GND pin to the GND rail, its VCC pin to the positive rail, its SDA pin to analog pin A4, and its SCL pin to analog pin A5. For the lock, connect the servo's signal wire (orange) to digital pin 10, its power wire (red) to the positive rail, and its ground wire (brown) to the GND rail. Connect digital pin 11 through a 220 Ω resistor to the anode (long leg) of the green LED, and digital pin 12 through a 220 Ω resistor to the anode of the red LED. Connect the cathode (short leg) of each LED to the GND rail. Finally, connect the piezo buzzer's positive leg to digital pin 13 and its negative leg to the GND rail.

### Circuit Diagram

```
                       ARDUINO UNO
                    ┌──────────────┐
   4x4 KEYPAD       │              │
  ┌──────────┐      │              │
  │ Row 1  ──┼──────┤ D9           │
  │ Row 2  ──┼──────┤ D8           │
  │ Row 3  ──┼──────┤ D7           │
  │ Row 4  ──┼──────┤ D6           │
  │ Col 1  ──┼──────┤ D5           │
  │ Col 2  ──┼──────┤ D4           │
  │ Col 3  ──┼──────┤ D3           │
  │ Col 4  ──┼──────┤ D2           │
  └──────────┘      │              │
                    │              │
   I2C LCD 16x2     │              │
  ┌──────────┐      │              │
  │ GND    ──┼──────┤ GND ─────────┼──────────────────┐ (GND rail)
  │ VCC    ──┼──────┤ 5V ──────────┼───────────────┐  │ (+ rail)
  │ SDA    ──┼──────┤ A4           │               │  │
  │ SCL    ──┼──────┤ A5           │               │  │
  └──────────┘      │              │               │  │
                    │ D10 ─────────┼── SERVO signal │  │
                    │              │   SERVO 5V ────┘  │
                    │              │   SERVO GND ──────┤
                    │              │                   │
                    │ D11 ──[220Ω]─┼──▶|── GREEN ──────┤
                    │ D12 ──[220Ω]─┼──▶|── RED ────────┤
                    │              │                   │
                    │ D13 ─────────┼── (+)PIEZO(−) ────┘
                    └──────────────┘

  ▶| = LED (anode/long leg on the resistor side, cathode/short leg to GND)
```

**Tip:** In Tinkercad, hover over each keypad pin to see its name (Row 1 to Row 4, then Column 1 to Column 4, from left to right). Search for **LCD 16 x 2 (I2C)** in the components panel, not the regular LCD.

---

## Code

In Tinkercad, open the **Code** panel, switch the editor from **Blocks** to **Text**, delete the default code, and paste the following:

```cpp
/*
  Keypad Door Lock (Tinkercad Circuits / Arduino Uno)
  ---------------------------------------------------
  Enter the PIN on a 4x4 keypad.
    0-9 = type a digit
    *   = clear
    #   = submit
    D   = lock again (while unlocked)

  Correct PIN  -> servo unlocks, green LED, "Access Granted"
  Wrong PIN    -> red LED blinks, low tone, "Access Denied"
  3 wrong PINs -> keypad disabled for 10 seconds
  Auto-locks 5 seconds after unlocking.
*/

#include <Keypad.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ---------- Keypad setup ----------
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ---------- Other components ----------
LiquidCrystal_I2C lcd(0x20, 16, 2);   // I2C address, 16 columns, 2 rows
Servo lockServo;

const int SERVO_PIN  = 10;
const int GREEN_LED  = 11;
const int RED_LED    = 12;
const int BUZZER_PIN = 13;

// ---------- Settings ----------
const String PASSWORD = "1234";
const int MAX_LENGTH   = 8;       // max digits the user can type
const int MAX_ATTEMPTS = 3;       // wrong tries before lockout

const int LOCKED_ANGLE   = 0;
const int UNLOCKED_ANGLE = 90;

const unsigned long UNLOCK_TIME  = 5000;   // ms before auto-lock
const unsigned long LOCKOUT_TIME = 10000;  // ms of lockout

// ---------- State ----------
String input = "";
int wrongAttempts = 0;
bool unlocked = false;
unsigned long unlockStart = 0;

// ---------- LCD helpers ----------
void showPrompt() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter PIN:");
  showInput();
}

void showInput() {
  lcd.setCursor(0, 1);
  lcd.print("                ");   // clear the second row
  lcd.setCursor(0, 1);
  for (int i = 0; i < input.length(); i++) {
    lcd.print('*');
  }
}

void showMessage(String line1, String line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

// ---------- Lock actions ----------
void lockDoor() {
  lockServo.write(LOCKED_ANGLE);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, HIGH);
  unlocked = false;

  showMessage("Door Locked", "");
  tone(BUZZER_PIN, 800, 150);
  delay(1000);
  showPrompt();
}

void unlockDoor() {
  wrongAttempts = 0;
  lockServo.write(UNLOCKED_ANGLE);
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);

  showMessage("Access Granted", "Door unlocked");

  // Happy rising tone
  tone(BUZZER_PIN, 1000, 150);
  delay(200);
  tone(BUZZER_PIN, 1500, 250);
  delay(300);

  unlocked = true;
  unlockStart = millis();
}

void accessDenied() {
  wrongAttempts++;
  int triesLeft = MAX_ATTEMPTS - wrongAttempts;

  showMessage("Access Denied", "Tries left: " + String(triesLeft));

  // Low tone and blinking red LED
  tone(BUZZER_PIN, 300, 600);
  for (int i = 0; i < 3; i++) {
    digitalWrite(RED_LED, LOW);
    delay(150);
    digitalWrite(RED_LED, HIGH);
    delay(150);
  }
  delay(600);

  if (wrongAttempts >= MAX_ATTEMPTS) {
    lockout();
  } else {
    showPrompt();
  }
}

void lockout() {
  // Keypad is ignored during this countdown
  for (int s = LOCKOUT_TIME / 1000; s > 0; s--) {
    showMessage("LOCKED OUT!", "Wait " + String(s) + " sec");
    tone(BUZZER_PIN, 400, 200);
    delay(1000);
  }
  wrongAttempts = 0;
  showPrompt();
}

void checkPin() {
  if (input.length() == 0) return;   // nothing typed yet

  if (input == PASSWORD) {
    unlockDoor();
  } else {
    accessDenied();
  }
  input = "";
}

// ---------- Setup and loop ----------
void setup() {
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  lockServo.attach(SERVO_PIN);
  lockServo.write(LOCKED_ANGLE);
  digitalWrite(RED_LED, HIGH);

  lcd.init();
  lcd.backlight();

  showMessage("Keypad Lock", "Starting...");
  delay(1000);
  showPrompt();
}

void loop() {
  char key = keypad.getKey();

  // While unlocked: lock on 'D' or after UNLOCK_TIME
  if (unlocked) {
    if (key == 'D' || millis() - unlockStart >= UNLOCK_TIME) {
      lockDoor();
    }
    return;
  }

  if (!key) return;   // no key pressed

  tone(BUZZER_PIN, 2000, 40);   // key beep

  if (key >= '0' && key <= '9') {
    if (input.length() < MAX_LENGTH) {
      input += key;
      showInput();
    }
  }
  else if (key == '*') {
    input = "";
    showInput();
  }
  else if (key == '#') {
    checkPin();
  }
  // A, B, C (and D while locked) are ignored
}
```

### How It Works

The `Keypad` library scans the 4×4 keypad by turning on one row at a time and checking which column connects to it, then returns the character from the `keys` map. Each digit is added to the `input` string, and the LCD shows one `*` per digit. When `#` is pressed, `checkPin()` compares `input` with `PASSWORD`. If they match, the servo turns to 90°, the green LED turns on, and the time is saved with `millis()` so the door can lock itself after 5 seconds. If they don't match, `wrongAttempts` goes up by one, and after 3 wrong tries the `lockout()` function shows a 10-second countdown while ignoring the keypad. The `LiquidCrystal_I2C` library controls the LCD over I2C, so it only needs two data pins (A4 and A5), which leaves enough digital pins for the keypad, servo, LEDs, and buzzer.

---

## Testing in Tinkercad

1. Click **Start Simulation**. The LCD should show **Enter PIN:** and the red LED should be on.
2. Click the keypad keys `1`, `2`, `3`, `4`, then `#`. The servo should turn, the green LED should turn on, and the LCD should show **Access Granted**.
3. Wait 5 seconds and check that the door locks again automatically.
4. Enter a wrong PIN such as `1111#` and check that **Access Denied** and the number of tries left appear.
5. Enter a wrong PIN 3 times in a row and check that the 10-second lockout countdown starts.
6. Type a few digits, press `*`, and check that the input is cleared.
7. Unlock the door again, then press `D` and check that it locks immediately.

**Tip:** If the LCD stays blank, make sure you used the **LCD 16 x 2 (I2C)** component and that SDA goes to A4 and SCL goes to A5. Then click the LCD and check its I2C address in the Inspector panel. The code uses `0x20` (32), which is the Tinkercad default. If your LCD shows a different address, such as `0x27`, change the number in `LiquidCrystal_I2C lcd(0x20, 16, 2);` to match.

---

## Extension Challenges (Optional)

1. Add a "change PIN" feature: after unlocking, press `A`, type a new PIN, and press `#` to save it.
2. Make the lockout time double after each lockout (10 s, 20 s, 40 s...).
3. Add a second valid PIN for a guest that only works once.
4. Print every access attempt (granted or denied) to the Serial Monitor as a simple log.