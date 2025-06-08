#include <LiquidCrystal.h>

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
long sumAmperage = 0;
enum MonitorState currentState = OFF;

using BoolFunc = bool(*)();

// variables will change:
int currentButtonState = 0;  // variable for reading the pushbutton status
int previousButtonState = 0;

void setup() {
  // for debugging
  Serial.begin(9600);
  lcd.begin(16, 2);
  // initialize the LED pin as an output:
  pinMode(READ_LED, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(MODE_BUTTON, INPUT);
}

void loop() {
  // read the state of the pushbutton value:
  switch(currentState){
    case OFF:
      digitalWrite(READ_LED, LOW);
      // Do nothing
      break;
    case READ:
      digitalWrite(READ_LED, HIGH);
      // Pretend we are reading from amp sensor
      sumAmperage = sumAmperage + random(-2, 5);
      break;
    case SHOW:
      // Interesting part
      // 1. We're going to stop reading inputs from the button
      // 2. We're then going to get the currentAmperage and
      //    use the SHOW_LED to tell the user what the summed amps
      printLcdMessage(sumAmperage);
      delay(1000);
      currentState = READ;
      printLcdMessage("SET SHOW TO READ");
      break;
  }

  currentButtonState = digitalRead(MODE_BUTTON);

  // When button is pressed, check what action to perform
  if (previousButtonState == LOW && currentButtonState == HIGH) {
    SetRequestedAction();
  } 
  previousButtonState = currentButtonState;

  delay(20); // for sensitivity issues
}

void ShowSummedAmps(){
  int primaryDigit = 0;
  do{

  }while(primaryDigit != 0);
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

void printLcdMessage(const char* msg){
  lcd.clear();
  lcd.print(msg);
}
void printLcdMessage(long number){
  lcd.clear();
  lcd.print(number);
}

void SetRequestedAction(){
  lcd.setCursor(0, 1);
  switch(currentState){
    case OFF:
      // The only thing we can do from OFF is start reading
      // So we wait until the user lets go of the button then we set mode to READ
      awaitCurrentButtonState(MODE_BUTTON, HIGH);
      currentState = READ;
      printLcdMessage("SET OFF TO READ");
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
        printLcdMessage("SET READ TO SHOW");
      }else{
        currentState = OFF;
        printLcdMessage("SET READ TO OFF");
      }

      // 4. Turn LED off to show user that we are about to show the reading
      digitalWrite(READ_LED, LOW);

      // 5. Wait for user to let go of button
      awaitCurrentButtonState(MODE_BUTTON, HIGH);
      break;
    case SHOW:
      // Do nothing
      // The button is disabled while the machine is showing the output
      break;
  }
  // Button is ALWAYS let go when this function ends
}




