#define MQTT_DEBUG
#include "SPI.h"
#include <Ethernet.h>
#include <Dns.h>
#include <Dhcp.h>
#include <PubSubClient.h>
#include "FastLED.h"
#include <LiquidCrystal.h>

#define IS_WS2812 0
#define IS_APA102 1

#define PRE_LEDS 0
#define POST_LEDS 1
#define NUM_LEDS 200
#define DATA_PIN 30
#define CLOCK_PIN 31
#define FRAME_DELAY 100
#define BUTTON_PIN 4
#define PI 3.1415
#define BIG_BTN_COM 40
#define BIG_BTN_NO 41
#define BIG_BTN_LED 42
//#define BRIGHTNESS_PIN 0
//#define SPEED_PIN 1
#define MIN_BRIGHTNESS 32
#define MAX_BRIGHTNESS 255
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

int clock_ticking;
int num_leds;

int lcd_key     = 0;
int adc_key_in  = 0;

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

CRGB leds[NUM_LEDS];

int index;
int spec_index;
int count;
int color;
int brightness;
int speed;
bool btn_still_down;
int specs[NUM_SPECS];

int num_leds;

int read_lcd_buttons() {
    adc_key_in = analogRead(0);      // read the value from the sensor
    // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
    // we add approx 50 to those values and check to see if we are close
    if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
    if (adc_key_in < 50)   return btnRIGHT;
    if (adc_key_in < 250)  return btnUP;
    if (adc_key_in < 380)  return btnDOWN;
    if (adc_key_in < 420)  return btnLEFT;
    if (adc_key_in < 700)  return btnSELECT;
    return btnNONE;  // when all others fail, return this...
}

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
    lcd.begin(16, 2);              // start the library
    lcd.setCursor(0,0);
    lcd.print("LED Test Bench"); // print a simple message
    specs[0] = 74;
    specs[1] = 113;
    specs[2] = 143;
    //Serial.begin(9600);
    if(IS_APA102) {
        FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR, DATA_RATE_MHZ(12)>(leds, NUM_LEDS);
    }
    if(IS_WS2812) {
        FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
    }
    for (int i=0; i<NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
    }
    FastLED.show();
    count=0;
    spec_index = 0;
    num_leds = specs[spec_index];
    index = 0;
    speed = 0;
}

void chase_white() {
    for(int i=0; i<NUM_LEDS;i++){
        if((i+count)%10==0) {
            leds[i] += CHSV(color+(((count+count/25)/10)%2*128)%255, 255, 85);
        }
    }
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

void light_tests() {
    for(int i=0; i<TEST_LEDS-1; i++) {
        leds[i] = CRGB::Red;
        leds[TEST_LEDS+num_leds+TEST_LEDS-(i+1)] = CRGB::Blue;
        FastLED.show();
        delay(1000);
        check_buttons();
    }
    fill(CRGB::Green);
    FastLED.show();
}

void test_strip() {
    // assumes 5 LED test strip before, and after
    // num_leds is the number of LEDs in the strip to be tested
    for (int i=0; i<num_leds; i++) {
        CRGB color = CRGB::White;
        int center = num_leds/2;
        if (abs(i-center)%(i/3)==0) {
            color = CRGB::Black;
        }
        leds[TEST_LEDS+i] = color;
        leds[TEST_LEDS+num_leds-(i+1)] = color;
        FastLED.show();
        fadeby(25);
        delay(3);
        check_buttons();
    }
    // as long as the first LED is still lit, keep fading out
    // (others shoudl not be lit by the time this one fades out)
    do {
        fadeby(25);
        FastLED.show();
        delay(3);
        check_buttons();
    } while (leds[TEST_LEDS]);
    delay(FRAME_DELAY);
}

void initialize() {
    fill(CRGB::Black);
    FastLED.show();
    count = 0;
    light_tests();
}

void print_status() {
    char buffer[16];
    lcd.setCursor(0,0);
    lcd.print("LED Test Bench");
    lcd.setCursor(0,1);
    sprintf(buffer, "Spec %d - %d px", spec_index, num_leds);
    lcd.print(buffer);
}

void check_buttons() {
    int btn = read_lcd_buttons();
    if(btn == btnNONE) {
        btn_still_down = false;
    } else {
        if (!btn_still_down) {
            btn_still_down = true;
            switch( btn ) {
                case btnUP :
                    if(spec_index > 0) {
                        spec_index--;
                        num_leds = specs[spec_index];
                    }
                    break;
                case btnDOWN :
                    spec_index = (spec_index + 1)%NUM_SPECS;
                    num_leds = specs[spec_index];
                    break;
            }
            print_status();
        }
    }
}

void check_big_button() {
    int val = digitalRead(BIG_BTN_NO);
    if (val) {
        if (clock_ticking) {
            clock_ticking = 0;
            digitalWrite(BIG_BTN_LED, LOW);
        } else {
            clock_ticking = 1;
            digitalWrite(BIG_BTN_LED, HIGH);
        }
    }
}

void loop() {
    print_status();
    initialize();
    test_strip();
    check_big_button();
    check_buttons();
    delay(FRAME_DELAY);
    count++;
}