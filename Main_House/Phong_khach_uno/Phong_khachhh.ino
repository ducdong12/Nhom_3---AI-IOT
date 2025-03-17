#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>

// --- Cấu hình WiFi ---
const char* ssid = "kevin kaslana";
const char* password = "12345678";

// Địa chỉ server Python (Flask)
String serverIP = "192.168.82.9"; 
String serverPort = "5000";

// Endpoint cập nhật dữ liệu cho phòng khách (chỉ gửi các thông số cảm biến)
String livingRoomUpdateEndpoint = "http://" + serverIP + ":" + serverPort + "/update?room=living_room";
// Endpoint điều khiển và lấy trạng thái từ server
String livingRoomSetLEDEndpoint    = "http://" + serverIP + ":" + serverPort + "/set_led?room=living_room&state=";
String livingRoomSetFanEndpoint     = "http://" + serverIP + ":" + serverPort + "/set_fan?state=";
String livingRoomSetSensorEndpoint  = "http://" + serverIP + ":" + serverPort + "/set_sensor?room=living_room&state=";
String livingRoomGetLEDEndpoint     = "http://" + serverIP + ":" + serverPort + "/get_led?room=living_room";
String livingRoomGetFanEndpoint      = "http://" + serverIP + ":" + serverPort + "/get_fan?room=living_room";
String livingRoomGetSensorEndpoint   = "http://" + serverIP + ":" + serverPort + "/get_sensor?room=living_room";

// ===== CẤU HÌNH CẢM BIẾN =====
// DHT11 (sử dụng chân D5 trên ESP8266, tức GPIO14)
#define DHTPIN 14
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// HC-SR04 (đặt trig và echo theo chân có sẵn trên board NodeMCU)
const int trigPin = 5;  // D1 (GPIO5)
const int echoPin = 4;  // D2 (GPIO4) //Phần chân cắm cảm biến khoảng cách

// ===== CẤU HÌNH ĐIỆN RA =====
// Các chân kết nối với LED và quạt (các chân có thể thay đổi tùy board)
const int led1Pin = 13;  // LED1: điều khiển từ server
const int led2Pin = 15;  // LED2: bật khi phát hiện chuyển động
const int fanPin    = 2;  // Quạt

// ===== BIẾN TRẠNG =====
bool fanState = false;       // Quạt OFF ban đầu
bool led1State = false;      // LED1 OFF ban đầu
bool sensorEnabled = true;   // Cảm biến HC-SR04 bật mặc định (có thể cập nhật từ server)
bool led2State = false;      // LED2 (chuyển động) OFF ban đầu
bool motionDetected = false; // Đánh dấu chuyển động

// Thời gian LED2 bật khi có chuyển động (15 giây)
unsigned long lastMotionTime = 0;
const unsigned long motionTimeout = 15000;  // 15,000 ms

// Ngưỡng khoảng cách HC-SR04: nếu đo được < distanceThreshold (cm) xem như có chuyển động
const float distanceThreshold = 5.0;  // (Điều chỉnh nếu cần)

// ----- Hàm đọc cảm biến HC-SR04 -----
float readDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH, 30000);  // timeout 30,000 µs
  float distance = (duration / 2.0) * 0.0343;  // Tính theo cm
  return distance;
}

// ----- Hàm gửi dữ liệu phòng khách (chỉ cảm biến) -----
void sendLivingRoomData(float temperature, float humidity, float distance) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;
    String motionStr = motionDetected ? "YES" : "NO";
    String url = livingRoomUpdateEndpoint +
                 "&temp=" + String(temperature, 1) +
                 "&hum=" + String(humidity, 1) +
                 "&distance=" + String(distance, 1) +
                 "&motion=" + motionStr;
    http.begin(client, url);  // Sử dụng API mới với WiFiClient
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Living room update: " + response);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
}

// ----- Hàm cập nhật trạng thái LED1 từ server -----
void updateLivingRoomLEDFromServer() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;
    http.begin(client, livingRoomGetLEDEndpoint);  // API mới
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      String payload = http.getString();
      if (payload.indexOf("ON") != -1 && !led1State) {
        led1State = true;
        digitalWrite(led1Pin, HIGH);
        Serial.println("Remote LED1 ON");
      } else if (payload.indexOf("OFF") != -1 && led1State) {
        led1State = false;
        digitalWrite(led1Pin, LOW);
        Serial.println("Remote LED1 OFF");
      }
    }
    http.end();
  }
}

// ----- Hàm cập nhật trạng thái Quạt từ server -----
void updateLivingRoomFanFromServer() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;
    http.begin(client, livingRoomGetFanEndpoint);  // API mới
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      String payload = http.getString();
      if (payload.indexOf("ON") != -1 && !fanState) {
        fanState = true;
        digitalWrite(fanPin, HIGH);
        Serial.println("Remote Fan ON");
      } else if (payload.indexOf("OFF") != -1 && fanState) {
        fanState = false;
        digitalWrite(fanPin, LOW);
        Serial.println("Remote Fan OFF");
      }
    }
    http.end();
  }
}

// ----- Hàm cập nhật trạng thái Cảm biến từ server -----
void updateLivingRoomSensorFromServer() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;
    http.begin(client, livingRoomGetSensorEndpoint);  // API mới
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      String payload = http.getString();
      // Giả sử server trả về JSON: {"sensor": "YES"} hoặc {"sensor": "NO"}
      if (payload.indexOf("YES") != -1) {
        sensorEnabled = true;
        Serial.println("Remote sensor ENABLED");
      } else if (payload.indexOf("NO") != -1) {
        sensorEnabled = false;
        if (led2State) {
          led2State = false;
          digitalWrite(led2Pin, LOW);
        }
        Serial.println("Remote sensor DISABLED");
      }
    }
    http.end();
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  // Cấu hình HC-SR04
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // Cấu hình các thiết bị đầu ra
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  pinMode(fanPin, OUTPUT);
  
  // Khởi tạo trạng thái ban đầu
  digitalWrite(led1Pin, LOW);
  digitalWrite(led2Pin, LOW);
  digitalWrite(fanPin, LOW);
  
  // ===== KẾT NỐI WIFI =====
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
}

void loop() {
  // Đọc cảm biến DHT11
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  float distance = 0.0;
  if (sensorEnabled) {
    distance = readDistance();
  }
  
  // ----- XỬ LÝ CHUYỂN ĐỘNG -----
  if (sensorEnabled) {
    if (distance > 0 && distance < distanceThreshold) {
      motionDetected = true;
      lastMotionTime = millis();
      if (!led2State) {
        led2State = true;
        digitalWrite(led2Pin, HIGH);
        Serial.println("Motion detected: LED2 ON");
      }
    } else {
      if (motionDetected && (millis() - lastMotionTime >= motionTimeout)) {
        motionDetected = false;
        if (led2State) {
          led2State = false;
          digitalWrite(led2Pin, LOW);
          Serial.println("No motion: LED2 OFF");
        }
      }
    }
  }
  
  // ----- GỬI DỮ LIỆU PHÒNG KHÁCH (chỉ cảm biến) -----
  sendLivingRoomData(temperature, humidity, distance);
  
  // ----- ĐỒNG BỘ TRẠNG THÁI TỪ SERVER (điều khiển từ web) -----
  updateLivingRoomLEDFromServer();
  updateLivingRoomFanFromServer();
  updateLivingRoomSensorFromServer();
  
  delay(250);
}
