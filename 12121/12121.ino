#define BLYNK_PRINT Serial
#include <SPI.h>
#include <WiFiS3.h>
#include <BlynkSimpleWifi.h>

char auth[] = "y91i6NledpsqcLH57HNWL-fDsxvVS-VS";
char ssid[] = "Napasorn 2G"; // ใส่ชื่อ WiFi
char pass[] = "tuktan25"; // ใส่รหัส WiFi

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

// ตัวแปรสำหรับสถานะสวิตช์
bool redBlinking = false;
bool greenLight1 = false;
bool greenLight2 = false;
bool greenLight3 = false;
bool greenLight4 = false;
bool yellowBlinking = false;

// เพิ่มตัวแปรสำหรับสถานะการทำงานของโหมดอัตโนมัติ
bool autoMode = true;

// ฟังก์ชันในการปิดไฟทุกตัว
void turnOffAllLights() {
  digitalWrite(GREEN_LIGHT1, LOW);
  digitalWrite(GREEN_LIGHT2, LOW);
  digitalWrite(GREEN_LIGHT3, LOW);
  digitalWrite(GREEN_LIGHT4, LOW);
  digitalWrite(YELLOW_LIGHT1, LOW);
  digitalWrite(YELLOW_LIGHT2, LOW);
  digitalWrite(YELLOW_LIGHT3, LOW);
  digitalWrite(YELLOW_LIGHT4, LOW);
}

// ฟังก์ชันสำหรับไฟจราจรเลนที่ 1
BLYNK_WRITE(V1) {
  if (autoMode) return; // ถ้าอยู่ในโหมดอัตโนมัติ ไม่ทำอะไร
  greenLight1 = true;
  digitalWrite(GREEN_LIGHT1, HIGH);
}

// ฟังก์ชันสำหรับไฟจราจรเลนที่ 2
BLYNK_WRITE(V2) {
  if (autoMode) return; // ถ้าอยู่ในโหมดอัตโนมัติ ไม่ทำอะไร
  greenLight2 = true;
  digitalWrite(GREEN_LIGHT2, HIGH);
}

// ฟังก์ชันสำหรับไฟจราจรเลนที่ 3
BLYNK_WRITE(V3) {
  if (autoMode) return; // ถ้าอยู่ในโหมดอัตโนมัติ ไม่ทำอะไร
  greenLight3 = true;
  digitalWrite(GREEN_LIGHT3, HIGH);
}

// ฟังก์ชันสำหรับไฟจราจรเลนที่ 4
BLYNK_WRITE(V4) {
  if (autoMode) return; // ถ้าอยู่ในโหมดอัตโนมัติ ไม่ทำอะไร
  greenLight4 = true;
  digitalWrite(GREEN_LIGHT4, HIGH);
}

// ฟังก์ชันสำหรับไฟแดง
BLYNK_WRITE(V0) {
  autoMode = false;
  redBlinking = true;
  turnOffAllLights(); // ดับไฟเขียวและเหลืองทั้งหมด
}

// ฟังก์ชันสำหรับการเปิดโหมด V5
BLYNK_WRITE(V5) {
  autoMode = false; // ออกจากโหมดอัตโนมัติ
  turnOffAllLights(); // ปิดไฟทุกตัว
}

// ฟังก์ชันสำหรับการควบคุมไฟจราจรในแต่ละเลน
void controlTrafficLane(int greenPin, int yellowPin) {
  // เปิดไฟเขียวเป็นเวลา 3 วินาที
  digitalWrite(greenPin, HIGH);
  delay(3000);
  digitalWrite(greenPin, LOW);
  // เปิดไฟเหลืองเป็นเวลา 1 วินาที
  digitalWrite(yellowPin, HIGH);
  delay(1000);
  digitalWrite(yellowPin, LOW);
}

// ฟังก์ชันสำหรับการทำงานแบบอัตโนมัติ
void runAutoMode() {
  // ฟังก์ชันนี้สามารถใส่ได้ตามที่คุณต้องการสำหรับโหมดอัตโนมัติ
}

void setup() {
  // เริ่มต้นการเชื่อมต่อ
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass, "blynk2.iot-cm.com", 8080);
  
  // กำหนดโหมดพิน
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
  Blynk.run(); // เรียกใช้ Blynk
  runAutoMode(); // ทำงานโหมดอัตโนมัติ
}
