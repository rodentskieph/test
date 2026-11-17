# Smart Room Controller (Serial Communication)

**Platform:** Tinkercad Circuits · **Board:** Arduino Uno R3 · **Level:** Intermediate

---

## Word Problem

Ate Liza wants to control her room from her laptop without standing up. She asks you to build a **Smart Room Controller** using an Arduino. She will type a single letter in the Serial Monitor, and the Arduino will respond:

| Command | Action |
|---|---|
| `a` | Turn the room light ON |
| `b` | Turn the room light OFF |
| `c` | Open the door (servo turns to 90°) |
| `d` | Close the door (servo turns to 0°) |
| `e` | Ring the doorbell (buzzer plays a two-tone chime) |
| `f` | Show the status of the light and the door |
| `h` | Show the list of commands |

The Arduino must reply with a message after every command (for example, `Light: ON`) so Ate Liza knows the command worked. Uppercase letters should also work, and any unknown letter should show an error message instead of doing nothing.

### Guide Questions

1. What does `Serial.available()` tell the Arduino, and why do we check it before calling `Serial.read()`?
2. Why does the code use `char` instead of `int` to store the command?
3. Why is a `switch` statement a good choice for handling many different commands?
4. What happens if the user types `A` instead of `a`? Which line of code handles this?

---

## Components

This activity uses the following components in Tinkercad Circuits: one Arduino Uno R3 as the main microcontroller, one yellow LED to act as the room light, one 220 Ω resistor to limit the current through the LED, one Micro Servo motor to act as the door, one Piezo buzzer to act as the doorbell, one small breadboard to hold and connect the components, and jumper wires to link everything to the Arduino.

---

## Pin Mapping

Connect the Arduino's 5V pin to the breadboard's positive rail and the GND pin to the negative rail. For the room light, connect digital pin 6 through a 220 Ω resistor to the anode (long leg) of the yellow LED, and connect the cathode (short leg) to the GND rail. For the door, connect the servo's signal wire (orange) to digital pin 9, its power wire (red) to the positive rail, and its ground wire (brown) to the GND rail. For the doorbell, connect the piezo buzzer's positive leg to digital pin 8 and its negative leg to the GND rail.

### Circuit Diagram

```
                 ARDUINO UNO
              ┌──────────────┐
              │ 5V ──────────┼─────────────────────┐ (+ rail)
              │ GND ─────────┼──────────────────┐  │ (GND rail)
              │              │                  │  │
              │ D6 ──[220Ω]──┼──▶|── YELLOW ────┤  │
              │              │                  │  │
              │ D8 ──────────┼── (+)PIEZO(−) ───┤  │
              │              │                  │  │
              │ D9 ──────────┼── SERVO signal   │  │
              │              │   SERVO GND  ────┘  │
              │              │   SERVO 5V  ────────┘
              └──────────────┘

  ▶| = LED (anode/long leg on the resistor side, cathode/short leg to GND)
```

---

## Code

In Tinkercad, open the **Code** panel, switch the editor from **Blocks** to **Text**, delete the default code, and paste the following:

```cpp
/*
  Smart Room Controller (Tinkercad Circuits / Arduino Uno)
  --------------------------------------------------------
  Control a light, a door, and a doorbell by typing letters
  in the Serial Monitor.

  Commands:
    a = light ON      b = light OFF
    c = open door     d = close door
    e = doorbell      f = status
    h = help
*/

#include <Servo.h>

// ---------- Pin assignments ----------
const int LIGHT_PIN  = 6;
const int BUZZER_PIN = 8;
const int SERVO_PIN  = 9;

// ---------- Door angles ----------
const int DOOR_CLOSED = 0;
const int DOOR_OPEN   = 90;

Servo door;

// ---------- Current state ----------
bool lightOn   = false;
bool doorOpen  = false;
int  doorAngle = DOOR_CLOSED;

void printMenu() {
  Serial.println("=============================");
  Serial.println("   SMART ROOM CONTROLLER");
  Serial.println("=============================");
  Serial.println(" a - Light ON");
  Serial.println(" b - Light OFF");
  Serial.println(" c - Open door");
  Serial.println(" d - Close door");
  Serial.println(" e - Ring doorbell");
  Serial.println(" f - Show status");
  Serial.println(" h - Show this menu");
  Serial.println("=============================");
}

void printStatus() {
  Serial.print("Light: ");
  Serial.println(lightOn ? "ON" : "OFF");
  Serial.print("Door : ");
  Serial.println(doorOpen ? "OPEN" : "CLOSED");
}

// Move the servo smoothly, one degree at a time
void moveDoor(int target) {
  int step = (target > doorAngle) ? 1 : -1;
  while (doorAngle != target) {
    doorAngle += step;
    door.write(doorAngle);
    delay(10);
  }
}

// Two-tone "ding-dong" chime
void ringDoorbell() {
  tone(BUZZER_PIN, 659, 300);  // E5
  delay(350);
  tone(BUZZER_PIN, 523, 500);  // C5
  delay(550);
  noTone(BUZZER_PIN);
}

void setup() {
  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(LIGHT_PIN, LOW);

  door.attach(SERVO_PIN);
  door.write(DOOR_CLOSED);

  Serial.begin(9600);
  printMenu();
}

void loop() {
  // Do nothing until a character arrives
  if (Serial.available() > 0) {
    char cmd = Serial.read();

    // Ignore Enter key and spaces
    if (cmd == '\n' || cmd == '\r' || cmd == ' ') return;

    // Accept uppercase letters too (A -> a)
    cmd = tolower(cmd);

    switch (cmd) {
      case 'a':
        digitalWrite(LIGHT_PIN, HIGH);
        lightOn = true;
        Serial.println("Light: ON");
        break;

      case 'b':
        digitalWrite(LIGHT_PIN, LOW);
        lightOn = false;
        Serial.println("Light: OFF");
        break;

      case 'c':
        if (doorOpen) {
          Serial.println("Door is already OPEN");
        } else {
          Serial.println("Opening door...");
          moveDoor(DOOR_OPEN);
          doorOpen = true;
          Serial.println("Door: OPEN");
        }
        break;

      case 'd':
        if (!doorOpen) {
          Serial.println("Door is already CLOSED");
        } else {
          Serial.println("Closing door...");
          moveDoor(DOOR_CLOSED);
          doorOpen = false;
          Serial.println("Door: CLOSED");
        }
        break;

      case 'e':
        Serial.println("Ding-dong!");
        ringDoorbell();
        break;

      case 'f':
        printStatus();
        break;

      case 'h':
        printMenu();
        break;

      default:
        Serial.print("Unknown command: ");
        Serial.println(cmd);
        Serial.println("Type h for help.");
        break;
    }
  }
}
```

### How It Works

The Serial Monitor sends each typed character to the Arduino over USB at 9600 baud. In `loop()`, `Serial.available()` checks if a character has arrived, and `Serial.read()` takes it out of the buffer. The code skips Enter and space characters, then converts the letter to lowercase with `tolower()` so both `A` and `a` work. A `switch` statement picks the action for each letter: it turns the LED on or off, sweeps the servo one degree at a time to open or close the door, or plays a chime on the buzzer. The Arduino also remembers the state of the light and the door in variables, so it can report the status and avoid opening a door that is already open. Every action ends with `Serial.println()` so the user gets a reply.

---

## Testing in Tinkercad

1. Click **Start Simulation**.
2. Open the **Serial Monitor** at the bottom of the Code panel. The command menu should appear.
3. Type `a` in the input box and click **Send**. The yellow LED should turn on, and the monitor should show `Light: ON`.
4. Try each of the other commands (`b`, `c`, `d`, `e`, `f`, `h`) and check that the components respond.
5. Type a letter that is not on the list, such as `z`, and check that the error message appears.

---

## Extension Challenges (Optional)

1. Add a command `g` that makes the light blink 5 times as a "party mode."
2. Use PWM (`analogWrite()`) and commands `1` to `5` to set the light's brightness level.
3. Add a "lock" feature: the door can only be opened after the user sends the password `x` first.
4. Add a second LED as a porch light and assign it two new letters.
