# Macro Keyboard
A 15-key Arduino-based, bluetooth wireless, mechanical macro keyboard.
I made this to improve my workflow at work and to avoid the finger gymnastics characteristic to iOS development.
# Glamour shots
# How it works
The keyboard is designed to work in conjunction with a tool like BetterTouchTool, which maps key presses to other shortcuts, based on the app in focus. 
The keyboard transmits <kbd>CMD</kbd> + <kbd>control</kbd> + <kbd>shift</kbd> + <kbd>F1</kbd> -> <kbd>F12</kbd> and <kbd>1</kbd> ->  <kbd>3</kbd>, to cover all the keys.
Using BetterTouchTool, these inputs can be mapped to various actions for each app.
Besides this, the top left, when long pressed, transmits kbd>CMD</kbd> + <kbd>control</kbd> + <kbd>shift</kbd> + <kbd>option</kbd> + <kbd>F1</kbd> and puts the keyboard in sleep mode. This is useful for setting up a shortcut to lock the Mac.

# Bill of materials
| Item | Description |
| ------ | ------ |
| 15 CHerry MX Mechanical switches with diode | I used Ebay, but the closest/cheapest the better |
| Arduino Pro Micro 32u4 (I used a clone) | Ebay |
| HC-05 bluetooth module | Anywhere, but take care to not get the "fake" one. You need to flash it with the RN-42 Firmware and the "fake" one won't work. Google is your friend here |
| Adafruit USB-C breakout | https://www.adafruit.com/product/4090 |
| 3mm LED | Used to indicate various actions. I used blue to match my case |
| 1400 MaH Lipo Battery JA-803450P | You can pick another one, but the case is designed with these dimensions in mind |
| TP4056 LiPo charger | Ebay or Aliexpress |
| Diodes | |
| P-channel mosfet | Used for LiPo bypass when charging. We don't want to drain the battery while it's charging, as this can break the LiPo |
| Resistors | Used as pull-down resistors for the keyboard matrix and as a voltage divider to check the LiPo battery level |
| Keycaps | I 3D printed some from https://www.thingiverse.com/thing:468651, but you can pick whatever you fancy |

# The circuit
The keys are set up using a keyboard matrix. The top-left key is not included in the matrix, as it has to be able to wake the Arduino from sleep.

# Missing stuff
- Sending keys through the USB connection. Technically possible and the reason why I chose a 32u4 Arduino, but still to be implemented
- LiPo voltage reading isn't the best
