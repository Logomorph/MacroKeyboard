# Macro Keyboard
A 15-key Arduino-based, bluetooth wireless, mechanical macro keyboard.
I made this to improve my workflow at work and to avoid the finger gymnastics characteristic to iOS development. It charges and can be programed on-the-fly via USB-C.
# Glamour shots
![](/rsc/final3.jpg)
![](/rsc/final1.jpg)
![](/rsc/final2.jpg)
# How it works
The keyboard is designed to work in conjunction with a tool like BetterTouchTool, which maps key presses to other shortcuts, based on the app in focus. Although I haven't tested it (lacking a PC with bluetooth), this should work just fine on Windows using a tool like AutoHotkey

The keyboard transmits <kbd>CMD</kbd> + <kbd>control</kbd> + <kbd>shift</kbd> + <kbd>F1</kbd> -> <kbd>F12</kbd> and <kbd>1</kbd> ->  <kbd>3</kbd>, to cover all the keys.

Using BetterTouchTool, these inputs can be mapped to various actions for each app.

Besides this, the top left key "master", when long pressed, transmits <kbd>CMD</kbd> + <kbd>control</kbd> + <kbd>shift</kbd> + <kbd>option</kbd> + <kbd>F1</kbd> and puts the keyboard in sleep mode. This is useful for setting up a shortcut to lock the Mac.

After 5 minutes where no keys are pressed, the keyboard turns off the bluetooth module and the arduino goes to deep sleep. The "master" key acts as a wake-up key and will turn the arduino and the bluetooth back on.
In my use, the battery lasts about 2-3 weeks.

At startup, holding the *master* key and the first key on its right and then turning on the power, will force *Pairing mode*
Holding *master* and the second key on its right will clear all paired devices.
# Folder structure

    .
    ├── src                         # source code for the arduino
    └── stl                         # STL files for the case and the lid

# Bill of materials
| Item | Description |
| ------ | ------ |
| 15 CHerry MX Mechanical switches with diode | I used Ebay, but the closest/cheapest the better |
| Arduino Pro Micro 32u4 (I used a clone) | Ebay |
| HC-05 bluetooth module | Take care to not get the "fake" one. You need to flash it with the RN-42 Firmware and the "fake" one won't work. Google is your friend here |
| Adafruit USB-C breakout | https://www.adafruit.com/product/4090 |
| 3mm LED | Used to indicate various actions. I used blue to match my case |
| 1400 MaH Lipo Battery JA-803450P | You can pick another one, but the case is designed with these dimensions in mind |
| TP4056 LiPo charger | Ebay or Aliexpress |
| SPDT Switch | https://www.adafruit.com/product/805 |
| Diodes | |
| P-channel mosfet | Used for LiPo bypass when charging. We don't want to drain the battery while it's charging, as this can break the LiPo |
| Resistors | Used as pull-down resistors for the keyboard matrix and as a voltage divider to check the LiPo battery level |
| Keycaps | I 3D printed some from https://www.thingiverse.com/thing:468651, but you can pick whatever you fancy |
| Spare micro-USB cable | Canibalized to link the Arduino to the USB-C breakout board. This allows programming without taking the keyboard apart. |

# The layout
While the box does fit all the required components, albeit it's a tight fit, I designed it for the components I used. You can see its layout in the diagram below.
![](/rsc/layout_diagram.png)

# The circuit
The keys are set up using a keyboard matrix. The top-left key is not included in the matrix, as it has to be able to wake the Arduino from sleep. It is set up as an interrupt.

Since the LiPo charger doesn't have a bypass for when it's plugged in, and it's dangerous to charge a LiPo while draining it, a bypass circuit is needed. I used the one described here and it works perfectly: https://arduino.stackexchange.com/questions/39805/can-you-charge-and-use-a-lipo-battery-at-the-same-time.

# Missing stuff
- Circuit diagram (coming soon)
- Sending keys through the USB connection. Technically possible and the reason why I chose a 32u4 Arduino, but still to be implemented
- LiPo voltage reading isn't the best
- This could easily be implemented with an Adafruit Feather 32u4 Bluefruit (https://learn.adafruit.com/adafruit-feather-32u4-bluefruit-le/overview). It has on-board bluetooth, LiPo voltage reading and reporting along with a battery charger. It should simplify the circuit, fit fine in the case and could allow for a larger battery, too.