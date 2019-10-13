#include "InternetButton/InternetButton.h"

// ------------
// Blink an LED
// ------------

/*-------------

We've heavily commented this code for you. If you're a pro, feel free to ignore it.

Comments start with two slashes or are blocked off by a slash and a star.
You can read them, but your device can't.
It's like a secret message just for you.

Every program based on Wiring (programming language used by Arduino, and Particle devices) has two essential parts:
setup - runs once at the beginning of your program
loop - runs continuously over and over

You'll see how we use these in a second. 

This program will blink an led on and off every second.
It blinks the D7 LED on your Particle device. If you have an LED wired to D0, it will blink that LED as well.

-------------*/

// First, we're going to make some variables.
// This is our "shorthand" that we'll use throughout the program:

// int led1 = D0; // Instead of writing D0 over and over again, we'll write led1
// You'll need to wire an LED to this one to see it blink.

int status = 0;
int lastPressedDay = 0;

uint8_t currentLed = 0;
uint8_t currentLedIntensity = 0;
bool rampUp = true;

InternetButton btn = InternetButton();

enum Mode
{
  OFF,
  READY,
  DONE
};
Mode mode = OFF;

bool isAlmostNoon()
{
  int hour = Time.hour();
  return hour > 10 && hour < 13;
}

// Having declared these variables, let's move on to the setup function.
// The setup function is a standard part of any microcontroller program.
// It runs only once when the device boots up or is reset.

void setup()
{
  btn.begin();

  waitFor(Time.isValid, 60000);
  Time.zone(-4);

  RGB.control(true);
  RGB.color(0, 0, 0);

  btn.allLedsOff();

  btn.playSong("C4,8,E4,8,G4,8,C5,8,G5,4");
}

// Main program loop
void loop()
{
  // Take care of time-based state transitions
  bool almostNoon = isAlmostNoon();
  if (!almostNoon && mode != OFF)
  {
    mode = OFF;
    btn.allLedsOff();
  }
  else if (almostNoon && mode == OFF)
  {
    mode = READY;
  }

  if (mode != READY)
  {
    delay(1000);
    return;
  }

  if (mode == READY)
  {
    if (rampUp && currentLedIntensity == 255)
    {
      rampUp = false;
    }
    else if (!rampUp && currentLedIntensity == 0)
    {
      rampUp = true;
      currentLed++;
    }
    if (currentLed == 12)
    {
      currentLed = 1;
    }
    currentLedIntensity = rampUp ? currentLedIntensity + 1 : currentLedIntensity - 1;
    btn.ledOn(currentLed, 0, currentLedIntensity, 0);
    delay(2);
  }

  int today = Time.day();

  // Read button and react if pressed
  int buttonState = btn.allButtonsOn();
  if (buttonState && lastPressedDay != today)
  {
    if (lastPressedDay != today)
    {

      lastPressedDay = today;
      Particle.publish("saladbutton", PUBLIC);
      mode = DONE;
      btn.allLedsOn(0, 64, 0);
      btn.playSong("C4,8,E4,8,G4,8,C5,8,G5,4");
    }
  }
}
