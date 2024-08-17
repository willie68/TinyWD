#include <Arduino.h>
#include <SimpleTimer.h>

#define digispark

#ifdef digispark 
// pin definitions, as we use normal pin out pin number = PB#
const int wdPin0 = 0; 
const int wdPin1 = 4; 
const int wdPin2 = 3; 
const int resetPin = 2; 
const int ledPin = 1; 
#elif __AVR_ATtiny85__ 
const int wdPin0 = 0; 
const int wdPin1 = 1; 
const int wdPin2 = 2; 
const int resetPin = 3; 
const int ledPin = 4; 
#endif

// Create timer instances
int wdTimerID; // the timer id from the lib
SimpleTimer timer; // timer instance

bool changed = false; // a input pn has been changed
// this is the interrupt service routine, 
// any change on the input pins will be regonised as change
ISR (PCINT0_vect){
    changed = true;
}

// resetting the host mcu
void resetHost() {
  timer.restartTimer(wdTimerID);
  for (byte i = 5; i > 0; i--) {
    digitalWrite(ledPin, 1);
    delay(100);
    digitalWrite(ledPin, 0);
    delay(100);
  }
  digitalWrite(resetPin, 0); 
  delay(1000); 
  digitalWrite(resetPin, 1);
  timer.restartTimer(wdTimerID);
}


void setup() {
  // Setting wd reset and led pin to output mode
  pinMode(resetPin, OUTPUT);
  digitalWrite(resetPin, 1);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, 1);
  delay(500);
  digitalWrite(ledPin, 0);

  // setting all watch pins to input mode with a pullup
  pinMode(wdPin0, INPUT_PULLUP);
  pinMode(wdPin1, INPUT_PULLUP);
  pinMode(wdPin2, INPUT_PULLUP);

  // setting up the pin change interrupt
  cli();
  GIMSK |= (1<<PCIE); // General Interrupt Mask Register
  PCMSK = (1<<wdPin0) | (1<<wdPin1) | (1<<wdPin2); // monitor all input pins 
  sei();

  // setting the timer intervall, callback and starting the timer
  wdTimerID = timer.setInterval(10000, resetHost);
}

// showing some heardbeat on the led
unsigned long saved = 0;
void heartbeat() {
  if (millis() > saved) {
    saved = millis() + 5000;
    digitalWrite(ledPin, 1);
    delay(50);
    digitalWrite(ledPin, 0);
  }
}

// checking if a pin has been changed
void pinChange() {
  if (changed) { // pin change interrupt occured
    changed = false;
    timer.restartTimer(wdTimerID);
    digitalWrite(ledPin, 1);
    delay(100);
    digitalWrite(ledPin, 0);
  }
}


void loop() {
  timer.run();

  heartbeat();

  pinChange();

  delay(10);
}
