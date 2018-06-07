#include "FastLED.h"
#include "LiquidCrystal.h"

#define DEBUG 0

#define IS_WS2812 0
#define IS_APA102 1

#define PRE_LEDS 0
#define POST_LEDS 1
#define NUM_LEDS 300
#define CLOCK_PIN 31
#define DATA_PIN 30
#define FRAME_DELAY 100
#define PI 3.1415

#define NUM_SPECS 7
#define NUM_MODES 5

#define CHASE_WHITE 0
#define FILL_WHITE 1
#define CHASE_COLOR 2
#define FILL_COLOR 3
#define TEST_STRIP 4

#define halt(s) { Serial.println(F( s )); while(1);  }

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;


int read_LCD_buttons()
{
    adc_key_in = analogRead(0);
    Serial.println(adc_key_in);
    if (adc_key_in > 1000) return btnNONE;
    if (adc_key_in < 50)   return btnRIGHT;
    if (adc_key_in < 250)  return btnUP;
    if (adc_key_in < 380)  return btnDOWN;
    if (adc_key_in < 420)  return btnLEFT;
    if (adc_key_in < 700)  return btnSELECT;
    return btnNONE;
}

int num_leds;

CRGB leds[NUM_LEDS];

unsigned int mode = 0;
unsigned int index = 0;
unsigned int spec_index = 0;
unsigned int count = 0;
unsigned int hue = 0;
unsigned int brightness = 25;
unsigned int pressed = btnNONE;
unsigned int pressed_time = 0;

unsigned int on = 0;

unsigned int specs[NUM_SPECS];

void fade(){
    for(int i=0; i<NUM_LEDS; i++){
         leds[i].fadeToBlackBy( 10 );
    }
}

void setup() {
    Serial.begin(115200);
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

    spec_index = 0;
    num_leds = specs[spec_index];

    lcd.begin(16, 2);
    lcd.setCursor(0,0);
    lcd.print("LED Field Tester");
    Serial.println("Starting LED Test Bench");

    if(IS_APA102) {
        FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR, DATA_RATE_MHZ(12)>(leds, NUM_LEDS);
    }
    if(IS_WS2812) {
        FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
    }

    initialize();
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

void turn_on() {
    on = 1;
    lcd.setCursor(0,1);
    lcd.print("On       ");
}

void turn_off() {
    on = 0;
    lcd.setCursor(0,1);
    lcd.print("Off       ");
}

void test_strip() {
    for(int i=0;i<num_leds;i++) {
        leds[i] = CHSV(((i+count)/4)%2==0? 40 : 160, 255, brightness);
    }
    FastLED.show();
    count++;
    delay(40);
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

String get_button_name(int the_key) {
    switch (the_key) {
        case btnNONE:
            return "None";
        case btnUP:
            return "Up";
        case btnDOWN:
            return "Down";
        case btnLEFT:
            return "Left";
        case btnSELECT:
            return "Select";
        case btnRIGHT:
            return "Right";
        default:
            // this should never happen
            break;
    }
}

void loop() {
    char buffer[4];
    int lcd_key = read_LCD_buttons();
    if (lcd_key != pressed) {
        if (DEBUG && lcd_key != btnNONE) {
            Serial.print(get_button_name(lcd_key));
            Serial.println(" pressed");
        }
        if (lcd_key == btnNONE) {
            if (DEBUG) {
                Serial.print(get_button_name(pressed));
                Serial.print(" released after ");
                Serial.println(pressed_time);
            }
            if (pressed_time <= 3) {
                switch (pressed) {
                    case btnUP:
                        if (!on) {
                            turn_on();
                        }
                        break;
                    case btnDOWN:
                        if (on) {
                            turn_off();
                        }
                        break;
                    default:
                        break;
                }
            }
        }
        pressed = lcd_key;
        pressed_time = 0;
    } else if (lcd_key != btnNONE) {
        pressed_time++;
        if (DEBUG) {
            Serial.print("still pressed for ");
            Serial.println(pressed_time);
        }
        if (pressed_time > 3) {
            switch (pressed) {
                case btnUP:
                    if (!on) {
                        turn_on();
                        brightness = 0;
                    }
                    if (brightness < 255) {
                        brightness++;
                    }
                    lcd.setCursor(0,1);
                    lcd.print("Bright");
                    lcd.print(" - ");
                    lcd.println(brightness);
                    break;
                case btnDOWN:
                    if (hue > 0) {
                        hue--;
                    } else {
                        turn_off();
                    }
                    lcd.setCursor(0,1);
                    lcd.print("Bright");
                    lcd.print(" - ");
                    lcd.println(brightness);
                    break;
                case btnLEFT:
                    spec_index++;
                    if(spec_index == NUM_SPECS) {
                        spec_index = 0;
                    }
                    num_leds = specs[spec_index];
                    lcd.setCursor(0,1);
                    lcd.print("spec ");
                    lcd.print(spec_index);
                    lcd.print(" - ");
                    lcd.print(num_leds);
                    lcd.println("px");
                    break;
            }
        }
    } else if (lcd_key == btnNONE) {
        pressed = btnNONE;
        pressed_time = 0;
    }
    // led animations
    if (on) {
        switch (mode) {
            case CHASE_WHITE:
                chase(CHSV(0,0,brightness));
                break;
            case FILL_WHITE:
                fill(CHSV(0,0,brightness));
                break;
            case CHASE_COLOR:
                chase(CHSV(hue,255,brightness));
                break;
            case FILL_COLOR:
                fill(CHSV(hue,255,brightness));
                break;
            case TEST_STRIP:
                test_strip();
                break;
        }
    } else {
        fill(CRGB::Black);
    }
    FastLED.show();
    count++;
    //delay(FRAME_DELAY);
}


