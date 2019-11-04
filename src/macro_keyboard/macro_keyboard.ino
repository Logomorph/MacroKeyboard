#include <LowPower.h>
#include <SoftwareSerial.h>
#include <Keyboard.h>
#include <avr/sleep.h>
#include <avr/power.h>

int r[3] = { 9, 8, 10 };
int c[5] = { 6, 5, 4, 7, 2};
int masterKey = 3;
int statusLed = A2;
int internalLed = 17;
int btPower = A3;
int lipoVoltage = A0;
int sleepTriggerThreshold = 2000; // 2 seconds
unsigned long sleepThreshold = 300000; // 5 minutes
bool sentEmptyKey;
unsigned long lastKeyPress;

#define RX_PIN 16
#define TX_PIN 14

//F1 F2 F3 F4 F5 F6 F7 F8 F9 F10 F11 F12 1 2 3
byte btKeys[15] = { 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x1E, 0x1F, 0x20 };

SoftwareSerial btSerial(RX_PIN, TX_PIN);

struct KeyInfo {
  unsigned long val;
  unsigned long last;
};
KeyInfo matrix[15];

// Battery management
float lipoValue = 0.0;
unsigned long lastBatteryCheck = 0;
unsigned long lastBatteryWarning = 0;
bool isLowBattery = false;
bool isEmptyBattery = false;

void setup() {
  lastKeyPress = millis();
  pinMode(lipoVoltage, INPUT);
  pinMode(statusLed, OUTPUT);
  digitalWrite(statusLed, LOW);
  pinMode(btPower, OUTPUT);
  digitalWrite(btPower, HIGH);
  enableStatusLed(true);
  resetKeyboardMatrix();
  for (int i = 0; i < 3; i++) {
    pinMode(r[i], OUTPUT);
    digitalWrite(r[i], LOW);
  }
  for (int i = 0; i < 5; i++) {
    pinMode(c[i], INPUT);
    digitalWrite(c[i], LOW);
  }
  pinMode(masterKey, INPUT);
  digitalWrite(masterKey, LOW);
  
  Serial.begin(115200);
  enableBluetooth(true);
  readKeyboardData();
  processStartupHotkeys();
}

void loop() {
  readKeyboardData();
  processKeyPresses();
  monitorBattery();
  
  while (btSerial.available() > 0) {
    Serial.write(btSerial.read());
  }
  while (Serial.available() > 0) {
    btSerial.write(Serial.read());
  }
  if (millis() - lastKeyPress > sleepThreshold) {
    enterSleep();
  }
}

void monitorBattery() {
  unsigned long currentTime = millis();
  // Only check the voltage every 10 seconds
  if (currentTime - lastBatteryCheck > 10000 && !isLowBattery && !isEmptyBattery) {
    return;
  }
  lastBatteryCheck = currentTime;
  lipoValue = analogRead(lipoVoltage);
  lipoValue = (lipoValue * 2.0f *  3.3f) / 1024.0f;
  if (lipoValue < 3.2) {
    isLowBattery = true;
  }
  if (lipoValue < 3.1) {
    isEmptyBattery = true;
  }
  if (isEmptyBattery && currentTime - lastBatteryWarning > 1000) {
    enableStatusLed(true);
    lastBatteryWarning = currentTime;
  } else if (isLowBattery && currentTime - lastBatteryWarning > 3000) {
    enableStatusLed(true);
    lastBatteryWarning = currentTime;
  }
}

void resetKeyboardMatrix() {
  for (int i = 0; i < 15; i++) {
    matrix[i].val = 0;
    matrix[i].last = 0;
  }
}

void readKeyboardData() {
  int val = digitalRead(masterKey);
  processKey(0, 0, val);
  for (int i = 0; i < 3; i++) {
    digitalWrite(r[i], HIGH);
    for (int j = 0; j < 5; j++) {
      if (i == 0 && j == 0) {
        continue;
      }
      int val = digitalRead(c[j]);
      processKey(i, j, val);
    }
    digitalWrite(r[i], LOW);
  }
  enableStatusLed(false);
}

void processKey(int i, int j, int val) {
  int index = i*5 + j;
  if (val == 1) {
    if (matrix[index].val == 0 ) {
      Serial.print(i);
      Serial.print(" ");
      Serial.println(j);
      matrix[index].val = millis();
      enableStatusLed(true);
    }
  } else {
    matrix[index].val = 0;
  }
}

void processStartupHotkeys() {
  enableStatusLed(true);
  if (matrix[0].val != 0 && matrix[2].val != 0) {
    btClearPairedDevices();
  }
  if (matrix[0].val != 0 && matrix[1].val != 0) {
    btEnterPairMode();
  }
  enableStatusLed(false);
}

void processKeyPresses() {
  unsigned long currentTime = millis();
  if (currentTime - matrix[0].val > sleepTriggerThreshold && matrix[0].val != 0) {
    if (matrix[1].val != 0) {
      btSerial.print("Logokeys voltage: ");
      btSerial.println(lipoValue);
      delay(1000);
      resetKeyboardMatrix();
      return;
    }
    resetKeyboardMatrix();

    byte data[] = {0xFD, 0x9, 0x1, (byte)0b00001111, (byte)0x0,(byte)0x3A,(byte)0x0,(byte)0x0,(byte)0x0,(byte)0x0,(byte)0x0,(byte)0x0,(byte)0x0};
    btSerial.write(data, sizeof(data));

    byte data2[] = {0xFD, 0x9, 0x1, (byte)0x0, (byte)0x0,(byte)0x0,(byte)0x0,(byte)0x0,(byte)0x0,(byte)0x0,(byte)0x0,(byte)0x0,(byte)0x0};
    btSerial.write(data2, sizeof(data2));
    
    enableStatusLed(true);
    delay(1000);
    enableStatusLed(false);
    enterSleep();
  } else {
    byte data[] = {0xFE, 0x5, (byte)0b0000111, (byte)0x0, (byte)0x0, (byte)0x0, (byte)0x0, (byte)0x0};
    byte keysDown = 0;
    int index = 3;
    for (int i=0; i<15; i++) {
      if (matrix[i].val != 0 && matrix[i].last == 0 && keysDown < 5) {
        data[index] = btKeys[i];
        keysDown++;
        index++;
        matrix[i].last = matrix[i].val;
        lastKeyPress = currentTime;
      }
      if (matrix[i].val == 0 && matrix[i].last != 0) {
        matrix[i].last = 0;
      }
    }
    if (keysDown != 0) {
      sentEmptyKey = false;
    }
    if (keysDown == 0 && !sentEmptyKey) {
      sentEmptyKey = true;
      byte empty[] = {0xFE, 0x0};
      btSerial.write(empty, sizeof(empty));
    }
    if (keysDown == 0 && sentEmptyKey) {
      return;
    }
    byte* finalArray = (byte*) malloc((keysDown+3) * sizeof(byte));
    
    finalArray[1] = keysDown;
    for (byte i=0;i<keysDown+3;i++) {
      if (i == 1) {
        finalArray[i] = keysDown + 1;
      } else {
        finalArray[i] = data[i];
      }
      byte d = finalArray[i];
      Serial.print(d, HEX);
      Serial.print(" ");
    }
    Serial.println();
    btSerial.write(finalArray, (keysDown+3) * sizeof(byte));
    delete [] finalArray;
  }
}

void btEnterPairMode() {
  btSerial.print("CMD");
  delay(100);
  btSerial.println("SM,6");
  delay(100);
  btSerial.println("---");
}

void btClearPairedDevices() {
  btSerial.print("CMD");
  delay(100);
  btSerial.println("SR,Z");
  delay(100);
  btSerial.println("---");
}

void enableBluetooth(bool enabled) {
  if (enabled) {
    digitalWrite(btPower, HIGH);
    btSerial.begin(115200);
  } else {
    digitalWrite(btPower, LOW);
    btSerial.end();
  }
}

void enableStatusLed(bool enabled) {
  digitalWrite(statusLed, enabled ? HIGH : LOW);
}

void enterSleep()
{
  attachInterrupt(digitalPinToInterrupt(3), wakeInterrupt, RISING);
  enableBluetooth(false);
  delay(100);
  
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); 


  lastKeyPress = millis ();
  Serial.begin(115200);
  enableBluetooth(true);
  enableStatusLed(true);
  delay(200);
}

void wakeInterrupt(void) { }
