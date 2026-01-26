# DALI featured snippet 
This is a branch of https://github.com/aiexr/sdvx-controller/ with a modified readme to discuss [/CAD/sdvx_controller.ino](https://github.com/aiexr/sdvx-controller/blob/dali/CAD/sdvx_controller.ino)

## Sound Voltex Controller Arduino Encoder and Button Logic
### 

</details>

### Motivation 

I built this controller primarily as a learning project: I wanted to understand the full loop from physical interaction, wiring, embedded code, input logic, to get a usable “real” interface on a computer. 

The SDVX layout was a good layout to learn because it uses discrete (buttons) and continuous (rotary encoders), and it forced me to think carefully about input mapping, and how the mechanical design, hardware, and software are all connected. 

I opted to use an Arduino for this project, since I have some experience with C++.

The arduino file (.ino) is what allows the controller to be functional. It initializes the pin modes for each pin connection on the Arduino 
(which in this case, is pins for all 7 buttons and the encoders, which require 4 pins). 

### Features

- Supports the full Sound Voltex control set (7 buttons + 2 rotary encoders, clockwise and counter-clockwise directions).

- Runs on an Arduino Leonardo

- Uses the Arduino Joystick Library to expose inputs as joystick/gamepad events

- Clear wiring/pin mapping 

- Iteration notes from testing 


### Implementation / Code Analysis

### Code Analysis
<details>
<summary>sdvx_controller.ino</summary>

```
#include <Encoder.h>
#include <Joystick.h>
#include <Keyboard.h>

/*-------------------- Physical pin assignments ------------------------*/
// Left encoder pins
const uint8_t LEFT_ENC_A = 0;  
const uint8_t LEFT_ENC_B = 1;  

// Right encoder pins
const uint8_t RIGHT_ENC_A = 2; 
const uint8_t RIGHT_ENC_B = 3; 

// Physical buttons
const uint8_t BT_1 = 13;
const uint8_t BT_2 = 12;
const uint8_t BT_3 = 11;
const uint8_t BT_4 = 10;
const uint8_t FX_L = 7;
const uint8_t FX_R = 6;
const uint8_t START_BTN = 5;
/*-------------------- Joystick button indices --------------------------*/
enum ButtonIndex : uint8_t {
  BTN_BT_1 = 4,
  BTN_BT_2 = 5,
  BTN_BT_3 = 6,
  BTN_BT_4 = 7,
  BTN_FX_L = 8,
  BTN_FX_R = 9,
  BTN_START = 10
};
const uint8_t NUM_BUTTONS = 7;
/*-------------------- Virtual joystick device setup --------------------*/
Encoder encLeft(LEFT_ENC_A, LEFT_ENC_B);
Encoder encRight(RIGHT_ENC_A, RIGHT_ENC_B);

// Joystick.h library joystick initialization
Joystick_ Joystick(
  JOYSTICK_DEFAULT_REPORT_ID,
  JOYSTICK_TYPE_GAMEPAD,
  NUM_BUTTONS, 0,           // buttons, hats
  false, false, false,      // X, Y, Z
  false, false, false,      // Rx, Ry, Rz
  false, false,             // rudder, throttle
  false, false, false       // accelerator, brake, steering

);
/*-------------------- Encoder and Keyboard configuration ---------------*/
// Detents are the physical clicks that you feel when moving an encoder.
// One detent is not typically one count.
// (i.e., the distance measured by the encoder of a change in position)

const int COUNTS_PER_DETENT = 4;  // This value might change depending on the encoder used.  
const uint16_t PULSE_MS = 5;      // This is the key press duration of one detent (ms).


enum EncAction : uint8_t {
  ENC_L_CW  = 0,   // Left encoder CW  -> arrow right
  ENC_L_CCW = 1,   // Left encoder CCW  -> arrow left
  ENC_R_CW  = 2,   // Right encoder CW -> D
  ENC_R_CCW = 3    // Right encoder CCW -> A
};

const uint8_t ENC_KEYS[4] = {
  KEY_RIGHT_ARROW, // ENC_L_CW
  KEY_LEFT_ARROW,  // ENC_L_CCW
  'd',             // ENC_R_CW
  'a'              // ENC_R_CCW
};
/*------------------------- Encoder logic & helpers ---------------------*/
// These variables keep track of the previous recorded position of the encoders.
long lastLeftDetent  = 0;
long lastRightDetent = 0;

// Encoder key pulse state initialization
bool keyPulsing[4] = {0};
uint32_t keyPulseUntil[4] = {0};

// Activates a key press from the encoder rotation
void pulseEncoderKey(uint8_t action) {
  const uint8_t key = ENC_KEYS[action];
  Keyboard.press(key);
  keyPulsing[action] = true;
  keyPulseUntil[action] = millis() + PULSE_MS;
}

// Releases a key press 
void serviceKeyPulses() {
  uint32_t now = millis();
  for (uint8_t i = 0; i < 4; i++) {
    if (keyPulsing[i] && (int32_t)(now - keyPulseUntil[i]) >= 0) {
      Keyboard.release(ENC_KEYS[i]);
      keyPulsing[i] = false;
    }
  }
}
/*------------------------- Arduino boot and loop ----------------------*/
void setup() {
  Keyboard.begin();
  Joystick.begin(false);

  // Physical buttons
  pinMode(BT_1, INPUT_PULLUP);
  pinMode(BT_2, INPUT_PULLUP);
  pinMode(BT_3, INPUT_PULLUP);
  pinMode(BT_4, INPUT_PULLUP);
  pinMode(FX_L, INPUT_PULLUP);
  pinMode(FX_R, INPUT_PULLUP);
  pinMode(START_BTN, INPUT_PULLUP);

  // Encoder pins
  pinMode(LEFT_ENC_A, INPUT_PULLUP);
  pinMode(LEFT_ENC_B, INPUT_PULLUP);
  pinMode(RIGHT_ENC_A, INPUT_PULLUP);
  pinMode(RIGHT_ENC_B, INPUT_PULLUP);

  // The input_pullup pin turns on internal pull up resistor so that the
  // readings are high and low when unpressed/press. 

  // Resets encoder position values
  encLeft.write(0);
  encRight.write(0);
  lastLeftDetent  = 0;
  lastRightDetent = 0;
}

void loop() {
  long leftDetent  = encLeft.read()  / COUNTS_PER_DETENT;
  long rightDetent = encRight.read() / COUNTS_PER_DETENT;

  while (leftDetent > lastLeftDetent)  { 
    pulseEncoderKey(ENC_L_CW);
    lastLeftDetent++; 
  } while (leftDetent < lastLeftDetent) 
  { pulseEncoderKey(ENC_L_CCW);
    lastLeftDetent--; 
  }

  while (rightDetent > lastRightDetent) {
    pulseEncoderKey(ENC_R_CW);
    lastRightDetent++; 
  } while (rightDetent < lastRightDetent) {
    pulseEncoderKey(ENC_R_CCW);
    lastRightDetent--;
  }

  serviceKeyPulses();

  // Example of the loop logic for the encoders: 
  // If we move the left encoder 3 detents CW, then leftDetent becomes positive 3.
  // Then, the condition of the while loop, leftDetent (3) > lastLeftDetent (0), is true.
  // This results in pulseEncoderKey(ENC_L_CW) being called 3 times, until 
  // leftDetent (3) is the same as lastLeftDetent (3) and the condition becomes false.
  
  bool bt1_pressed    = (digitalRead(BT_1) == LOW);
  bool bt2_pressed    = (digitalRead(BT_2) == LOW);
  bool bt3_pressed    = (digitalRead(BT_3) == LOW);
  bool bt4_pressed    = (digitalRead(BT_4) == LOW);
  bool fxL_pressed    = (digitalRead(FX_L) == LOW);
  bool fxR_pressed    = (digitalRead(FX_R) == LOW);
  bool start_pressed  = (digitalRead(START_BTN) == LOW);

  Joystick.setButton(BTN_BT_1,  bt1_pressed);
  Joystick.setButton(BTN_BT_2,  bt2_pressed);
  Joystick.setButton(BTN_BT_3,  bt3_pressed);
  Joystick.setButton(BTN_BT_4,  bt4_pressed);
  Joystick.setButton(BTN_FX_L,  fxL_pressed);
  Joystick.setButton(BTN_FX_R,  fxR_pressed);
  Joystick.setButton(BTN_START, start_pressed);

  Joystick.sendState();
}
```

Summary of the logic:

This Arduino program turns a homemade Sound Voltex controller into a “real” input device for a computer. It does two related jobs at the same time:

1) Physical controller setup with Arduino
- The controller has 7 physical push-buttons (BT1–BT4, FX-L, FX-R, Start).
- Each button pin is set to INPUT_PULLUP, which means:
  - Unpressed = HIGH 
  - Pressed = LOW 
- In every loop, the code reads each button and instantly updates a virtual gamepad button state using the [Arduino Joystick Library.](https://github.com/MHeironimus/ArduinoJoystickLibrary)
- Then it sends the full gamepad state to the connected device (Joystick.sendState()).

2) Rotary knobs (encoders) and key pressing logic
- The controller also has two rotary encoders (left and right knobs). Encoders don’t have a simple pressed/unpressed like buttons, instead they keep track of position changes as they rotate.
- The Encoder library reports a running count that changes as the knob turns.
- Because the encoder count changes multiple times per physical “click,” (that you feel) the code converts raw counts into “detents”:
  - detents = encoder_count / COUNTS_PER_DETENT
  - Here, COUNTS_PER_DETENT is set to 4, meaning 4 count changes (aka units of distance on the encoder) = 1 physical click (detent).
- The code keeps a memory of the last detent value for each knob (lastLeftDetent, lastRightDetent). Each loop it:
  - Reads the current detent position
  - Compares current detent to the last detent position
  - For every detent step forward or backward, it triggers one action 

What action happens when you rotate:
- Left knob:
  - Clockwise (CW) presses the Right Arrow key
  - Counterclockwise (CCW) presses the Left Arrow key
- Right knob:
  - Clockwise (CW) presses the 'd' key
  - Counterclockwise (CCW) presses the 'a' key

How the code makes a “press” (instead of holding the key forever):
- When a detent step is detected, pulseEncoderKey(action) is called:

```
void pulseEncoderKey(uint8_t action) {
  const uint8_t key = ENC_KEYS[action];
  Keyboard.press(key);
  keyPulsing[action] = true;
  keyPulseUntil[action] = millis() + PULSE_MS;
}
```

- It presses the corresponding keyboard key immediately (Keyboard.press()).
- It sets a timer to release this key after PULSE_MS milliseconds.
- Every loop, serviceKeyPulses() checks those timers and releases any key whose pulse time has expired (Keyboard.release()):
```
void serviceKeyPulses() {
  uint32_t now = millis();
  for (uint8_t i = 0; i < 4; i++) {
    if (keyPulsing[i] && (int32_t)(now - keyPulseUntil[i]) >= 0) {
      Keyboard.release(ENC_KEYS[i]);
      keyPulsing[i] = false;
    }
  }
```
- With PULSE_MS = 5 ms, each detent becomes a short tap, which is similar to how the actual sound voltex arcade games receive input; the encoders are simply repeating inputs, like a mouse scroll wheel. 

Overall behavior:
- Holding a physical button holds a matching virtual gamepad button on the computer.
- Turning a knob generates short discrete keyboard taps for each detent.
- The logic loop is always running while the device is plugged in. 

Libraries used:
- Arduino Joystick Library (MHeironimus): creates the virtual gamepad device.
- Encoder library: reads the rotary encoders’ position changes.
- Keyboard library: sends the knob events as keyboard key taps.

### What I Learned
- How to use a microcontroller as a functional USB device
- Implementing continous and discrete inputs
- Translating a physical button layout into code
- Learned how to debug errors in wiring using arduino watching/logging


### References/Sources
https://github.com/MHeironimus/ArduinoJoystickLibrary/blob/master/README.md 
https://github.com/PaulStoffregen/Encoder/tree/master/examples
https://docs.arduino.cc/language-reference/en/functions/usb/Keyboard/
https://www.w3schools.com/cpp/cpp_enum.asp


### Video of working controller using the arduino code

<table>
  <tr>
    <td width="50%">
      <video src="https://github.com/user-attachments/assets/48bb6534-442d-47c3-9c97-483c5b49c126" width="100%" controls></video>
    </td>
    <td width="50%">
      <video src="https://github.com/user-attachments/assets/058fed3e-3493-4807-bf1f-01e18b2e095d" width="100%" controls></video>
    </td>
  </tr>
</table>


# Beyond this point, the readme is the same as it is in the [main branch](https://github.com/aiexr/sdvx-controller/) and not specifically for the DALI full stack application.





# Overview
This project is not intended to be a guide. I am documenting how I was able to build a controller as a beginner. This project was primarily a learning experience for me, and I hope to share what I learned.

A Sound Voltex controller itself has 7 buttons and 2 rotary encoders. The photo below shows the controls of a real cabinet.

![SOUND VOLTEX controls](https://upload.wikimedia.org/wikipedia/commons/6/6e/SOUND_VOLTEX_Controls.JPG)

<small>image credit: https://ja.wikipedia.org/wiki/%E5%88%A9%E7%94%A8%E8%80%85:Sanjo</small>

# Demonstration
Below is the final product of what I made:

![Completed controller](media/controller/complete.jpg)

<table>
  <tr>
    <td width="50%">
      <video src="https://github.com/user-attachments/assets/48bb6534-442d-47c3-9c97-483c5b49c126" width="100%" controls></video>
    </td>
    <td width="50%">
      <video src="https://github.com/user-attachments/assets/058fed3e-3493-4807-bf1f-01e18b2e095d" width="100%" controls></video>
    </td>
  </tr>
</table>

# Process

I chose an [Arduino Leonardo w/ headers](https://store-usa.arduino.cc/products/arduino-leonardo-with-headers) to power all inputs.  

For the **main buttons** (BT-A, BT-B, BT-C, BT-D), I was hoping to buy 60mm buttons to make it more accurate to the actual cabinet, and I found this push button on Adafruit: https://www.adafruit.com/product/6261. 50mm would have worked fine functionally, but I wanted accurate button dimensions. The rest of the parts were purchased on Amazon. 

| Item | Qty | Image |
| --- | --- | --- |
| [Dot PCB](https://www.amazon.com/ELEGOO-Prototype-Soldering-Compatible-Arduino/dp/B072Z7Y19F/) | 1 | <img src="media/products/pcb.png" alt="PCB" height="80" /> |
| [35x22 knob](https://www.amazon.com/dp/B00X73QARO?ref=ppx_yo2ov_dt_b_fed_asin_title) | 2 | <img src="media/products/knob.png" alt="Knob" height="80" /> |
| [600p/r encoder](https://www.amazon.com/dp/B00UTIFCVA?ref=ppx_yo2ov_dt_b_fed_asin_title) | 2 | <img src="media/products/encoder.png" alt="Encoder" height="80" /> |
| [50x50 pushbutton](https://www.amazon.com/dp/B07WNFFPG5?ref=ppx_yo2ov_dt_b_fed_asin_title) | 1 | <img src="media/products/start.png" alt="Start button" height="80" /> |
| [49x33 pushbutton](https://www.amazon.com/dp/B07WQSBSWD?ref=ppx_yo2ov_dt_b_fed_asin_title) | 2 | <img src="media/products/fx.png" alt="FX buttons" height="80" /> |
| [60x60 Adafruit pushbutton](https://www.adafruit.com/product/6261) | 4 | <img src="media/products/main-buttons.png" alt="Main buttons" height="80" /> |

Filament used: ~600g of [Official Creality Hyper PLA Filament 1.75mm, High Speed PLA](https://www.amazon.com/dp/B0C2BJB256?ref=ppx_yo2ov_dt_b_fed_asin_title)

Print settings: 30% infill and 4 walls

Additionally:
- Fusion 360
- a 3D printer (used a Creality K1 Max) 
- Arduino Leonardo (used with headers)

## CAD files and printing
[Link to Fusion360 project](https://a360.co/48BC7ad), fully parametrized. 

Arduino sketch located in [`./CAD/sdvx_controller.ino`](./CAD/sdvx_controller.ino)

It requires this library: https://github.com/MHeironimus/ArduinoJoystickLibrary 

![Button plate](media/screenshots/plate.png)

The uppermost part of the board holds the buttons in place. The dimensions of the cutouts on this part of the board are designed to allow each button to snap into place. With the exceptions of BT-A through BT-D (the Adafruit button already had an additional lip on the edge of each pushbutton), each button required an additional shelf to hold the button in place after being placed in its slot. 

This piece is also divided into three separate pieces that can be printed and connected horizontally. 

![Button plate with mid piece](media/screenshots/plate_mid.png)
![Button plate without mid piece](media/screenshots/plate_no_mid.png)

Shell pieces:

![Shell](media/screenshots/shell.png)
![Shell with middle section](media/screenshots/shell_mid.png)
![Shell without middle section](media/screenshots/shell_no_mid.png)

The encoder slot is meant to be used with two additional cutouts provided in the Fusion file and uploaded to this repository. 
One piece is an insert that holds the encoder right below the cutout, and another is a small piece that boosts the Z position of the encoder so that it is flush with the board. 

![Encoder shelf](media/screenshots/encoder_shelf.png)
![Encoder step stool](media/screenshots/encoder_step_stool.png)
![Encoder mount](media/controller/encoder%20shelf.jpg)

Button and encoder knob sizes, distances, cutouts are all parametrized. There are test cutouts available to print for each of the different buttons and slots. 

![Test cutouts](media/screenshots/test_cutouts.png)

# Wiring

- Button with LED (breadboard)

| COM | GND bus |  |
| --- | --- | --- |
| NO | Arduino pin |  |
| CATHODE | NO |  |
| ANODE | +5V rail |  |
- Encoder (breadboard)

| VCC | +5V rail |  |
| --- | --- | --- |
| GND/black | GND rail |  |
| Green | D9 |  |
| White | D8 |  |

- Arduino

| D13 | BT-1 |
| --- | --- |
| D12 | BT-2 |
| D11 | BT-3 |
| D10 | BT-4 |
| D9 | Left encoder |
| D8 | Left encoder |
| D7 | FX-L |
| D6 | FX-R |
| D5 | Start |
| D4 | Right encoder |
| D3 | Right encoder |

---

## Images

![Shell and top](media/controller/shell+top.jpg)
![Long view](media/controller/long.jpg)
![Top-down view](media/controller/top.jpg)
![Wiring](media/controller/wiring.jpg)

Afterthoughts:
- The actuation force on the switches that come with each of the buttons is too high, especially for main buttons. This can easily be fixed by using a [lighter microswitch](https://paradisearcadeshop.com/products/ultra-light-touch-microswitch?variant=34672370647202)
- The encoder knob cutout design was not the best and could be made sturdier to prevent any movement when playing
