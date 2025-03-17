#include <WiFi.h>
#include <HTTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// --- Cấu hình WiFi ---
const char* ssid = "kevin kaslana";
const char* password = "12345678";

// Địa chỉ server Python (Flask)
String serverIP = "192.168.82.9"; 
String serverPort = "5000";
String updateEndpoint = "http://192.168.82.9:5000/update";

// --- Cấu hình cảm biến DHT11 ---
#define DHTPIN 15
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// --- Khởi tạo LCD ---
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- Khai báo chân kết nối (điều chỉnh theo ESP32 của bạn) ---
const int gasSensorPin = 34;    
const int buzzerPin    = 16;    
const int fanPin       = 17;    
const int ledPin       = 18;    
const int buttonPin    = 19;    

// *** MỚI ***: Khai báo thêm LED 2 để báo động
const int ledAlarmPin  = 23;    
bool ledAlarmState     = false; // Trạng thái LED 2 (báo động)

// --- Ngưỡng ---
const float tempThreshold = 30.0;
const int gasThreshold = 6000;    
const int gasHysteresis = 500;    

// Biến trạng thái
bool ledState = false;       // LED 1 (người dùng)
bool alarmState = false;     // Trạng thái báo động
bool alarmLockLED = false;   // Khóa LED 1 khi báo động

// Hàm đọc giá trị khí gas trung bình
int getGasValue() {
  int sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(gasSensorPin);
    delay(10);
  }
  return sum / 10;
}

// Gửi dữ liệu cảm biến + trạng thái báo động (cho phòng bếp)
void sendData(float temperature, float humidity, float gasPpm) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = updateEndpoint + "?room=kitchen&temp=" + String(temperature) +
                 "&hum=" + String(humidity) +
                 "&gas=" + String(gasPpm) +
                 "&alarm=" + String(alarmState ? "ALERT" : "SAFE");
    http.begin(url);
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Server response: " + response);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
}

// Cập nhật trạng thái LED 1 (người dùng) lên server
void updateLEDStateToServer(bool state) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String stateStr = state ? "on" : "off";
    String url = "http://" + serverIP + ":" + serverPort + "/set_led?room=kitchen&state=" + stateStr;
    http.begin(url);
    http.GET();
    http.end();
  }
}

// Lấy trạng thái LED 1 từ server để đồng bộ
void updateLEDFromServer() {
  // Bỏ qua nếu đang khóa LED 1
  if (alarmLockLED) return;

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://" + serverIP + ":" + serverPort + "/get_led?room=kitchen";
    http.begin(url);
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      String payload = http.getString();
      if (payload.indexOf("ON") != -1) {
        digitalWrite(ledPin, HIGH);
        ledState = true;
      } else {
        digitalWrite(ledPin, LOW);
        ledState = false;
      }
    }
    http.end();
  }
}

void setup() {
  pinMode(gasSensorPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(fanPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  // *** MỚI ***: Cấu hình chân cho LED 2
  pinMode(ledAlarmPin, OUTPUT);
  digitalWrite(ledAlarmPin, LOW);

  Serial.begin(115200);
  dht.begin();

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Starting");
  delay(2000);

  // Kết nối WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected!");
}

void loop() {
  // Đọc dữ liệu
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int gasValue = getGasValue();
  float ppmValue = map(gasValue, 0, 4095, 300, 10000);

  Serial.print("Temp: "); Serial.print(temperature);
  Serial.print(" C, Hum: "); Serial.print(humidity);
  Serial.print(" %, Gas: "); Serial.println(ppmValue);

  // Hiển thị trên LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C");
  lcd.setCursor(0, 1);
  lcd.print("Hum: ");
  lcd.print(humidity);
  lcd.print(" %");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Gas: ");
  lcd.print(ppmValue);

  // Kiểm tra điều kiện báo động
  if ((temperature > tempThreshold || ppmValue > gasThreshold) && !alarmState) {
    // Kích hoạt báo động
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(fanPin, HIGH);
    alarmState = true;

    // 1) Tắt LED 1 và khóa
    ledState = false;
    digitalWrite(ledPin, LOW);
    updateLEDStateToServer(false);
    alarmLockLED = true;

    // *** MỚI ***: 2) Bật LED 2 báo động
    ledAlarmState = true;
    digitalWrite(ledAlarmPin, HIGH);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ALERT!");
    lcd.setCursor(0, 1);
    lcd.print("DANGEROUS GAS!");
    delay(5000);
  }
  // Khi trở về an toàn
  else if ((temperature <= tempThreshold - 2 && ppmValue < gasThreshold - gasHysteresis) && alarmState) {
    // Tắt báo động
    digitalWrite(buzzerPin, LOW);
    digitalWrite(fanPin, LOW);
    alarmState = false;

    // Mở khóa LED 1
    alarmLockLED = false;

    // *** MỚI ***: Tắt LED 2 báo động
    ledAlarmState = false;
    digitalWrite(ledAlarmPin, LOW);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SAFE");
    delay(5000);
  }

  // Nút cứng để bật/tắt LED 1 (nếu không bị khóa)
  if (digitalRead(buttonPin) == LOW) {
    if (!alarmLockLED) {
      ledState = !ledState;
      digitalWrite(ledPin, ledState ? HIGH : LOW);
      updateLEDStateToServer(ledState);
    }
    delay(200);
  }

  // Gửi dữ liệu cảm biến + trạng thái báo động (phòng bếp)
  sendData(temperature, humidity, ppmValue);

  // Cập nhật LED 1 từ server (nếu không bị khóa)
  updateLEDFromServer();

  delay(1000);
}
