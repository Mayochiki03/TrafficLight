#define BLYNK_PRINT Serial
#include <SPI.h>
#include <WiFiS3.h>
#include <BlynkSimpleWifi.h>

char auth[] = "..............";
char ssid[] = "..............";  // WiFi name
char pass[] = "..............";     // WiFi password

// Define pins for traffic lights
#define RED_LIGHT D0
#define YELLOW_LIGHT1 D1
#define GREEN_LIGHT1 D2
#define YELLOW_LIGHT2 D3
#define GREEN_LIGHT2 D4
#define YELLOW_LIGHT3 D5
#define GREEN_LIGHT3 D6
#define YELLOW_LIGHT4 D7
#define GREEN_LIGHT4 D8

// Define pins for crosswalk lights
#define irsensor 12
#define Button 11
#define Red_Cross_light 9
#define Green_Cross_light 10

unsigned long previousMillis = 0;
const long intervalGreen = 3000;   // 3 seconds for green light
const long intervalYellow = 1000;  // 1 second for yellow light
int currentLane = 1;               // Start with lane 1

bool redBlinking = false;
bool greenLight1 = false;
bool greenLight2 = false;
bool greenLight3 = false;
bool greenLight4 = false;

bool yellowBlinking = false;
unsigned long previousYellowMillis = 0;
const long yellowBlinkInterval = 500;  // Yellow blink interval

bool greenBlinking = false;
unsigned long previousGreenMillis = 0;
const long blinkInterval = 500;  // Green blink interval

bool autoMode = true;
bool v5Active = false;  // Variable to check status of V5
bool crosswalkEnabled = false; // Flag for crosswalk control
bool objectDetected = false; // Flag for IR sensor detection
unsigned long detectionStartTime = 0; // Time tracking for detection

void checkRedLight() {
  if (redBlinking) {
    digitalWrite(RED_LIGHT, HIGH);
    delay(500);
    digitalWrite(RED_LIGHT, LOW);
    delay(500);
  } else {
    digitalWrite(RED_LIGHT, LOW);
  }
}

// Function for V5 to enable/disable buttons V1-V7
BLYNK_WRITE(V5) {
  if (param.asInt() == 1) {
    // When button V5 is pressed
    v5Active = true;

    // Turn off all lights when V5 is activated
    digitalWrite(GREEN_LIGHT1, LOW);
    digitalWrite(GREEN_LIGHT2, LOW);
    digitalWrite(GREEN_LIGHT3, LOW);
    digitalWrite(GREEN_LIGHT4, LOW);
    digitalWrite(YELLOW_LIGHT1, LOW);
    digitalWrite(YELLOW_LIGHT2, LOW);
    digitalWrite(YELLOW_LIGHT3, LOW);
    digitalWrite(YELLOW_LIGHT4, LOW);

    // Stop auto mode
    autoMode = false; 
  } else {
    // When V5 is released
    v5Active = false;

    // Restart auto mode
    autoMode = true; 
  }
}

BLYNK_WRITE(V1) {
  if (v5Active) {
    autoMode = false; // Stop auto mode when V1 is pressed
    if (param.asInt() == 1) {
      greenLight1 = true;
      digitalWrite(GREEN_LIGHT1, HIGH);
    } else {
      greenLight1 = false;
      digitalWrite(GREEN_LIGHT1, LOW);
      digitalWrite(YELLOW_LIGHT1, HIGH);
      delay(3000);
      digitalWrite(YELLOW_LIGHT1, LOW);
    }
  }
}

BLYNK_WRITE(V2) {
  if (v5Active) {
    autoMode = false; // Stop auto mode when V2 is pressed
    if (param.asInt() == 1) {
      greenLight2 = true;
      digitalWrite(GREEN_LIGHT2, HIGH);
    } else {
      greenLight2 = false;
      digitalWrite(GREEN_LIGHT2, LOW);
      digitalWrite(YELLOW_LIGHT2, HIGH);
      delay(3000);
      digitalWrite(YELLOW_LIGHT2, LOW);
    }
  }
}

BLYNK_WRITE(V3) {
  if (v5Active) {
    autoMode = false; // Stop auto mode when V3 is pressed
    if (param.asInt() == 1) {
      greenLight3 = true;
      digitalWrite(GREEN_LIGHT3, HIGH);
    } else {
      greenLight3 = false;
      digitalWrite(GREEN_LIGHT3, LOW);
      digitalWrite(YELLOW_LIGHT3, HIGH);
      delay(3000);
      digitalWrite(YELLOW_LIGHT3, LOW);
    }
  }
}

BLYNK_WRITE(V4) {
  if (v5Active) {
    autoMode = false; // Stop auto mode when V4 is pressed
    if (param.asInt() == 1) {
      greenLight4 = true;
      digitalWrite(GREEN_LIGHT4, HIGH);
    } else {
      greenLight4 = false;
      digitalWrite(GREEN_LIGHT4, LOW);
      digitalWrite(YELLOW_LIGHT4, HIGH);
      delay(3000);
      digitalWrite(YELLOW_LIGHT4, LOW);
    }
  }
}

BLYNK_WRITE(V0) {
  if (v5Active) {
    autoMode = false; // Stop auto mode when V0 is pressed
    if (param.asInt() == 1) {
      redBlinking = true;
      digitalWrite(GREEN_LIGHT1, LOW);
      digitalWrite(GREEN_LIGHT2, LOW);
      digitalWrite(GREEN_LIGHT3, LOW);
      digitalWrite(GREEN_LIGHT4, LOW);
    } else {
      redBlinking = false;
      digitalWrite(RED_LIGHT, LOW);
    }
  }
}

// V6 to enable/disable crosswalk functionality
BLYNK_WRITE(V6) {
  if (param.asInt() == 1) {
    crosswalkEnabled = true; // Enable crosswalk
  } else {
    crosswalkEnabled = false; // Disable crosswalk
    digitalWrite(Green_Cross_light, LOW); // Ensure the green cross light is off
    digitalWrite(Red_Cross_light, HIGH); // Ensure the red cross light is on
  }
}

void manageCrosswalk() {
  // Only check the IR sensor if crosswalk is enabled
  if (crosswalkEnabled) {
    int val = digitalRead(irsensor); // Read the IR sensor value
    Serial.print("val = ");
    Serial.println(val);

    if (val == 0) { // If an object is detected
      if (!objectDetected) { // First time detection
        detectionStartTime = millis(); // Record the start time
        objectDetected = true;
      }
      // Check if the object has been detected for more than 3 seconds
      if (millis() - detectionStartTime > 3000) {
        // Turn off traffic lights
        digitalWrite(GREEN_LIGHT1, LOW);
        digitalWrite(GREEN_LIGHT2, LOW);
        digitalWrite(GREEN_LIGHT3, LOW);
        digitalWrite(GREEN_LIGHT4, LOW);
        digitalWrite(YELLOW_LIGHT1, LOW);
        digitalWrite(YELLOW_LIGHT2, LOW);
        digitalWrite(YELLOW_LIGHT3, LOW);
        digitalWrite(YELLOW_LIGHT4, LOW);
        
        // Change traffic light state
        digitalWrite(Red_Cross_light, LOW); // Turn off red cross light
        digitalWrite(Green_Cross_light, HIGH); // Turn on green cross light
        delay(5000); // Keep green light on for 5 seconds
        digitalWrite(Green_Cross_light, LOW); // Turn off green light
        digitalWrite(Red_Cross_light, HIGH); // Turn red light back on
        
        // Turn off all traffic lane lights
        digitalWrite(GREEN_LIGHT1, LOW);
        digitalWrite(GREEN_LIGHT2, LOW);
        digitalWrite(GREEN_LIGHT3, LOW);
        digitalWrite(GREEN_LIGHT4, LOW);
      }
    } else {
      objectDetected = false; // Reset detection if no object
      digitalWrite(Green_Cross_light, LOW); // Ensure green light is off
      digitalWrite(Red_Cross_light, HIGH); // Keep the red light on
    }
  }
}

void controlTrafficLane(int greenPin, int yellowPin) {
  // Turn off yellow lights before activating the next green light
  digitalWrite(YELLOW_LIGHT1, LOW);
  digitalWrite(YELLOW_LIGHT2, LOW);
  digitalWrite(YELLOW_LIGHT3, LOW);
  digitalWrite(YELLOW_LIGHT4, LOW);

  digitalWrite(greenPin, HIGH); // Activate the green light
  delay(3000); // Wait 3 seconds before changing to yellow
  digitalWrite(greenPin, LOW);
  digitalWrite(yellowPin, HIGH); // Activate the yellow light
  delay(1000); // Wait 1 second before changing to red
  digitalWrite(yellowPin, LOW); // Turn off the yellow light
}

void runAutoMode() {
  unsigned long currentMillis = millis();
  if (autoMode && (currentMillis - previousMillis >= intervalGreen + intervalYellow)) {
    previousMillis = currentMillis;
    switch (currentLane) {
      case 1:
        controlTrafficLane(GREEN_LIGHT1, YELLOW_LIGHT1);
        currentLane = 2;
        break;
      case 2:
        controlTrafficLane(GREEN_LIGHT2, YELLOW_LIGHT2);
        currentLane = 3;
        break;
      case 3:
        controlTrafficLane(GREEN_LIGHT3, YELLOW_LIGHT3);
        currentLane = 4;
        break;
      case 4:
        controlTrafficLane(GREEN_LIGHT4, YELLOW_LIGHT4);
        currentLane = 1;
        break;
    }
  }
}

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass, "blynk2.iot-cm.com", 8080);

  // Initialize traffic light pins
  pinMode(RED_LIGHT, OUTPUT);
  pinMode(YELLOW_LIGHT1, OUTPUT);
  pinMode(GREEN_LIGHT1, OUTPUT);
  pinMode(YELLOW_LIGHT2, OUTPUT);
  pinMode(GREEN_LIGHT2, OUTPUT);
  pinMode(YELLOW_LIGHT3, OUTPUT);
  pinMode(GREEN_LIGHT3, OUTPUT);
  pinMode(YELLOW_LIGHT4, OUTPUT);
  pinMode(GREEN_LIGHT4, OUTPUT);

  // Initialize crosswalk light pins
  pinMode(Red_Cross_light, OUTPUT);
  pinMode(Green_Cross_light, OUTPUT);
  pinMode(irsensor, INPUT); // Initialize IR sensor pin
  digitalWrite(Red_Cross_light, HIGH); // Ensure the red cross light is on initially
}

void loop() {
  Blynk.run();
  checkRedLight();
  runAutoMode(); // This will work when autoMode is true
  manageCrosswalk(); // Manage crosswalk lights based on IR sensor detection
  delay(100); // Small delay to stabilize readings
}
