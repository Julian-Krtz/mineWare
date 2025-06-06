/*"THE BEER-WARE LICENSE" (Revision 42):
Julian-Krtz wrote this file. As long as you retain this notice you
can do whatever you want with this stuff. If we meet some day, and you think
this stuff is worth it, you can buy me a beer in return*/

#include <Servo.h>
#include <Bounce2.h>


enum MineState {
  STATE_ARMING,
  STATE_ARMED,
  STATE_TRIGGERED,
  STATE_UNARMED,
  STATE_INIT
};

const char* stateToString(MineState s) {
  switch (s) {
    case STATE_UNARMED:   return "UNARMED";
    case STATE_ARMING:    return "ARMING";
    case STATE_ARMED:     return "ARMED";
    case STATE_TRIGGERED: return "TRIGGERED";
    default:              return "UNKNOWN";
  }
}

MineState currentState;
volatile MineState newState;

int pirPin = 2;     
int buttonPin = 3; 
int ledSoundPin = 4;

bool isHeld = false;
int clickCount = 0;
int delayCount = 0;
unsigned long pressStartTime = 0;
unsigned long lastClickTime = 0;

const unsigned long longPressDuration = 1000;
const unsigned long clickEvalTimeout = 1500;

bool testingEnvironment = true;

Servo triggerServo;

Bounce debouncer = Bounce();

int repsRemaining = 0;
int onDuration = 0;
int offDuration = 0;
bool currentlyOn = false;
unsigned long lastBlink = 0;

void setup() {

  if (testingEnvironment) {
    Serial.begin(9600);        
  }

  currentState = STATE_INIT;
  newState = STATE_UNARMED;

  pinMode(pirPin, INPUT_PULLUP);    
  pinMode(buttonPin, INPUT_PULLUP);    
  pinMode(ledSoundPin, OUTPUT);

  triggerServo.attach(9);        
  triggerServo.write(180);  

  debouncer.attach(buttonPin);
  debouncer.interval(25);
  
  attachInterrupt(digitalPinToInterrupt(pirPin), trigger, RISING);

}

void loop() {

  if(currentState == STATE_ARMING && lastBlinkFinished()) {
    newState = STATE_ARMED;
  }

  if(currentState != newState) {
    enterState(newState);
  }

  debouncer.update();

  if (debouncer.fell()) {
    Serial.println("Pressed");
    pressStartTime = millis();
    isHeld = false;
  }

  if (!isHeld && debouncer.read() == LOW && (millis() - pressStartTime >= longPressDuration)) {
    Serial.println("Long press");
    evaluateLongPress();
    clickCount = 0;
    isHeld = true;
  }

  if (debouncer.rose()) {
    if (millis() - pressStartTime < longPressDuration) {
      clickCount++;
      lastClickTime = millis();
      Serial.print("Short press(es): ");
      Serial.println(clickCount);
      isHeld = false;
    }
  }

  if (clickCount > 0 && millis() - lastClickTime >= clickEvalTimeout) {
    evaluateClicks();
    delayCount = clickCount;
    clickCount = 0;
    Serial.println("evaluating");
  }

  updateBlinking(ledSoundPin);
}

void enterState(MineState executeState) {

  Serial.print("Mine wechselt in Status ");
  Serial.println(stateToString(executeState));

  currentState = executeState;

  switch(executeState) {
    case STATE_ARMING:       
      onOffSound(200, 800, ( 3 * delayCount ));
      delayCount = 0;
      break;

    case STATE_ARMED: 
      onOffSound(1000, 0, 1);
      break;
    
    case STATE_TRIGGERED:
      triggerServo.write(30);
      break;

    case STATE_UNARMED: 
      triggerServo.write(180); 
      onOffSound(100, 100, 3);
      break;
  }


}

void evaluateClicks() {
  if(currentState == STATE_UNARMED) {
    newState = STATE_ARMING;
  }
}

void evaluateLongPress() {
  newState = STATE_UNARMED;
}

void trigger() {

  if(currentState == STATE_ARMED) {
    newState = STATE_TRIGGERED;
  }
  
}

void updateBlinking(int pin) {
  if(currentlyOn || repsRemaining > 0) {
    if(currentlyOn) {
      if(millis() > lastBlink + onDuration) {
        digitalWrite(pin, LOW);
        lastBlink = millis();
        currentlyOn = false;
        repsRemaining -= 1;
      } 
    } else {
      if(millis() > lastBlink + offDuration) {
        digitalWrite(pin, HIGH);
        lastBlink = millis();
        currentlyOn = true;
      } 
    }
  }
}

void onOffSound(int on, int off, int reps) {
  if(lastBlinkFinished()) {
    repsRemaining = reps;
    onDuration = on;
    offDuration = off;
  }
}

bool lastBlinkFinished() {
  return millis() >= lastBlink + offDuration && repsRemaining == 0;
}

void killBlinking() {
  digitalWrite(pin, LOW);
  repsRemaining = 0;
  currentlyOn = false;
  lastBlink = 0;
}
