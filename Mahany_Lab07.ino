int buttonPin[4] = {29, 28, 31, 32}; // setting up buttton pins as arrays
int ledPin[4] = {35, 36, 37, 38}; // setting up led pins as arrays
boolean lastButtonState[4] = {LOW, LOW, LOW, LOW}; // setting up default button state as array
boolean buttonState[4] = {LOW, LOW, LOW, LOW}; // setting up current button state as array
boolean stepState[3][4] = {
  {false, false, false, false},
  {false, true, false, true},
  {false, true, true, true},
};
int currentStep = 0; // starting the step at 0
unsigned long lastStepTime = 0;
int speedPot = 0; // setting up default speed pot at 0
int tempo = 0; // default tempo at 0
int switchUp = 26; // setting up switches
int switchDown = 27;
boolean lastUpState = LOW; // setting up default state of switches
boolean lastDownState = LOW;
boolean upState = LOW; // setting up current state if switches
boolean downState = LOW;
int channelCounter = 0; // initializing channel counter

void setup() {
  pinMode(buttonPin[0], INPUT); // setting up I/O types
  pinMode(buttonPin[1], INPUT);
  pinMode(buttonPin[2], INPUT);
  pinMode(buttonPin[3], INPUT);
  pinMode(ledPin[0], OUTPUT); 
  pinMode(ledPin[1], OUTPUT);
  pinMode(ledPin[2], OUTPUT);
  pinMode(ledPin[3], OUTPUT);
  pinMode(switchUp, INPUT);
  pinMode(switchDown, INPUT);
}

void loop() { // main function with other functions

  channel();
  checkButtons();
  sequence();
  setLeds();
}
void channel () { // sets channel counter based on which buttons are pressed
  lastUpState = upState;
  upState = digitalRead(switchUp);
  lastDownState = downState;
  downState = digitalRead(switchDown);
  if (upState == HIGH && lastUpState == LOW) { // if up button is pressed, adds one
    channelCounter = channelCounter + 1;
    if (channelCounter == 3) {
      channelCounter = 0;
    }
  }
  if (downState == HIGH && lastDownState == LOW) { // if down button is pressed, subtract one
    channelCounter = channelCounter - 1;
    if (channelCounter == -1) {
      channelCounter = 2;
    }
  }
}

void checkButtons() { // function to tell if button has been pressed
  for (int i = 0; i < 4; i++) { // checking all buttons using for loop to cycle through
    lastButtonState[i] = buttonState[i];
    buttonState[i] = digitalRead(buttonPin[i]);
    if (buttonState[i] == HIGH && lastButtonState[i] == LOW) {

      if (stepState[channelCounter][i] == false) {

        stepState[channelCounter][i] = true;
      } else if (stepState[channelCounter][i] == true) {

        stepState[channelCounter][i] = false;
      }
    }
  }
}
void sequence() {

  speedPot = analogRead(A9); // reading in and mapping speed of sequencer
  tempo = map(speedPot, 0, 1023, 500, 1000);

  if (millis() > lastStepTime + tempo) {   //if its time to go to the next step...

    Serial.println("...");
    //digitalWrite(ledPin[currentStep], LOW);  //turn off the current led

    currentStep = currentStep + 1;         //increment to the next step
    if (currentStep > 3) {
      currentStep = 0;
    }

    if (stepState[0][currentStep] == HIGH ) { // if button is pressed, send midi note
      usbMIDI.sendNoteOn(60, 127, 1);
      Serial.println("0");

    }
    if (stepState[1][currentStep] == HIGH ) { // if button is pressed, send midi note
      usbMIDI.sendNoteOn(62, 127, 1);
      Serial.println("1");

    }
    if (stepState[2][currentStep] == HIGH ) { // if button is pressed, send midi note
      usbMIDI.sendNoteOn(64, 127, 1);
      Serial.println("2");

    }
    //digitalWrite(ledPin[currentStep], HIGH); //turn on the new led

    lastStepTime = millis();               //set lastStepTime to the current time
  }
}
void setLeds() {
  for (int i = 0; i < 4; i++) {
    if (currentStep == i) { // writes the current step of the led as bright, but less so
      analogWrite(ledPin[i], 30);
    }
    else if (stepState[channelCounter][i] == HIGH) { // write high if recently pressed
      analogWrite(ledPin[i], 250);


    }
    else {
      analogWrite(ledPin[i], 0); // if not, write low
    }
  }
}
