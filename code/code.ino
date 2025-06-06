enum INPUT_PINS{
  MODE_BUTTON = 8
};

enum OUTPUT_PINS{
  READ_LED = 7
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

// variables will change:
int currentButtonState = 0;  // variable for reading the pushbutton status
int previousButtonState = 0;

void setup() {
  // for debugging
  Serial.begin(9600);
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
      Serial.print("We would be showing now! ");
      Serial.println(sumAmperage);
      delay(1000);
      currentState = READ;
      Serial.println("SET SHOW TO READ");
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

// void ShowSummedAmps(){
//   int primaryDigit = 0;
//   do{

//   }while(primaryDigit != 0);
// }


int readButtonState(int button){
  delay(1);
  return digitalRead(button);
}

void SetRequestedAction(){
  switch(currentState){
    case OFF:
      // The only thing we can do from OFF is start reading
      // So we wait until the user lets go of the button then we set mode to READ
      while(currentButtonState == HIGH){
        currentButtonState = digitalRead(MODE_BUTTON);
      }
      currentState = READ;
      Serial.println("SET OFF TO READ");
      break;
    case READ:
      // We can either turn OFF, or SHOW the current reading
      // We determine which one based on how long the button is clicked
      // 1. Start a timer
      start = millis();

      // 2. Wait for user to release button OR until click sensitivity time has passed
      while(currentButtonState == HIGH && millis() - start < clickSensitivityTime){
        currentButtonState = digitalRead(MODE_BUTTON);
      }

      // 3. If the button is still pressed, set state to SHOW
      // else set the state to OFF
      if(currentButtonState == HIGH){
        currentState = SHOW;
        Serial.println("SET READ TO SHOW");
      }else{
        currentState = OFF;
        Serial.println("SET READ TO OFF");
      }

      // 4. Turn LED off to show user that we are about to show the reading
      digitalWrite(READ_LED, LOW);

      // 5. Wait for user to let go of button
      while(currentButtonState == HIGH){
        currentButtonState = digitalRead(MODE_BUTTON);
      }
      break;
    case SHOW:
      // Do nothing
      // The button is disabled while the machine is showing the output
      break;
  }
  // Button is ALWAYS let go when this function ends
}




