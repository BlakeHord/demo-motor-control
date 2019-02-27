/*  Demonstration Motor Controls
     Blake Hord
     Last Updated February 26, 2019

     For use in the hybrid motor to be used in Stanford's AA 103 Propulsion Class
     Developed in Professor Brian Cantwell's lab under the guidance of PhD students Flora Mechentel and David Dyrda
*/

#include <FastLED.h>

#define LED_PIN     10
#define NUM_LEDS    82
#define BRIGHTNESS  16
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define UPDATES_PER_SECOND 1000
CRGB leds[NUM_LEDS];

CRGBPalette16 currentPalette;
TBlendType    currentBlending;

int propane_input = 6;
int o2_input = 2;
int arm_input = 9;
int fire_input = 3;
int arm_led = 4;
int fire_led = 5;

int o2_output = A1;
int propane_relay = 11;
int arm_relay = 12;
int fire_relay = 13;

unsigned long count = 0;
bool armed = false;
int currentAction = 0;

// Ignition sequence in milliseconds
int spark_time = 250;

bool debounce(int pin);
void FillLEDsFromPaletteColors(uint8_t colorIndex);
void led_white();
void led_green();
void led_blue();
void led_red();
void led_orange();


void setup() {
  // Controller Pins
  pinMode(o2_input, INPUT);
  pinMode(propane_input, INPUT);
  pinMode(arm_input, INPUT);
  pinMode(fire_input, INPUT);

  pinMode(arm_led, OUTPUT);
  pinMode(fire_led, OUTPUT);

  // LED's for arming and firing
  digitalWrite(arm_led, LOW);
  digitalWrite(fire_led, LOW);

  // Relay Pins
  pinMode(o2_output, OUTPUT);
  pinMode(propane_relay, OUTPUT);
  pinMode(arm_relay, OUTPUT);
  pinMode(fire_relay, OUTPUT);

  // Setting relay pins to HIGH. HIGH == off for relay
  digitalWrite(o2_output, HIGH);
  digitalWrite(propane_relay, HIGH);
  digitalWrite(arm_relay, HIGH);
  digitalWrite(fire_relay, HIGH);

  // Serial with 57600 baud rate
  Serial.begin(57600);

  // Initialize LED strip as white
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );
  led_white();
}


// Main motor control loop
void loop() {
  count += 1;

  // OXYGEN VENT CONTROLLER
  while (debounce(o2_input) == true) {
    Serial.println("oxygen vent on");
    Serial.println(count);
    digitalWrite(o2_output, LOW);
    led_green();
    currentAction = 1;
  }
  digitalWrite(o2_output, HIGH);

  
  // PROPANE VENT CONTROLLER
  while (debounce(propane_input) == true) {
    Serial.println("propane vent on");
    Serial.println(count);
    digitalWrite(propane_relay, LOW);
    led_blue();
    currentAction = 2;
  }
  digitalWrite(propane_relay, HIGH);

 
  // ARM CONTROLLER
  if (debounce(arm_input) == true) {
    while (debounce(arm_input) == true);

    // If armed, make disarmed. If disarmed, make armed
    if (armed == true) {
      digitalWrite(arm_relay, HIGH);
      digitalWrite(arm_led, LOW);
      armed = false;
      Serial.println("Disarmed");
      Serial.println(count);
      party();
      led_white();
      currentAction = 0;
    } else if (armed == false) {
      digitalWrite(arm_relay, LOW);
      digitalWrite(arm_led, HIGH);
      armed = true;
      Serial.println("Armed");
      Serial.println(count);
      led_orange();
      currentAction = 3;
    }
  }

  // Return to white if not armed
  if (currentAction == 0) {
    led_white();
  }


  // FIRING SEQUENCE CONTROLLER
  unsigned long current = millis();
  if (debounce(fire_input) == true && armed == true) {
    digitalWrite(fire_led, HIGH);
    Serial.println("Commencing firing sequence");
    led_red();
    currentAction = 4; 
    
    // Keep o2 and propane on while turning on fire relay to start spark plug for 250 millisecond
    if (debounce(fire_input) == true) {
      Serial.println("Spark plug on");
      while (debounce(fire_input) == true && (millis() - current) < spark_time) {
        digitalWrite(o2_output, LOW);
        digitalWrite(propane_relay, LOW);
        digitalWrite(fire_relay, LOW); // turns on spark plug
      }
    } else {
      Serial.println("Fire aborted before spark plug turned on");
    }

    // Turns off both relays for spark plug (redundancy)
    digitalWrite(fire_relay, HIGH);
    digitalWrite(arm_relay, HIGH);
    digitalWrite(propane_relay, HIGH);

    // Keep o2 on while button is pressed
    if (debounce(fire_input) == true) {
      Serial.println("Firing while button is held");
      while(debounce(fire_input) == true);
    } else {
      Serial.println("Fire aborted after spark plug turned on but before firing");
    }

    // Just to make sure that everything is off
    digitalWrite(o2_output, HIGH);
    digitalWrite(propane_relay, HIGH);
    digitalWrite(arm_relay, HIGH);
    digitalWrite(fire_relay, HIGH);
    digitalWrite(fire_led, LOW);
    digitalWrite(arm_led, LOW);
    led_white();
      
    armed = false;
    Serial.println("Firing sequence finished");
  } else if (debounce(fire_input) == true) {
    Serial.println("Cannot fire without arming first");
  }

  if (currentAction != 3) {
    currentAction = 0;
  }
}


// Debounce function to get accurate button pressing input
bool debounce(int pin) {
  if (digitalRead(pin) == HIGH) {
    delay(10);
    if (digitalRead(pin) == HIGH) {
      delay(10);
      if (digitalRead(pin) == HIGH) {
        return true;
      }
    }
  }
  return false;
}

// Function to make LED's a certain color
void FillLEDsFromPaletteColors(uint8_t colorIndex) {
  uint8_t brightness = 255;
  for( int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
    colorIndex += 3;
  }
}


void led_white() {
  fill_solid(currentPalette, 16, CRGB::White);
  FillLEDsFromPaletteColors(0);
  FastLED.show();
}

void led_green() {
  fill_solid(currentPalette, 16, CRGB::Green);
  FillLEDsFromPaletteColors(0);
  FastLED.show();
}

void led_blue() {
  fill_solid(currentPalette, 16, CRGB::Blue);
  FillLEDsFromPaletteColors(0);
  FastLED.show();
}

void led_red() {
  fill_solid(currentPalette, 16, CRGB::Red);
  FillLEDsFromPaletteColors(0);
  FastLED.show();
}

void led_orange() {
  fill_solid(currentPalette, 16, CRGB::Orange);
  FillLEDsFromPaletteColors(0);
  FastLED.show();
}

void party() {
  currentPalette = RainbowStripeColors_p;
  currentBlending = LINEARBLEND;

  for (int i = 0; i < NUM_LEDS * 20; i++) {
    FillLEDsFromPaletteColors(i);
    FastLED.show();
    FastLED.delay(1000 / UPDATES_PER_SECOND);
  }
}


