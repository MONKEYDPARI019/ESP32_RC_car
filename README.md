# ESP32 RC Car

A broken toy-grade RC car, bought from a scrap pile, with its electronics replaced by an ESP32. The car now hosts **its own WiFi network** and serves a control page — no phone app, no pairing, no internet connection needed.

Connect your phone to the WiFi network `ESP32-RC`, open the car's address in a browser, and drive.

---

## What it does

- ESP32 runs in **SoftAP mode** (Access Point) — it *is* the WiFi network, it does not join one
- Hosts a small web server on port 80 serving a touch-friendly D-pad page
- Each button sends an HTTP request (`/forward`, `/left`, …) that drives the motors
- **Hold to move, release to stop** — buttons use `ontouchstart` / `ontouchend`, so letting go stops the car
- Drive and steering are separate motor channels through one L298N motor driver

---

## Hardware

| Component | Purpose | Qty |
|---|---|---|
| ESP32 DevKit board | Brain, WiFi access point, web server | 1 |
| L298N dual H-bridge motor driver | Drives both motors in both directions | 1 |
| DC motor — drive | Rear wheels, forward/backward | 1 |
| DC motor — steering | Front wheels, left/right | 1 |
| Salvaged RC car chassis | Body, gearbox, wheels | 1 |
| 7.4V battery pack | Motor supply | 1 |

Most cheap RC cars steer with a plain DC motor pushing against a return spring rather than a servo — that is why steering here is just another H-bridge channel, not a PWM angle.

---

## Wiring

| ESP32 GPIO | L298N pin | Function |
|---|---|---|
| 27 | ENA | Drive motor speed (PWM enable) |
| 26 | IN1 | Drive direction A |
| 25 | IN2 | Drive direction B |
| 14 | ENB | Steering motor enable |
| 13 | IN3 | Steer direction A |
| 12 | IN4 | Steer direction B |

| Power | Connection |
|---|---|
| Battery + | L298N `12V` input |
| Battery − | L298N `GND` |
| L298N `5V` output | ESP32 `VIN` |
| L298N `GND` | ESP32 `GND` — **grounds must be common** |

Drive motor → L298N `OUT1`/`OUT2`. Steering motor → L298N `OUT3`/`OUT4`.

> `STEER_IN1` and `STEER_IN2` are swapped in the code relative to the obvious order. That was not a mistake to fix in hardware — it was easier to swap two lines of code than to unsolder the steering motor when left and right came out backwards.

---

## Drive it

1. Install the Arduino IDE with **ESP32 board support**, select your ESP32 board.
2. Upload `ESP32_RC_car.ino`.
3. Open the Serial Monitor at 115200 baud — it prints the car's IP address (usually `192.168.4.1`).
4. On your phone, join the WiFi network:
   - **SSID:** `ESP32-RC`
   - **Password:** `12345678`
5. Browse to that IP address and drive.

---

## Known limitations

- Speed is **on or off** — every command writes full PWM (255). No proportional throttle.
- Range is whatever the ESP32's access point covers, roughly a room.
- If the browser tab is closed mid-command the car keeps its last state until something else is sent.
- The default password is in plain text in the sketch. Change it if that matters to you.

## Possible next steps

- Proportional throttle from an on-screen joystick instead of fixed 255
- A watchdog that stops the motors if no command arrives for ~300ms
- WebSocket control instead of one HTTP request per press, for lower latency
