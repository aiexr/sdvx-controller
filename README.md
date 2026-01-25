# Overview
This project is not intended to be a guide. I am documenting how I was able to build a controller as a beginner. This project was primarily a learning experience for me, and I hope to share what I learned.

A Sound Voltex controller itself has 7 buttons and 2 rotary encoders. The photo below shows the controls of a real cabinet.

![SOUND VOLTEX controls](https://upload.wikimedia.org/wikipedia/commons/6/6e/SOUND_VOLTEX_Controls.JPG)

<small>image credit: https://ja.wikipedia.org/wiki/%E5%88%A9%E7%94%A8%E8%80%85:Sanjo</small>

# Demonstration
Below is the final product of what I made:

![Completed controller](media/controller/complete.jpg)

<video src="https://github.com/aiexr/sdvx-controller/blob/main/media/videos/simplegameplay.mp4" width="80%" controls></video>
<video src="https://github.com/aiexr/sdvx-controller/blob/main/media/videos/encodermovement.mp4" width="80%" controls></video>

# Process

I chose to use an [Arduino Leonardo w/ headers](https://store-usa.arduino.cc/products/arduino-leonardo-with-headers) to power all inputs.  

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
