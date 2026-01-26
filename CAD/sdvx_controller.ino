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