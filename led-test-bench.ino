#include "FastLED.h"

#define IS_WS2812 0
#define IS_APA102 1

#define NUM_LEDS 153 // 5+153+5
#define TEST_LEDS 5
#define DATA_PIN 7
#define CLOCK_PIN 6
#define SPEED_PIN A0
#define FRAME_DELAY 100
#define BUTTON_PIN 4
#define PI 3.1415
//#define BRIGHTNESS_PIN 0
//#define SPEED_PIN 1
#define MIN_BRIGHTNESS 32
#define MAX_BRIGHTNESS 255

CRGB leds[NUM_LEDS];

int index;
int index2;
int count;
int color;
int brightness;
int speed;
int num_leds;

void fade(){
    for(int i=0; i<NUM_LEDS; i++){
         leds[i].fadeToBlackBy( 10 );
    }
}

void countfade() {
    for (int i=0;i<count;i++) {
        if(i%10!=9) {
            leds[i].fadeToBlackBy(10);
        }
    }
}

void fadeby(int b){
    for(int i=0; i<NUM_LEDS; i++){
         leds[i].fadeToBlackBy( b );
    }
}

void fade_out_by(int b){
    if(brightness > 0) {
        brightness -= b;
        FastLED.setBrightness(brightness);
    }
}

void setup() {
    //Serial.begin(9600);
    if(IS_APA102) {
        FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, RGB, DATA_RATE_MHZ(12)>(leds, NUM_LEDS);
    }
    if(IS_WS2812) {
        FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
    }
    for (int i=0; i<NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
    }
    FastLED.show();
    count=0;
    num_leds = NUM_LEDS;
    //randomSeed(analogRead(A3));
    index = 0;
    index2 = 0;
    speed = 0;
}

void chase_white() {
    for(int i=0; i<NUM_LEDS;i++){
        if((i+count)%10==0) {
            leds[i] += CHSV(color+(((count+count/25)/10)%2*128)%255, 255, 85);
        }
    }
}

int count_leds() {
    color = 80;
    count = 0;
    for (int i=index; i<500; i++) {
        countfade();
        if(digitalRead(BUTTON_PIN) == HIGH) {
            num_leds = i-(i%10);
            break;
        }
        if(i%50==49) {
            leds[i] = CHSV((color+128)%255,255,128);
        }
        else if(i%10==9) {
            leds[i] = CHSV((color+96)%255,255,128);
        }
        else if(i%5==4) {
            leds[i] = CHSV((color+64)%255,255,64);
        } else {
            leds[i] = CHSV(color,255,40);
        }
        color+=3;
        FastLED.show();
        count++;
        delay(50*((i%50==49)+(i%10==9)+(i%5==4)+1)*2);
    }
    return count;
    delay(5000);
}

void sinewave(){
    for(int i=0;i<1000;i++){
        fade();
        leds[(num_leds+(int)(sin(i*PI/180)*num_leds/2+num_leds/2))%num_leds] = CRGB::White;
        FastLED.show();
        delay(10);
    }
}

void randstrobe() {
    for(int i=0; i<10000; i++) {
        if(i%8==0) {
            for(int j=random(NUM_LEDS); j<random(NUM_LEDS)+random(NUM_LEDS/3); j++) {
                leds[j%NUM_LEDS] += CHSV(255,0,180);
            }
        }
        FastLED.show();
        fadeby(50);
        count++;
    }
}

void strobe() {
    for(int i=0; i<10000; i++) {
        if(i%4==0) {
            int which = random(4);
            for(i=which*143; i<which*143+143; i++) {
                leds[i]+= CHSV(0,0,188);
            }
        }
        FastLED.show();
        count++;
    }
}

void fill(CRGB color) {
    for(int i=0; i<NUM_LEDS; i++) {
        leds[i] = color;
    }
}

void light_tests(int num_leds) {
    for(int i=0; i<TEST_LEDS; i++) {
        leds[i] = CRGB::Blue;
        leds[TEST_LEDS+num_leds+TEST_LEDS-(i+1)] = CRGB::Blue;
        FastLED.show();
        delay(1000);
    }
}

void test_strip(int num_leds) {
    // assumes 5 LED test strip before, and after
    // num_leds is the number of LEDs in the strip to be tested
    for(int i=0;i<num_leds;i++) {
        CRGB color = CRGB::White;
        if(i%10 == 0) color = CRGB::Blue;
        if(i%100 == 0) color = CRGB::Red;
        leds[TEST_LEDS+i] = color;
        leds[TEST_LEDS+num_leds-(i+1)] = color;
        FastLED.show();
        fadeby(25);
        delay(3);
    }
}

void initialize(int num_leds) {
    fill(CRGB::White);
    FastLED.show();
    fill(CRGB::Black);
    FastLED.show();
    count = 0;
    light_tests(num_leds);
}

void loop() {
    int num_leds = 143;
    initialize(num_leds);
    test_strip(num_leds);
    delay(FRAME_DELAY);
    count++;
}