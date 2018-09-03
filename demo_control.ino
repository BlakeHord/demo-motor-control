/*  Demonstration Motor Controls
     Blake Hord
     Last Updated Aug 21, 2018

     For use in the hybrid motor to be used in Stanford's AA 103 Propulsion Class
     Developed in Professor Brian Cantwell's lab under the guidance of PhD students Flora Mechentel and David Dyrda
*/

int propane_input = 6;
int o2_input = 2;
int arm_input = 9;
int fire_input = 3;
int arm_led = 4;
int fire_led = 5;
int o2_relay = 10;
int propane_relay = 11;
int arm_relay = 12;
int fire_relay = 13;
unsigned long count = 0;
bool armed = false;

bool debounce(int pin);

void setup() {
  // Controller Pins
  pinMode(o2_input, INPUT);
  pinMode(propane_input, INPUT);
  pinMode(arm_input, INPUT);
  pinMode(fire_input, INPUT);

  pinMode(arm_led, OUTPUT);
  pinMode(fire_led, OUTPUT);

  digitalWrite(arm_led, LOW);
  digitalWrite(fire_led, LOW);

  // Relay Pins
  pinMode(o2_relay, OUTPUT);
  pinMode(propane_relay, OUTPUT);
  pinMode(arm_relay, OUTPUT);
  pinMode(fire_relay, OUTPUT);

  digitalWrite(o2_relay, LOW);
  digitalWrite(propane_relay, LOW);
  digitalWrite(arm_relay, LOW);
  digitalWrite(fire_relay, LOW);

  // Serial with 57600 baud rate
  Serial.begin(57600);
}


// I'm going to try to complete this without messy interrupts but we'll see how that works
void loop() {
  count += 1;


  // OXYGEN VENT CONTROLLER
  while (debounce(o2_input) == true) {
    Serial.println("oxygen vent on");
    Serial.println(count);
    digitalWrite(o2_relay, HIGH);
  }
  digitalWrite(o2_relay, LOW);


  // PROPANE VENT CONTROLLER
  while (debounce(propane_input) == true) {
    Serial.println("propane vent on");
    Serial.println(count);
    digitalWrite(propane_relay, HIGH);
  }
  digitalWrite(propane_relay, LOW);


  // ARM CONTROLLER
  if (debounce(arm_input) == true) {
    while (debounce(arm_input) == true);

    if (armed == true) {
      digitalWrite(arm_relay, LOW);
      digitalWrite(arm_led, LOW);
      armed = false;
      Serial.println("Disarmed");
      Serial.println(count);
    } else if (armed == false) {
      digitalWrite(arm_relay, HIGH);
      digitalWrite(arm_led, HIGH);
      armed = true;
      Serial.println("Armed");
      Serial.println(count);
    }
  }


  // FIRING SEQUENCE CONTROLLER
  unsigned long current = millis();
  if (debounce(fire_input) == true && armed == true) {
    digitalWrite(fire_led, HIGH);
    Serial.println("Commencing firing sequence");
    
    // Start o2 and propane for 1 second
    if (debounce(fire_input) == true) {
      Serial.println("Flowing O2 and Propane for 1 second");
      while (debounce(fire_input) == true && (millis() - current) < 1000) {
        digitalWrite(o2_relay, HIGH);
        digitalWrite(propane_relay, HIGH);
      }
    } else {
      Serial.println("Fire aborted before starting O2 and propane flow");
    }
    
    // Keep o2 and propane on while turning on fire relay to start spark plug for 1 second
    if (debounce(fire_input) == true) {
      Serial.println("Spark plug on");
      while (debounce(fire_input) == true && (millis() - current) < 2000) {
        digitalWrite(o2_relay, HIGH);
        digitalWrite(propane_relay, HIGH);
        digitalWrite(fire_relay, HIGH); // turns on spark plug
      }
    } else {
      Serial.println("Fire aborted before spark plug turned on");
    }

    // Turns off both relays for spark plug (redundancy)
    digitalWrite(fire_relay, LOW);
    digitalWrite(arm_relay, LOW);
    digitalWrite(propane_relay, LOW);

    // Keep o2 on while button is pressed
    if (debounce(fire_input) == true) {
      Serial.println("Firing while button is held");
      while(debounce(fire_input) == true);
    } else {
      Serial.println("Fire aborted after spark plug turned on but before firing");
    }

    // Just to make sure that everything is off
    digitalWrite(o2_relay, LOW);
    digitalWrite(propane_relay, LOW);
    digitalWrite(arm_relay, LOW);
    digitalWrite(fire_relay, LOW);
    digitalWrite(fire_led, LOW);
    digitalWrite(arm_led, LOW);
    armed = false;
    Serial.println("Firing sequence finished");
  } else if (debounce(fire_input) == true) {
    Serial.println("Cannot fire without arming first");
  }
}



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




