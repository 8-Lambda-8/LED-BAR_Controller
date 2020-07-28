#include <Arduino.h>
#include <DMXSerial.h>
#include <FastLED.h>


#define COMMON_ANODE

#define LED_R 11
#define LED_G 12
#define LED_B 13

#ifdef COMMON_ANODE

  void singleRed() {
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_G, HIGH);
    digitalWrite(LED_B, HIGH);
  }

  void singleGreen() {
    digitalWrite(LED_R, HIGH);
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_B, HIGH);
  }

  void singleBlue() {
    digitalWrite(LED_R, HIGH);
    digitalWrite(LED_G, HIGH);
    digitalWrite(LED_B, LOW);
  }

  void singleYellow() {
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_B, HIGH);
  }

  void allBlack() {
    digitalWrite(LED_R, HIGH);
    digitalWrite(LED_G, HIGH);
    digitalWrite(LED_B, HIGH);
  }

#else

  void singleRed() {
    digitalWrite(LED_R, HIGH);
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_B, LOW);
  }

  void singleGreen() {
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_G, HIGH);
    digitalWrite(LED_B, LOW);
  }

  void singleBlue() {
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_B, HIGH);
  }

  void singleYellow() {
    digitalWrite(LED_R, HIGH);
    digitalWrite(LED_G, HIGH);
    digitalWrite(LED_B, LOW);
  }

  void allBlack() {
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_B, LOW);
  }

#endif


#define LED_PIN1  2
#define LED_PIN2  3

#define COLOR_ORDER GRB
#define CHIPSET     WS2812

#define BRIGHTNESS 255

#define stripLength 60

uint16_t address = 0;

#define NUM_LEDS stripLength

CRGB leds1[NUM_LEDS];
CRGB leds2[NUM_LEDS];

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
  
  allBlack();

  singleRed();
  delay(2000);
  singleYellow();

  //Serial.begin(9600);

  DMXSerial.init(DMXReceiver);
  DMXSerial.maxChannel(400);

  FastLED.addLeds<CHIPSET, LED_PIN1, COLOR_ORDER>(leds1, NUM_LEDS);
  FastLED.addLeds<CHIPSET, LED_PIN2, COLOR_ORDER>(leds2, NUM_LEDS);

  FastLED.setBrightness( BRIGHTNESS );
  
  leds1[0] = CRGB::Blue;
  FastLED.show();
  delay(500);

  leds1[0] = CRGB::Green;
  FastLED.show();
  delay(500);

  for(uint8_t i=0;i<8;i++){
    leds1[i] = CRGB(0,255,0);
    leds2[i] = CRGB(0,255,0);
    FastLED.show();
  }
  for(uint8_t i=0;i<8;i++){
    leds1[i] = CRGB(0,0,0);
    leds2[i] = CRGB(0,0,0);
    FastLED.show();
  }

  updateAddress();  

  singleGreen();
  delay(200);

  //allBlack();  

  leds1[address-1] = CRGB(0,255,0);
  FastLED.show();
  delay(2000);

}

void loop() {

  uint16_t lastPacket = DMXSerial.noDataSince();
  
  //setBlack
  for(uint8_t i=0;i<stripLength;i++){
    leds1[i] = CRGB(0,0,0);
    leds2[i] = CRGB(0,0,0);
  }

  if (lastPacket < 5000) {

    uint8_t r, g, b;
    
    //read RGB
    r = DMXSerial.read(0+address);
    g = DMXSerial.read(1+address);
    b = DMXSerial.read(2+address);//*/
    
    // strip1
    uint8_t P = DMXSerial.read(3+address); 
    uint8_t V = DMXSerial.read(4+address);    

    P = map(P,0,255,0,stripLength);
    V = map(V,0,255,0,stripLength);    

    uint8_t minPixel = constrain((P-V), 0, stripLength);
    uint8_t maxPixel = constrain((P+V), 0, stripLength);

    for(uint8_t i = minPixel; i < maxPixel; i++){        
      leds1[i] = CRGB(r,g,b);      
    }
    
    //Strip2
    P = DMXSerial.read(5+address);
    V = DMXSerial.read(6+address);

    P = map(P,0,255,0,stripLength);
    V = map(V,0,255,0,stripLength);    

    minPixel = constrain((P-V), 0, stripLength);
    maxPixel = constrain((P+V), 0, stripLength);

    for(uint8_t i = minPixel; i < maxPixel; i++){        
      leds2[i] = CRGB(r,g,b);
    }
    
  }else{
  
    //DMX signal Lost indikator
    leds1[stripLength-1] = CRGB(0,0,255);
    singleRed();

  }

  FastLED.show();

  delay(35);
}