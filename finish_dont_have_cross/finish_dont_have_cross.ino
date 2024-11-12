#define BLYNK_PRINT Serial
#include <SPI.h>
#include <WiFiS3.h>
#include <BlynkSimpleWifi.h>

char auth[] = "..............";
char ssid[] = "..............";  // WiFi name
char pass[] = "..............";     // WiFi password

// กำหนดพินสำหรับไฟจราจร
#define RED_LIGHT D0
#define YELLOW_LIGHT1 D1
#define GREEN_LIGHT1 D2
#define YELLOW_LIGHT2 D3
#define GREEN_LIGHT2 D4
#define YELLOW_LIGHT3 D5
#define GREEN_LIGHT3 D6
#define YELLOW_LIGHT4 D7
#define GREEN_LIGHT4 D8

unsigned long previousMillis = 0;
const long intervalGreen = 3000;   // 3 วินาทีสำหรับไฟเขียว
const long intervalYellow = 1000;  // 1 วินาทีสำหรับไฟเหลือง
int currentLane = 1;               // เริ่มที่เลน 1

bool redBlinking = false;
bool greenLight1 = false;
bool greenLight2 = false;
bool greenLight3 = false;
bool greenLight4 = false;

bool yellowBlinking = false;
unsigned long previousYellowMillis = 0;
const long yellowBlinkInterval = 500;  // กำหนดให้ไฟเหลืองกระพริบทุกๆ 500 มิลลิวินาที

bool greenBlinking = false;
unsigned long previousGreenMillis = 0;
const long blinkInterval = 500;  // กำหนดให้ไฟเขียวกระพริบทุกๆ 500 มิลลิวินาที

bool autoMode = true;
bool v5Active = false;  // ตัวแปรเพื่อเช็คสถานะ V5

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

// ฟังก์ชันการทำงานของ V5 เพื่อเปิดหรือปิดการทำงานของปุ่ม V1-V7
BLYNK_WRITE(V5) {
  if (param.asInt() == 1) {
    // เมื่อกดปุ่ม V5
    v5Active = true;

    // ปิดไฟทั้งหมดเมื่อ V5 ถูกเปิดใช้งาน
    digitalWrite(GREEN_LIGHT1, LOW);
    digitalWrite(GREEN_LIGHT2, LOW);
    digitalWrite(GREEN_LIGHT3, LOW);
    digitalWrite(GREEN_LIGHT4, LOW);
    digitalWrite(YELLOW_LIGHT1, LOW);
    digitalWrite(YELLOW_LIGHT2, LOW);
    digitalWrite(YELLOW_LIGHT3, LOW);
    digitalWrite(YELLOW_LIGHT4, LOW);

    // หยุดโหมดอัตโนมัติ
    autoMode = false; 
  } else {
    // เมื่อปล่อย V5
    v5Active = false;

    // เริ่มกลับไปโหมดอัตโนมัติ
    autoMode = true; 
  }
}

BLYNK_WRITE(V1) {
  if (v5Active) {
    autoMode = false; // หยุดโหมดอัตโนมัติเมื่อกด V1
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
    autoMode = false; // หยุดโหมดอัตโนมัติเมื่อกด V2
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
    autoMode = false; // หยุดโหมดอัตโนมัติเมื่อกด V3
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
    autoMode = false; // หยุดโหมดอัตโนมัติเมื่อกด V4
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
    autoMode = false; // หยุดโหมดอัตโนมัติเมื่อกด V0
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

BLYNK_WRITE(V6) {
  
}

BLYNK_WRITE(V7) {
  if (v5Active) {
    autoMode = false; // หยุดโหมดอัตโนมัติเมื่อกด V7
    if (param.asInt() == 1) {
      yellowBlinking = true;
    } else {
      yellowBlinking = false;
      digitalWrite(YELLOW_LIGHT1, LOW);
      digitalWrite(YELLOW_LIGHT2, LOW);
      digitalWrite(YELLOW_LIGHT3, LOW);
      digitalWrite(YELLOW_LIGHT4, LOW);
    }
  }
}

void controlTrafficLane(int greenPin, int yellowPin) {
  digitalWrite(greenPin, HIGH);
  delay(3000); // รอ 3 วินาทีก่อนเปลี่ยนไปไฟเหลือง
  digitalWrite(greenPin, LOW);
  digitalWrite(yellowPin, HIGH);
  delay(1000); // รอ 1 วินาทีก่อนเปลี่ยนไปไฟแดง
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
}

void loop() {
  Blynk.run();
  checkRedLight();
  runAutoMode(); // จะทำงานเมื่อ autoMode เป็น true
}
