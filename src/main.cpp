#include <Arduino.h>
#include <DMXSerial.h>
#include <FastLED.h>

#define COMMON_ANODE

#define LED_R 11
#define LED_G 12
#define LED_B 13

#ifdef COMMON_ANODE

void ledRed() {
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, HIGH);
  digitalWrite(LED_B, HIGH);
}

void ledGreen() {
  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, HIGH);
}

void ledBlue() {
  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, HIGH);
  digitalWrite(LED_B, LOW);
}

void ledYellow() {
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, HIGH);
}

void ledOff() {
  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, HIGH);
  digitalWrite(LED_B, HIGH);
}

#else

void ledRed() {
  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, LOW);
}

void ledGreen() {
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, HIGH);
  digitalWrite(LED_B, LOW);
}

void ledBlue() {
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, HIGH);
}

void ledYellow() {
  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, HIGH);
  digitalWrite(LED_B, LOW);
}

void ledOff() {
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, LOW);
}

#endif

#define LED_PIN1 2
#define LED_PIN2 3
#define LED_PIN3 4
#define LED_PIN4 5

#define COLOR_ORDER GRB
#define CHIPSET WS2812

#define BRIGHTNESS 255  // 0

static uint16_t address = 0;

#define NUM_LEDS 60
#define NUM_BARS 4
#define CHANNEL_COUNT 6

CRGB leds[NUM_BARS][NUM_LEDS];

void updateAddress() {
  address = 0;

  for (uint8_t i = 0; i < 5; i++) {
    // 17+
    uint16_t val = analogRead(17 + i);

    uint8_t x = 0;

    if (val < 290) {
      // Serial.println("1 1");
      x = 3;
    } else if (val < 400) {
      // Serial.println("0 1");
      x = 1;
    } else if (val < 600) {
      // Serial.println("1 0");
      x = 2;
    } else {
      // Serial.println("0 0");
    }
    address += x << i * 2;
  }
  if (address > (513 - 5 * NUM_BARS)) address = 513 - 5 * NUM_BARS;
  // Serial.println(address);
}

void setup() {
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  ledOff();
  delay(100);
  ledBlue();

  DMXSerial.init(DMXReceiver, 10);
  DMXSerial.maxChannel(DMXSERIAL_MAX);

  FastLED.addLeds<CHIPSET, LED_PIN1, COLOR_ORDER>(leds[0], NUM_LEDS);
  FastLED.addLeds<CHIPSET, LED_PIN2, COLOR_ORDER>(leds[1], NUM_LEDS);
  FastLED.addLeds<CHIPSET, LED_PIN3, COLOR_ORDER>(leds[2], NUM_LEDS);
  FastLED.addLeds<CHIPSET, LED_PIN4, COLOR_ORDER>(leds[3], NUM_LEDS);

  FastLED.setBrightness(BRIGHTNESS);

  leds[0][0] = CRGB::Blue;
  leds[1][2] = CRGB::Blue;
  leds[2][4] = CRGB::Blue;
  leds[3][6] = CRGB::Blue;
  FastLED.show();
  delay(500);

  leds[0][0] = CRGB::Green;
  leds[1][2] = CRGB::Green;
  leds[2][4] = CRGB::Green;
  leds[3][6] = CRGB::Green;
  FastLED.show();
  delay(500);

  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    for (uint8_t j = 0; j < NUM_BARS; j++) {
      leds[j][i] = CRGB::Green;
    }
    FastLED.show();
  }
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    for (uint8_t j = 0; j < NUM_BARS; j++) {
      leds[j][i] = CRGB::Black;
    }
    FastLED.show();
  }

  updateAddress();

  ledOff();

  leds[0][address - 1] = CRGB::Green;
  FastLED.show();
}

uint32_t updateDelayMillis = 0;

void loop() {
  uint16_t lastPacket = DMXSerial.noDataSince();

  // setBlack
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    for (uint8_t j = 0; j < NUM_BARS; j++) {
      leds[j][i] = CRGB::Black;
    }
  }

  if (lastPacket < 5000) {
    ledGreen();

    for (uint8_t x = 0; x < NUM_BARS; x++) {
      CRGB col;
      // read RGB
      uint8_t masterDimer = DMXSerial.read(5 + address + CHANNEL_COUNT * x);

      col.r = DMXSerial.read(0 + address + CHANNEL_COUNT * x) * masterDimer / 255;
      col.g = DMXSerial.read(1 + address + CHANNEL_COUNT * x) * masterDimer / 255;
      col.b = DMXSerial.read(2 + address + CHANNEL_COUNT * x) * masterDimer / 255;
      uint8_t P = DMXSerial.read(3 + address + CHANNEL_COUNT * x);  // Position
      uint8_t V = DMXSerial.read(4 + address + CHANNEL_COUNT * x);  // Value

      P = map(P, 0, 255, 0, NUM_LEDS);
      V = map(V, 0, 255, 0, NUM_LEDS);

      uint8_t minPixel = constrain((P - V), 0, NUM_LEDS);
      uint8_t maxPixel = constrain((P + V), 0, NUM_LEDS);

      for (uint8_t i = minPixel; i < maxPixel; i++) {
        leds[x][i] = col;
      }
    }
  } else {
    // DMX signal Lost indikator
    for (uint8_t j = 0; j < NUM_BARS; j++) {
      leds[j][NUM_LEDS - 1] = CRGB::Blue;
    }
    ledRed();
  }

  if ((millis() - updateDelayMillis) > 40) {
    FastLED.show();
    updateDelayMillis = millis();
  }
}