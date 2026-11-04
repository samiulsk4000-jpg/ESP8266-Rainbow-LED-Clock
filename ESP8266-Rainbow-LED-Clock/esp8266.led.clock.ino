#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#define NUM_LEDS 60
#define DATA_PIN1 D5
#define DATA_PIN2 D6
#define RED_LED_PIN D7
CRGB leds1[NUM_LEDS];
CRGB leds2[NUM_LEDS];
const char* ssid = "samiul";
const char* password = "SAMI00.@.";
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "[pool.ntp.org](http://pool.ntp.org/)", 19800, 60000);
// ==================== SETTINGS ====================
enum DisplayMode { CLOCK_MODE, RAINBOW_MODE, STATIC_COLOR_MODE };
DisplayMode currentMode = CLOCK_MODE;
int backgroundBrightness = 80;
// রেইনবো হাতের জন্য আলাদা হিউ
static uint8_t hourHue = 0;
static uint8_t minuteHue = 85;
static uint8_t secondHue = 170;
// Timing
unsigned long lastLEDUpdate = 0;
unsigned long lastTimeUpdate = 0;
const int LED_INTERVAL = 30;
const int TIME_UPDATE_INTERVAL = 1000;
// ==================== SETUP ====================
void setup() {
    Serial.begin(115200);
   
    pinMode(RED_LED_PIN, OUTPUT);
    digitalWrite(RED_LED_PIN, LOW);
   
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Connected!");
   
    timeClient.begin();
 
    FastLED.addLeds<WS2812, DATA_PIN1, GRB>(leds1, NUM_LEDS);
    FastLED.addLeds<WS2812, DATA_PIN2, GRB>(leds2, NUM_LEDS);
 
    Serial.println("LED Clock Started - Rainbow Hands");
}
// ==================== MAIN LOOP ====================
void loop() {
    unsigned long now = millis();
    if (now - lastTimeUpdate >= TIME_UPDATE_INTERVAL) {
        timeClient.update();
        lastTimeUpdate = now;
    }
    if (now - lastLEDUpdate >= LED_INTERVAL) {
        updateLEDArrays();
        FastLED.setBrightness(255);
        FastLED.show();
        lastLEDUpdate = now;
    }
   
    updateRedLEDs();
}
// ==================== LED UPDATE ====================
void updateLEDArrays() {
    if (currentMode == CLOCK_MODE) {
        displaySimpleClock();
    }
}
void displaySimpleClock() {
    static uint8_t bgHue = 0;
    static int lastMinute = -1;
    // ==================== রেইনবো ব্যাকগ্রাউন্ড ====================
    for (int i = 0; i < NUM_LEDS; i++) {
        leds1[i] = CHSV(bgHue + (i * 8), 220, backgroundBrightness);
        leds2[i] = leds1[i];
    }
    bgHue += 3;
    int hour = timeClient.getHours();
    int minute = timeClient.getMinutes();
    int second = timeClient.getSeconds();
    if (minute != lastMinute) {
        lastMinute = minute;
    }
    int hourLED   = (hour % 12) * 5 + (minute / 12);
    int minuteLED = map(minute, 0, 59, 0, NUM_LEDS - 1);
    int secondLED = map(second, 0, 59, 0, NUM_LEDS - 1);
    // ==================== সোলো রেইনবো হাত ====================
    // Hour - লাল-কমলা দিকে
    leds1[hourLED] = CHSV(hourHue, 255, 255);
    leds2[hourLED] = leds1[hourLED];
    
    // Minute - নীল-সবুজ দিকে
    leds1[minuteLED] = CHSV(minuteHue, 255, 255);
    leds2[minuteLED] = leds1[minuteLED];
    
    // Second - বেগুনি-নীল দিকে
    leds1[secondLED] = CHSV(secondHue, 255, 255);
    leds2[secondLED] = leds1[secondLED];
    // হাতগুলো আরও উজ্জ্বল
    leds1[hourLED].maximizeBrightness();
    leds1[minuteLED].maximizeBrightness();
    leds1[secondLED].maximizeBrightness();
    leds2[hourLED].maximizeBrightness();
    leds2[minuteLED].maximizeBrightness();
    leds2[secondLED].maximizeBrightness();
    // হিউ পরিবর্তন (রেইনবো ঘুরবে)
    hourHue += 2;
    minuteHue += 3;
    secondHue += 4;
}
void updateRedLEDs() {
    if (currentMode == CLOCK_MODE) {
        digitalWrite(RED_LED_PIN, (millis() / 1000) % 2);
    } else {
        digitalWrite(RED_LED_PIN, HIGH);
    }
} 
