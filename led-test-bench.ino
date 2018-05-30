#define MQTT_DEBUG
#include "SPI.h"
#include <Ethernet.h>
#include <Dns.h>
#include <Dhcp.h>
#include <PubSubClient.h>
#include "FastLED.h"
#include <ArduinoJson.h>

#define IS_WS2812 0
#define IS_APA102 1

#define PRE_LEDS 0
#define POST_LEDS 1
#define NUM_LEDS 500
#define DATA_PIN 30
#define CLOCK_PIN 31
#define FRAME_DELAY 100
#define BUTTON_PIN 4
#define PI 3.1415
#define BIG_BTN_COM 38
#define BIG_BTN_NO 39
#define BIG_BTN_LED 40
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
#define NUM_SPECS 5

#define halt(s) { Serial.println(F( s )); while(1);  }

void mqtt_callback(char* topic, byte* payload, unsigned int length);
void reconnect();

int clock_ticking;
int num_leds;

CRGB leds[NUM_LEDS];

int index;
int spec_index;
int count;
int color;
int brightness;
bool btn_still_down;
int specs[NUM_SPECS];

// Ethernet Vars
byte mac[] = { 0xCA, 0x3D, 0xB3, 0x33, 0xFC, 0x1D };

const char* mqtt_server = "mqtt.spaceboycoop.com";
const int mqtt_port = 1883;
const char* mqtt_username = "spaceboycoop";
const char* mqtt_key = "r7rObnC6i2paWPxeEMuWiF";

EthernetClient eth_client;
PubSubClient mqtt_client(eth_client);

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

void progress() {
    count++;
    leds[count] = CRGB::DodgerBlue;
    FastLED.show();
}

void failed() {
    for(int i=0; i<count; i++) {
        leds[i] = CRGB::Red;
    }
    FastLED.show();
}

void finish() {
    for (int i=0; i<NUM_LEDS; i++) {
        leds[i] = CHSV(i, 255, 25);
        FastLED.show();
        delay(5);
    }
}

void setup() {
    Serial.begin(115200);
    count=0;
    index = 0;
    specs[0] = 42;
    specs[1] = 74;
    specs[2] = 113;
    specs[3] = 143;
    specs[4] = 27;

    spec_index = 0;
    num_leds = specs[spec_index];

    StaticJsonBuffer<200> jsonBuffer;

    Serial.println("Starting LED Test Bench");
    if(IS_APA102) {
        FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR, DATA_RATE_MHZ(12)>(leds, NUM_LEDS);
    }
    if(IS_WS2812) {
        FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
    }
    for (int i=0; i<NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
    }
    leds[0] = CRGB::Green;
    FastLED.show();
    progress();
    Serial.println("Initializing Interfaces");
    digitalWrite(BIG_BTN_COM, HIGH);
    digitalWrite(BIG_BTN_LED, LOW);
    progress();
    mqtt_client.setServer(mqtt_server, mqtt_port);
    mqtt_client.setCallback(mqtt_callback);
    progress();
    Ethernet.begin(mac);

    Serial.println(F("Connecting..."));
    progress();
    if(!Ethernet.begin(mac)) {
        Serial.println(F("Ethernet configuration failed."));
        failed();
    } else {
        progress();
        Serial.println(F("Ethernet configured via DHCP"));
        Serial.print("IP address: ");
        Serial.println(Ethernet.localIP());
        Serial.println();
        progress();
    }
    finish();
    delay(1500);
    initialize();
}

void chase_white() {
    for(int i=0; i<num_leds;i++){
        if((i+count)%10==0) {
            leds[i] += CHSV(color+(((count+count/25)/10)%2*128)%255, 255, 85);
        }
    }
}

void fill(CRGB color) {
    for(int i=0; i<NUM_LEDS; i++) {
        leds[i] = color;
    }
}

void test_strip() {
    for (int i=0; i<NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
    }
    leds[0] = CRGB::Red;
    leds[num_leds-1] = CRGB::Red;
    leds[num_leds] = CRGB::Blue;
    for (int i=1; i<num_leds-1; i++) {
        leds[i] = CRGB::Indigo;
    }
    FastLED.show();

    delay(1000);
}

void initialize() {
    fill(CRGB::Black);
    FastLED.show();
    count = 0;
}

/*void print_status() {
    char buffer[16];
    sprintf(buffer, "Spec %d - %d px", spec_index, num_leds);
}*/

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

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    payload[length] = '\0';
    if(strcmp(topic,"leds/test/checkin") == 0) {
        Serial.println("Message received from leds/test/checkin");
        int new_spec = atoi((char *)payload);
        num_leds = specs[new_spec];
        Serial.print("Chaned to spec ");
        Serial.print(new_spec);
        Serial.print('(');
        Serial.print(num_leds);
        Serial.print(')');
    }
}

void reconnect() {
  // Loop until we're reconnected
    while (!mqtt_client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (mqtt_client.connect("arduinoClient",mqtt_username,mqtt_key)) {
            Serial.println("connected");
            // ... and resubscribe
            if(!mqtt_client.subscribe("leds/test/checkin")) {
                Serial.println("Subscribe to leds/test/checkin failed.");
            } else {
                Serial.println("Subscribed to leds/test/checkin.");
            }
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqtt_client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void loop() {
    if (!mqtt_client.connected()) {
        Serial.println("Reconnecting...");
        reconnect();
    }
    mqtt_client.loop();
    //print_status();
    test_strip();
    check_big_button();
    delay(FRAME_DELAY);
    count++;
}