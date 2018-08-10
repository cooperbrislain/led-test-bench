#include "FastLED.h"

#define DEBUG 0

#define IS_WS2812 1
#define IS_APA102 0

#define PRE_LEDS 0
#define POST_LEDS 0
#define NUM_LEDS 10
#define CLOCK_PIN 6
#define DATA_PIN 7
#define FRAME_DELAY 100
#define PI 3.1415

#define NUM_SPECS 7
#define NUM_MODES 5

#define CHASE_WHITE 0
#define FILL_WHITE 1
#define CHASE_COLOR 2
#define FILL_COLOR 3
#define TEST_STRIP 4

int num_leds;

CRGB leds[NUM_LEDS];

unsigned int mode = 0;
unsigned int index = 0;
unsigned int spec_index = 0;
unsigned int count = 0;
unsigned int hue = 180;
unsigned int brightness = 25;
unsigned int specs[NUM_SPECS];

void fade(){
    for(int i=0; i<NUM_LEDS; i++){
         leds[i].fadeToBlackBy(10);
    }
}

void setup() {
    count=0;
    index = 0;
    brightness = 35;
    mode = 0;

    specs[4] = 27;
    specs[0] = 42;
    specs[1] = 74;
    specs[2] = 113;
    specs[3] = 143;
    specs[5] = 250;
    specs[6] = 500;

    spec_index = 5;
    num_leds = specs[spec_index];

    if(IS_APA102) {
        FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR, DATA_RATE_MHZ(12)>(leds, NUM_LEDS);
    }
    if(IS_WS2812) {
        FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
    }

    initialize();
}

void chase_specs(CRGB color) {
    CRGB color2 = CHSV((hue+80)%255, 355, brightness );
    fade();
    leds[count%num_leds] = color;
    for (int i = 0; i<NUM_SPECS; i++) {
        leds[specs[i]-1] = color2;
    }
}

void chase(CRGB color) {
    fade();
    leds[count%num_leds] = color;
}

void fill(CRGB color) {
    for(int i=0; i<num_leds; i++) {
        leds[i] = color;
    }
}

void test_strip() {
    for(int i=0;i<num_leds;i++) {
        leds[i] = CHSV(((i+count)/4)%2==0? 40 : 160, 255, brightness);
    }
    FastLED.show();
    count++;
    delay(40);
}

void randpulse() {
    fade();
    if (count%10 == 0) {
        index = random(NUM_LEDS);
        hue = random(255);
    }
    CRGB color = CHSV(hue, 255, 100);
    leds[index%NUM_LEDS] += color;
    index++;
}

void initialize() {
    for (int i=0; i<NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
    }
    FastLED.show();
    count = 0;
}

void blink_end() {
    if (num_leds < NUM_LEDS) {
        if (count%25 == 0) {
            if (leds[num_leds] != CRGB(0,0,0)) {
                leds[num_leds] = CRGB::Black;
            } else {
                leds[num_leds] = CRGB::White;
            }
        }
    }
}

void loop() {
    randpulse();
    FastLED.show();
    count++;
    delay(FRAME_DELAY);
}


