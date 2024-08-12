#include <Arduino.h>
#include <SimpleTimer.h>

const int wdPin = 0; // PB0, PB1, PB2
const int resetPin = 3; //PB3
const int ledPin = 4; //PB4

int wd_timer_id;
int currState = HIGH;
int prevState = LOW;

// Create timer instances
SimpleTimer timer;

void wdCallback();

void setup() {
  pinMode(resetPin, OUTPUT);
  digitalWrite(resetPin, HIGH);
  wd_timer_id = timer.setInterval(30000, wdCallback);
}

void ledHeartbeat(void) {
  digitalWrite(ledPin, currState = digitalRead(wdPin));
  if ( currState != prevState) { // State has changed
    prevState = currState; // Update
    timer.restartTimer(wd_timer_id);
  }
}

void resetHost(int resetPin) {
  digitalWrite(resetPin, LOW); 
  delay(5000); 
  digitalWrite(resetPin, HIGH);
}

void wdCallback() {
  resetHost(resetPin); 
}

void loop() {
  timer.run();
  ledHeartbeat();
}

