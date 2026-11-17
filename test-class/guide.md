# Ultrasonic Parking Sensor

**Platform:** Tinkercad Circuits · **Board:** Arduino Uno R3 · **Level:** Intermediate

---

## Word Problem

Mang Ruben owns a small garage. His customers keep bumping their cars into the back wall while parking. He asks you to build a **parking assist device** using an Arduino.

The device must measure the distance between the car and the wall, then warn the driver:

| Distance | Zone | LED | Buzzer |
|---|---|---|---|
| More than 100 cm | SAFE | Green | Silent |
| 50 – 100 cm | CAUTION | Yellow | Slow beeps |
| 20 – 50 cm | WARNING | Red | Fast beeps |
| Less than 20 cm | STOP | Red | Continuous tone |

The beeps should get **gradually faster** as the car gets closer, not just jump between speeds. The distance should also be printed to the Serial Monitor so Mang Ruben can check the readings.

### Guide Questions

1. Sound travels at about 343 m/s (0.0343 cm/µs). If the echo takes 2,900 µs to return, how far is the object, and which zone is it in?
2. Why do we divide the echo travel time by 2?
3. Why is `millis()` better than `delay()` for timing the beeps?

---

## Components

This activity uses the following components in Tinkercad Circuits: one Arduino Uno R3 as the main microcontroller, one Ultrasonic Distance Sensor (4-pin, HC-SR04) to measure the distance to an obstacle, one Piezo buzzer for the audible warning beeps, three LEDs (one green, one yellow, and one red) to show the SAFE, CAUTION, and WARNING/STOP zones, three 220 Ω resistors to limit the current through each LED, one small breadboard to hold and connect the components, and jumper wires to link everything to the Arduino.

---

## Pin Mapping

Connect the Arduino's 5V pin to the breadboard's positive rail and the GND pin to the negative rail. Wire the ultrasonic sensor's VCC pin to the positive rail and its GND pin to the negative rail. Connect its TRIG pin to digital pin 9 and its ECHO pin to digital pin 10. Connect the piezo buzzer's positive leg to digital pin 8 and its negative leg to the GND rail. For the LEDs, connect digital pin 4 through a 220 Ω resistor to the anode (long leg) of the green LED, digital pin 5 through a 220 Ω resistor to the anode of the yellow LED, and digital pin 6 through a 220 Ω resistor to the anode of the red LED. Connect the cathode (short leg) of each LED to the GND rail.

### Circuit Diagram

```
                 ARDUINO UNO
              ┌──────────────┐
   HC-SR04    │              │
  ┌───────┐   │              │
  │ VCC ──┼───┤ 5V           │
  │ TRIG ─┼───┤ D9           │
  │ ECHO ─┼───┤ D10          │
  │ GND ──┼───┤ GND ─────────┼──────────────────┐ (GND rail)
  └───────┘   │              │                  │
              │ D8 ──────────┼── (+)PIEZO(−) ───┤
              │              │                  │
              │ D4 ──[220Ω]──┼──▶|── GREEN  ────┤
              │ D5 ──[220Ω]──┼──▶|── YELLOW ────┤
              │ D6 ──[220Ω]──┼──▶|── RED    ────┘
              └──────────────┘

  ▶| = LED (anode/long leg on the resistor side, cathode/short leg to GND)
```

---

## Code

In Tinkercad, open the **Code** panel, switch the editor from **Blocks** to **Text**, delete the default code, and paste the following:

```cpp
/*
  Ultrasonic Parking Sensor (Tinkercad Circuits / Arduino Uno)
  ------------------------------------------------------------
  HC-SR04 measures distance to an obstacle.
  LEDs show the zone; the buzzer beeps faster as the object gets closer.

  Zones:
    > 100 cm       SAFE     green LED, silent
    50 - 100 cm    CAUTION  yellow LED, slow beeps
    20 - 50 cm     WARNING  red LED, fast beeps
    < 20 cm        STOP     red LED, continuous tone
*/

// ---------- Pin assignments ----------
const int TRIG_PIN   = 9;
const int ECHO_PIN   = 10;
const int BUZZER_PIN = 8;
const int GREEN_LED  = 4;
const int YELLOW_LED = 5;
const int RED_LED    = 6;

// ---------- Distance thresholds (cm) ----------
const int FAR_CM  = 100;
const int MID_CM  = 50;
const int NEAR_CM = 20;

// ---------- Timing ----------
const unsigned long MEASURE_INTERVAL = 60;  // ms between readings (HC-SR04 needs ~60 ms)
const unsigned long BEEP_MS          = 50;  // length of each beep
const int BEEP_FREQ                  = 1000; // Hz
const int STOP_FREQ                  = 1500; // Hz for continuous tone

unsigned long lastMeasure = 0;
unsigned long lastBeep    = 0;
bool buzzerOn             = false;
long distanceCm           = -1;

// Send a 10 us trigger pulse and convert the echo time to centimetres.
// Returns -1 if no echo is received (nothing in range).
long readDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000UL); // 30 ms timeout (~5 m)
  if (duration == 0) return -1;

  // Speed of sound = 0.0343 cm/us; divide by 2 for the round trip
  return (long)(duration * 0.0343 / 2.0);
}

void setLeds(bool g, bool y, bool r) {
  digitalWrite(GREEN_LED,  g);
  digitalWrite(YELLOW_LED, y);
  digitalWrite(RED_LED,    r);
}

// Non-blocking beep: a short beep every `interval` ms.
void beepEvery(unsigned long interval) {
  unsigned long now = millis();
  if (!buzzerOn && now - lastBeep >= interval) {
    tone(BUZZER_PIN, BEEP_FREQ);
    buzzerOn = true;
    lastBeep = now;
  } else if (buzzerOn && now - lastBeep >= BEEP_MS) {
    noTone(BUZZER_PIN);
    buzzerOn = false;
  }
}

void buzzerOff() {
  noTone(BUZZER_PIN);
  buzzerOn = false;
}

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  Serial.begin(9600);
  Serial.println("Parking sensor ready");
}

void loop() {
  unsigned long now = millis();

  // 1) Take a new reading every MEASURE_INTERVAL ms
  if (now - lastMeasure >= MEASURE_INTERVAL) {
    lastMeasure = now;
    distanceCm = readDistanceCm();

    Serial.print("Distance: ");
    if (distanceCm < 0) Serial.println("out of range");
    else { Serial.print(distanceCm); Serial.println(" cm"); }
  }

  // 2) Decide the zone and drive LEDs + buzzer
  if (distanceCm < 0 || distanceCm > FAR_CM) {
    // SAFE
    setLeds(true, false, false);
    buzzerOff();
  }
  else if (distanceCm > MID_CM) {
    // CAUTION: 700 ms at 100 cm -> 300 ms at 50 cm
    setLeds(false, true, false);
    beepEvery(map(distanceCm, MID_CM, FAR_CM, 300, 700));
  }
  else if (distanceCm > NEAR_CM) {
    // WARNING: 300 ms at 50 cm -> 80 ms at 20 cm
    setLeds(false, false, true);
    beepEvery(map(distanceCm, NEAR_CM, MID_CM, 80, 300));
  }
  else {
    // STOP: continuous tone
    setLeds(false, false, true);
    tone(BUZZER_PIN, STOP_FREQ);
    buzzerOn = true;
    lastBeep = millis();
  }
}
```

### How It Works

The Arduino sends a 10 µs pulse on the TRIG pin, which makes the sensor emit an ultrasonic burst. The ECHO pin stays HIGH for as long as the sound takes to bounce back. Multiplying that time by the speed of sound (0.0343 cm/µs) gives the round-trip distance, and dividing by 2 gives the one-way distance to the object. The `map()` function converts the distance into a beep interval, so the beeps speed up smoothly as the object gets closer. Using `millis()` instead of `delay()` lets the Arduino keep reading the sensor and updating the LEDs while the buzzer is beeping.

---

## Testing in Tinkercad

1. Click **Start Simulation**.
2. Click the ultrasonic sensor. A circular "object" appears in front of it.
3. Drag the object closer to and farther from the sensor. The LEDs and beep speed should change with the distance.
4. Open the **Serial Monitor** at the bottom of the Code panel to see the distance readings.

---

## Extension Challenges (Optional)

1. Add a 16×2 LCD that displays the distance and the current zone.
2. Use a potentiometer to adjust the STOP distance without changing the code.
3. Average 5 readings to smooth out jumpy distance values.
