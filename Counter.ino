#include <AceButton.h>
#include <AdjustableButtonConfig.h>
#include <ButtonConfig.h>
#include <SevenSeg.h>

using namespace ace_button;
const int BUTTON_PIN = 19;
AceButton button(BUTTON_PIN);
int n = 0;
unsigned long interval = 1000;
unsigned long previousMillis  = 0;
const uint8_t STOPWATCH_INIT = 0;
const uint8_t STOPWATCH_STARTED = 1;
const uint8_t STOPWATCH_STOPPED = 2;
uint8_t stopwatchState = STOPWATCH_INIT;
SevenSeg disp(2, 3, 4, 5, 6, 7, 8);
const int numOfDigits = 2;
int digitPins[numOfDigits] = {10, 9};

void handleEvent(AceButton*, uint8_t, uint8_t);
void counter(int x);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT);
  ButtonConfig* buttonConfig = button.getButtonConfig();
  buttonConfig->setEventHandler(handleEvent);
  buttonConfig->setFeature(ButtonConfig::kFeatureClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);
  disp.setDigitPins(numOfDigits, digitPins);
  disp.setTimer(2);
  disp.startTimer();
  disp.write("--");
  Serial.println(F("Stopwatch Ready"));
}

void loop() {
  button.check();
  if (stopwatchState == STOPWATCH_STARTED) {
    unsigned long currentMillis = millis();
    if ((unsigned long)(currentMillis - previousMillis) >= interval) {
      if (n < 100) {
        n++;
        disp.write(n);
        Serial.println(n);
        previousMillis = millis();
      } else {
        for (int i = 0; i < 4; i++) {
          disp.write("FF");
          delay(250);
          disp.write("");
          delay(250);
        }
        stopwatchState = STOPWATCH_INIT;
        n = 0;
        disp.write("--");
      }
    }
  }
}

void handleEvent(AceButton* /* button */, uint8_t eventType, uint8_t /* buttonState */) {

  switch (eventType) {
    case AceButton::kEventClicked:
      if (stopwatchState == STOPWATCH_INIT || stopwatchState == STOPWATCH_STOPPED) {
        if (n == 0) {
          Serial.println("Counter Start");
        } else {
          Serial.println("Counter Resume");
        }

        stopwatchState = STOPWATCH_STARTED;
      } else if (stopwatchState == STOPWATCH_STARTED) {
        stopwatchState = STOPWATCH_STOPPED;
        Serial.println("Counter Stop");
      }
      break;
    case AceButton::kEventLongPressed:
      Serial.println("Counter Reset");
      stopwatchState = STOPWATCH_INIT;
      n = 0;
      disp.write("--");
      break;
  }
}

ISR(TIMER2_COMPA_vect) {
  disp.interruptAction();
}
