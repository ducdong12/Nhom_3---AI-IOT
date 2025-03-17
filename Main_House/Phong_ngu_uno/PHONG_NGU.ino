#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>
#include <Servo.h>

// --- Cấu hình WiFi ---
const char* ssid = "kevin kaslana";
const char* password = "12345678";

// Địa chỉ server Python (Flask)
String serverIP = "192.168.82.9"; 
String serverPort = "5000";
String updateEndpoint = "http://192.168.82.9:5000/update";

// ----- CẤU HÌNH CẢM BIẾN & ĐIỆN RA -----
// DHT11: đo nhiệt độ, độ ẩm
#define DHTPIN 14    // VD: sử dụng GPIO14 (D5) cho DHT11
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHT11);

// Rain sensor: sử dụng chân digital (nếu có mưa, tín hiệu = LOW)
#define RAINPIN D6   // VD: GPIO12 (D6)

// Đèn LED
#define LEDPIN 13   // VD: GPIO13 (D7)

// Quạt (2 chân): chỉ cần bật/tắt
#define FANPIN 16   // VD: GPIO16 (D0)

// MicroServo: dùng để điều khiển cửa sổ (0° = đóng, 90° = mở)
#define SERVO_PIN 5  // VD: GPIO5 (D1)
Servo windowServo;

// Biến trạng thái điều khiển (được đồng bộ từ server)
bool ledState = false;
bool fanState = false;
String windowState = "CLOSED";  // "OPEN" hoặc "CLOSED"

// ----- Hàm kết nối WiFi -----
void setupWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
}

// ----- Hàm gửi dữ liệu cảm biến lên server -----
// Gửi dữ liệu cho phòng "bedroom": nhiệt độ, độ ẩm, và trạng thái mưa ("RAIN" nếu mưa, "DRY" nếu không)
void sendBedroomData(float temperature, float humidity, bool isRaining) {
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;
    String rainStr = isRaining ? "RAIN" : "DRY";
    String url = updateEndpoint + "?room=bedroom&temp=" + String(temperature, 1) +
                 "&hum=" + String(humidity, 1) +
                 "&rain=" + rainStr;
    http.begin(client, url);
    int httpCode = http.GET();
    if(httpCode > 0) {
      String payload = http.getString();
      Serial.println("Bedroom update: " + payload);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpCode);
    }
    http.end();
  }
}

// ----- Hàm lấy trạng thái LED từ server -----
void updateBedroomLEDFromServer() {
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;
    String url = "http://" + serverIP + ":" + serverPort + "/get_led?room=bedroom";
    http.begin(client, url);
    int httpCode = http.GET();
    if(httpCode > 0) {
      String payload = http.getString();
      // Giả sử server trả về JSON: {"led": "ON"} hoặc {"led": "OFF"}
      if(payload.indexOf("ON") != -1) {
        ledState = true;
        digitalWrite(LEDPIN, HIGH);
      } else {
        ledState = false;
        digitalWrite(LEDPIN, LOW);
      }
    }
    http.end();
  }
}

// ----- Hàm lấy trạng thái Quạt từ server -----
void updateBedroomFanFromServer() {
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;
    String url = "http://" + serverIP + ":" + serverPort + "/get_fan?room=bedroom";
    http.begin(client, url);
    int httpCode = http.GET();
    if(httpCode > 0) {
      String payload = http.getString();
      if(payload.indexOf("ON") != -1) {
        fanState = true;
        digitalWrite(FANPIN, HIGH);
      } else {
        fanState = false;
        digitalWrite(FANPIN, LOW);
      }
    }
    http.end();
  }
}

// ----- Hàm lấy trạng thái cửa sổ từ server -----
void updateBedroomWindowFromServer() {
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;
    String url = "http://" + serverIP + ":" + serverPort + "/get_window?room=bedroom";
    http.begin(client, url);
    int httpCode = http.GET();
    if(httpCode > 0) {
      String payload = http.getString();
      if(payload.indexOf("OPEN") != -1) {
        windowState = "OPEN";
      } else {
        windowState = "CLOSED";
      }
    }
    http.end();
  }
}

// ----- Hàm điều khiển cửa sổ bằng servo -----
// Nếu cảm biến mưa phát hiện mưa, cửa sổ tự động đóng (0°). Ngược lại, theo trạng thái từ server: OPEN -> 90°, CLOSED -> 0°.
void updateBedroomWindow() {
  // Kiểm tra cảm biến mưa: nếu LOW => mưa
  bool isRaining = (digitalRead(RAINPIN) == LOW);
  if(isRaining) {
    // Nếu mưa, ép cửa sổ luôn đóng
    windowServo.write(0);
  } else {
    // Nếu không mưa, điều khiển theo trạng thái lấy từ server (windowState)
    if(windowState == "OPEN") {
      windowServo.write(180); // Mở cửa
    } else {
      windowServo.write(0);   // Đóng cửa
    }
  }
}

void setup() {
  Serial.begin(115200);
  setupWiFi();
  dht.begin();
  windowServo.attach(SERVO_PIN);
  
  // Cấu hình chân
  pinMode(LEDPIN, OUTPUT);
  pinMode(FANPIN, OUTPUT);
  pinMode(RAINPIN, INPUT);
  
  // Khởi tạo trạng thái ban đầu
  digitalWrite(LEDPIN, LOW);
  digitalWrite(FANPIN, LOW);
  windowServo.write(0);
  windowState = "CLOSED";
}

void loop() {
  // Đọc cảm biến
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  bool isRaining = (digitalRead(RAINPIN) == LOW);
  
  Serial.print("Temp: "); Serial.print(temperature);
  Serial.print(" C, Hum: "); Serial.print(humidity);
  Serial.print(" %, Rain: "); Serial.println(isRaining ? "RAIN" : "DRY");
  
  // Gửi dữ liệu cảm biến lên server cho phòng "bedroom"
  sendBedroomData(temperature, humidity, isRaining);
  
  // Lấy trạng thái điều khiển từ server
  updateBedroomLEDFromServer();
  updateBedroomFanFromServer();
  updateBedroomWindowFromServer();
  
  // Cập nhật cửa sổ dựa theo trạng thái và tự động đóng nếu mưa
  updateBedroomWindow();
  
  delay(5000);  // Poll mỗi 5 giây
}
