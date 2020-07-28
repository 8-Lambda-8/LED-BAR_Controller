#include <Arduino.h>
#include <unity.h>

String STR_TO_TEST;


const uint8_t stripLength = 60;


void setUp(void) {
    // set stuff up here
    STR_TO_TEST = "Hello, world!";
}

void tearDown(void) {
    // clean stuff up here
    STR_TO_TEST = "";
}

uint8_t maxVal(uint8_t pos){
  if(pos<30){
    return map(pos, 0,29,60,30);
  }else{
    return map(pos, 30,60,30,60);
  }
}

uint8_t j = 0;


void test_maxVal(){    

    //uint8_t x = maxVal(j);
    

    uint8_t P1 = j;//map(x,0,255,0,60);
    
    uint8_t V1 = 3;//map(V1,0,255,0,maxVal(P1));
    
    

    int minPixel = constrain((P1-V1), 0, stripLength);
    int maxPixel = constrain((P1+V1), 0, stripLength);


    char buf [16];
    sprintf (buf, "%03i %03i %03i", minPixel, maxPixel, maxPixel-minPixel);


    TEST_IGNORE_MESSAGE(buf);

    TEST_ASSERT_EQUAL_INT(V1*2,maxPixel-minPixel);

}

void test_test(){

    TEST_ASSERT_EQUAL(60, 60);
    
}


void setup()
{
    delay(2000); // service delay
    UNITY_BEGIN();
    
    for(uint8_t i = 0;i<=stripLength;i++){
        j = i;
        RUN_TEST(test_maxVal);
    }
    
    
    UNITY_END(); // stop unit testing
}

void loop()
{
}
