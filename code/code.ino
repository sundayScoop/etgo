#include <LiquidCrystal.h>
#include <Adafruit_INA219.h>
#include <Wire.h>

String version = "0.0.1";

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

enum INPUT_PINS{
  MODE_BUTTON = 10
};

enum OUTPUT_PINS{
  READ_LED = 9
};

enum MonitorState{
  OFF,
  READ,
  SHOW
};

unsigned long clickSensitivityTime = 1000; // 200ms
unsigned long start = 0; // start timer memory
unsigned long end = 0; // end timer memory
unsigned long lastCheck = 0;
unsigned long showMessageCheck = 0;
float current_mW = 0;
long total_milliJoules = 0;

enum MonitorState currentState = OFF;

using BoolFunc = bool(*)();

// variables will change:
int currentButtonState = 0;  // variable for reading the pushbutton status
int previousButtonState = 0;


Adafruit_INA219 ina219;

void setup() {
  // for debugging
  Serial.begin(9600);
  lcd.begin(16, 2);
  // initialize the LED pin as an output:
  pinMode(READ_LED, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(MODE_BUTTON, INPUT);

  // Initialize the INA219.
  if (!ina219.begin())
  {
    Serial.println("Failed to find INA219 chip");
    while (1) 
    {
      delay(10);
    }
  }
  ina219.setCalibration_16V_400mA();
}

void loop() {
  // read the state of the pushbutton value:
  switch(currentState){
    case OFF:
      digitalWrite(READ_LED, LOW);
      // Do nothing
      printOffMessage();
      break;
    case READ:
      // Do the most important thingo
      calculateTotalJoules();

      digitalWrite(READ_LED, HIGH); // indicate we are reading
      printReadMessage();
      break;
    case SHOW:
      // Interesting part
      // 1. We're going to stop reading inputs from the button
      // 2. We're then going to get the currentAmperage and
      //    use the SHOW_LED to tell the user what the summed amps
      printShowMessage();
      if(millis() - showMessageCheck > 7000){
        currentState = READ;
      }
      break;
  }

  currentButtonState = digitalRead(MODE_BUTTON);

  // When button is pressed, check what action to perform
  if (previousButtonState == LOW && currentButtonState == HIGH) {
    SetRequestedAction();
  } 
  previousButtonState = currentButtonState;

  // make a class so that this doesn't affect the sensor reading. we should be reading from sensor as max speed
  delay(50); // for sensitivity issues
}

void calculateTotalJoules(){
  // Interval is 100ms
  if(millis() - lastCheck > 100){
    current_mW = ina219.getPower_mW();
    total_milliJoules += (long)(current_mW * 100); // times 100 since we measure every 100 ms   mJ = mW * mS
    lastCheck = millis(); // update last check
  }
}

void awaitCurrentButtonState(uint8_t button, uint8_t level){
  while(currentButtonState == level){
    currentButtonState = digitalRead(button);
    delay(5);
  }
}
void awaitCurrentButtonState(uint8_t button, uint8_t level, BoolFunc func){
  while(currentButtonState == level && func()){
    currentButtonState = digitalRead(button);
    delay(5);
  }
}

void printOffMessage(){
  lcd.clear();
  lcd.print("Click button");
  lcd.setCursor(0, 1);
  lcd.print("to turn on");
}

void printReadMessage(){
  lcd.clear();
  lcd.print(String("Current mW: ") + String(current_mW));
  lcd.setCursor(0, 1);
  lcd.print(String("Joules: ") + String(total_milliJoules / 1000));
}

void printShowMessage(){
  lcd.clear();
  lcd.print(String("Uptime: ") + String(millis() / 1000) + String("s"));
  lcd.setCursor(0, 1);
  lcd.print(String("Version: ") + version);
}

void printLcdMessage(const char* msg){
  lcd.clear();
  lcd.print(msg);
}
void printLcdMessage(long number){
  lcd.clear();
  lcd.print(number);
}
void printLcdMessage(float number){
  lcd.clear();
  lcd.print(number, 2);
}

void SetRequestedAction(){
  lcd.setCursor(0, 1);
  switch(currentState){
    case OFF:
      // The only thing we can do from OFF is start reading
      // So we wait until the user lets go of the button then we set mode to READ
      awaitCurrentButtonState(MODE_BUTTON, HIGH);
      currentState = READ;
      total_milliJoules = 0; // restart count
      break;
    case READ:
      // We can either turn OFF, or SHOW the current reading
      // We determine which one based on how long the button is clicked
      // 1. Start a timer
      start = millis();

      // 2. Wait for user to release button OR until click sensitivity time has passed
      auto timeCheck = []() -> bool {
        return millis() - start < clickSensitivityTime;
      };
      awaitCurrentButtonState(MODE_BUTTON, HIGH, timeCheck);

      // 3. If the button is still pressed, set state to SHOW
      // else set the state to OFF
      if(currentButtonState == HIGH){
        currentState = SHOW;
      }else{
        currentState = OFF;
      }

      // 4. Turn LED off to show user that we are about to show the reading
      digitalWrite(READ_LED, LOW);

      // 5. Wait for user to let go of button
      awaitCurrentButtonState(MODE_BUTTON, HIGH);

      if(currentState == SHOW){
        showMessageCheck = millis(); // start a "timer" so main loop knows when to switch back to read mode
      }
      break;
    case SHOW:
      // Do nothing
      // The button is disabled while the machine is showing the output
      break;
  }
  // Button is ALWAYS let go when this function ends
}




