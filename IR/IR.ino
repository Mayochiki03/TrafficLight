#define irsensor 12
#define Button 11
int buttonState = 0;

#define Red_Cross_light 9
#define Green_Cross_light 10

int val = 0;
unsigned long detectionStartTime = 0;
bool objectDetected = false;
bool buttonPressed = false;

void setup() {
  pinMode(Green_Cross_light, OUTPUT);  // sets the pin as output
  pinMode(Red_Cross_light, OUTPUT);  // sets the pin as output
  pinMode(irsensor, INPUT);           // sets the pin as input
  pinMode(Button, INPUT_PULLUP);      // Use internal pull-up resistor for button

  Serial.begin(9600);
  digitalWrite(Red_Cross_light, HIGH); // Start with red light on
}

void loop() {
  val = digitalRead(irsensor);         // Read the IR sensor 'value'
  buttonState = digitalRead(Button);    // Read the button state
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
    detectionStartTime = millis();   // Record the start time
    objectDetected = true;
  }

  // Check if IR sensor has detected an object for more than 3 seconds or button is pressed
  if ((objectDetected && (val == 0 && millis() - detectionStartTime > 3000)) || buttonPressed) {
    digitalWrite(Red_Cross_light, LOW);    // Turn off the red light
    digitalWrite(Green_Cross_light, HIGH); // Turn on the green light
    delay(5000);                            // Keep green light on for 5 seconds
    digitalWrite(Green_Cross_light, LOW);  // Turn off green light after 5 seconds
    digitalWrite(Red_Cross_light, HIGH);   // Turn red light back on
    objectDetected = false;                // Reset detection after action
    buttonPressed = false;                 // Reset button pressed status
  } else if (val != 0) {
    // Reset detection if no object is detected
    objectDetected = false;
    detectionStartTime = 0;  // Reset the timer
  }

  delay(100); // Small delay to stabilize readings
}
