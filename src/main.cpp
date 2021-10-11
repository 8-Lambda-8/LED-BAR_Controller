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
#define LED_PIN5 6
#define LED_PIN6 7
#define LED_PIN7 8
#define LED_PIN8 9

#define LED_PIN1  2
#define LED_PIN2  3

#define COLOR_ORDER GRB
#define CHIPSET     WS2812

#define BRIGHTNESS 255

#define stripLength 60

uint16_t address = 0;

#define NUM_LEDS stripLength
#define NUM_BARS 8

CRGB leds[NUM_BARS][NUM_LEDS];

void updateAddress(){    

  address = 0;
   
  for (uint8_t i = 0; i < 5; i++){
    //17+
    uint16_t val = analogRead(17 + i);

    uint8_t x = 0;

    if (val<290){
        //Serial.println("1 1");
        x = 3;
    }else if (val < 400){
        //Serial.println("0 1");
        x = 1;
    }else if (val < 600){
        //Serial.println("1 0");
        x = 2;
    }else{
        //Serial.println("0 0");
    }
    address += x << i*2;
  } 
    //Serial.println(address);
}


//*/
void setup() {

  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  
  ledOff();
  delay(100);
  ledRed();
  delay(100);
  ledGreen();
  delay(100);
  ledBlue();
  delay(100);
  ledOff();
  delay(500);

  singleRed();
  delay(2000);
  singleYellow();

  //Serial.begin(9600);

  DMXSerial.init(DMXReceiver);
  DMXSerial.maxChannel(400);

  FastLED.addLeds<CHIPSET, LED_PIN1, COLOR_ORDER>(leds1, NUM_LEDS);
  FastLED.addLeds<CHIPSET, LED_PIN2, COLOR_ORDER>(leds2, NUM_LEDS);

  FastLED.addLeds<CHIPSET, LED_PIN1, COLOR_ORDER>(leds[0], NUM_LEDS);
  FastLED.addLeds<CHIPSET, LED_PIN2, COLOR_ORDER>(leds[1], NUM_LEDS);
  FastLED.addLeds<CHIPSET, LED_PIN3, COLOR_ORDER>(leds[2], NUM_LEDS);
  FastLED.addLeds<CHIPSET, LED_PIN4, COLOR_ORDER>(leds[3], NUM_LEDS);
  FastLED.addLeds<CHIPSET, LED_PIN5, COLOR_ORDER>(leds[4], NUM_LEDS);
  FastLED.addLeds<CHIPSET, LED_PIN6, COLOR_ORDER>(leds[5], NUM_LEDS);
  FastLED.addLeds<CHIPSET, LED_PIN7, COLOR_ORDER>(leds[6], NUM_LEDS);
  FastLED.addLeds<CHIPSET, LED_PIN8, COLOR_ORDER>(leds[7], NUM_LEDS); //*/

  FastLED.setBrightness( BRIGHTNESS );
  
  leds[0][0] = CRGB::Blue;
  FastLED.show();
  delay(500);

  leds[0][0] = CRGB::Green;
  FastLED.show();
  delay(500);

  for(uint8_t i=0;i<NUM_BARS;i++){
    for(uint8_t j=0;j<NUM_BARS;j++){
      leds[j][i] = CRGB(0,255,0);
    }
    FastLED.show();
  }
  for(uint8_t i=0;i<NUM_BARS;i++){
    for(uint8_t j=0;j<NUM_BARS;j++){
      leds[j][i] = CRGB(0,0,0);
    }
    FastLED.show();
  }

  updateAddress();  

  ledGreen();
  delay(200);
  ledOff();

  leds[0][address-1] = CRGB(0,255,0);
  FastLED.show();
  delay(2000);

}

void loop() {

  uint16_t lastPacket = DMXSerial.noDataSince();
  
  //setBlack
  for(uint8_t i=0;i<stripLength;i++){
    for(uint8_t j=0;j<NUM_BARS;j++){
      leds[j][i] = CRGB(0,0,0);
    }
  }

  if (lastPacket < 5000) {
    ledGreen();
    
    for (uint8_t x = 0; x < NUM_BARS; x++){
    
      uint8_t r, g, b;
      //read RGB
      r = DMXSerial.read(0+address+5*x);
      g = DMXSerial.read(1+address+5*x);
      b = DMXSerial.read(2+address+5*x);//*/
      
      uint8_t P = DMXSerial.read(3+address+5*x); //Position
      uint8_t V = DMXSerial.read(4+address+5*x); //Value

      P = map(P,0,255,0,stripLength);
      V = map(V,0,255,0,stripLength);    

      uint8_t minPixel = constrain((P-V), 0, stripLength);
      uint8_t maxPixel = constrain((P+V), 0, stripLength);

      for(uint8_t i = minPixel; i < maxPixel; i++){        
        leds[x][i] = CRGB(r,g,b);      
      }
    }
  }else{
  
    //DMX signal Lost indikator
    for(uint8_t j=0;j<NUM_BARS;j++){
      leds[j][stripLength-1] = CRGB(0,0,255);
    }
    ledRed();

  }

  FastLED.show();

}