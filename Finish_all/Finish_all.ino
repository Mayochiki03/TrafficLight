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

// Define pins for crosswalk lights and sensors
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
bool v5Active = false;          // Variable to check status of V5
bool v7Active = false;          // Variable to check status of V7
bool crosswalkEnabled = false;  // Flag for crosswalk control
int buttonState = 0;
int val = 0;
unsigned long detectionStartTime = 0;
bool objectDetected = false;
bool buttonPressed = false;

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

    autoMode = false;
  } else {
    v5Active = false;
    autoMode = true;
  }
}

BLYNK_WRITE(V1) {
  if (v5Active) {
    autoMode = false;
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
    autoMode = false;
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
    autoMode = false;
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
    autoMode = false;
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
    autoMode = false;
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
    crosswalkEnabled = true;              // Enable crosswalk
    digitalWrite(Red_Cross_light, HIGH);  // Ensure red cross light is on initially
  } else {
    crosswalkEnabled = false;              // Disable crosswalk
    digitalWrite(Green_Cross_light, LOW);  // Ensure green cross light is off
    digitalWrite(Red_Cross_light, HIGH);   // Ensure red cross light is on
  }
}

// V7 to override auto mode and set all lights to green
BLYNK_WRITE(V7) {
  if (param.asInt() == 1) {
    v7Active = true;
    autoMode = false;  // Turn off auto mode when V7 is active

    // Turn off all green and yellow lights
    digitalWrite(GREEN_LIGHT1, LOW);
    digitalWrite(GREEN_LIGHT2, LOW);
    digitalWrite(GREEN_LIGHT3, LOW);
    digitalWrite(GREEN_LIGHT4, LOW);
    digitalWrite(YELLOW_LIGHT1, LOW);
    digitalWrite(YELLOW_LIGHT2, LOW);
    digitalWrite(YELLOW_LIGHT3, LOW);
    digitalWrite(YELLOW_LIGHT4, LOW);
    digitalWrite(Green_Cross_light, HIGH);  // Ensure green cross light is off
    digitalWrite(Red_Cross_light, LOW);   // Ensure red cross light is on


  } else {
    v7Active = false;
    autoMode = true;  // Re-enable auto mode when V7 is deactivated

    // Turn off all green lights to prepare for auto mode
    digitalWrite(Red_Cross_light, HIGH);   // Ensure red cross light is on
    digitalWrite(Green_Cross_light, LOW);  // Ensure green cross light is off

  }
}

void manageCrosswalk() {
  if (!crosswalkEnabled) return;      // Only run if crosswalk is enabled by V6
  val = digitalRead(irsensor);        // Read the IR sensor 'value'
  buttonState = digitalRead(Button);  // Read the button state
  Serial.print("val = ");
  Serial.println(val);
  Serial.print("buttonState = ");
  Serial.println(buttonState);

  // Check if button is pressed (LOW when pressed due to pull-up)
  if (buttonState == LOW && !buttonPressed) {
    buttonPressed = true;
    objectDetected = true;  // Simulate object detection when button is pressed
  }

  // If IR sensor detects an object
  if (val == 0 && !objectDetected) {
    detectionStartTime = millis();  // Record the start time
    objectDetected = true;
  }

  // Check if IR sensor has detected an object for more than 3 seconds or button is pressed
  if ((objectDetected && (val == 0 && millis() - detectionStartTime > 3000)) || buttonPressed) {
    digitalWrite(Red_Cross_light, LOW);     // Turn off the red light
    digitalWrite(Green_Cross_light, HIGH);  // Turn on the green light
    delay(5000);                            // Keep green light on for 5 seconds
    digitalWrite(Green_Cross_light, LOW);   // Turn off green light after 5 seconds
    digitalWrite(Red_Cross_light, HIGH);    // Turn red light back on
    objectDetected = false;                 // Reset detection after action
    buttonPressed = false;                  // Reset button pressed status
  } else if (val != 0) {
    // Reset detection if no object is detected
    objectDetected = false;
    detectionStartTime = 0;  // Reset the timer
  }

  delay(100);  // Small delay to stabilize readings
}
void controlTrafficLane(int greenPin, int yellowPin) {
  digitalWrite(YELLOW_LIGHT1, LOW);
  digitalWrite(YELLOW_LIGHT2, LOW);
  digitalWrite(YELLOW_LIGHT3, LOW);
  digitalWrite(YELLOW_LIGHT4, LOW);

  digitalWrite(greenPin, HIGH);
  delay(3000);
  digitalWrite(greenPin, LOW);
  digitalWrite(yellowPin, HIGH);
  delay(1000);
  digitalWrite(yellowPin, LOW);
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

  pinMode(RED_LIGHT, OUTPUT);
  pinMode(YELLOW_LIGHT1, OUTPUT);
  pinMode(GREEN_LIGHT1, OUTPUT);
  pinMode(YELLOW_LIGHT2, OUTPUT);
  pinMode(GREEN_LIGHT2, OUTPUT);
  pinMode(YELLOW_LIGHT3, OUTPUT);
  pinMode(GREEN_LIGHT3, OUTPUT);
  pinMode(YELLOW_LIGHT4, OUTPUT);
  pinMode(GREEN_LIGHT4, OUTPUT);

  pinMode(irsensor, INPUT);
  pinMode(Button, INPUT_PULLUP);  // Button with pull-up resistor
  pinMode(Red_Cross_light, OUTPUT);
  pinMode(Green_Cross_light, OUTPUT);

  digitalWrite(Red_Cross_light, HIGH);  // Crosswalk red light on initially
}

void loop() {
  Blynk.run();
  checkRedLight();
  runAutoMode();
  manageCrosswalk();  // Manage crosswalk with button and IR sensor
  delay(100);         // Small delay to stabilize readings
}
