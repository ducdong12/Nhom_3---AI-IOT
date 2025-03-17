#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "kevin kaslana";
const char* password = "12345678";
const char* serverUrl = "http://192.168.82.9:5000/alert_status"; // Địa chỉ Flask server

const int LED_PIN = D2; // Chân LED
bool ledState = false;
unsigned long lastDetected = 0;
const unsigned long ledTimeout = 5000; // 5 giây

WiFiClient wifiClient;  // Thêm đối tượng WiFiClient

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi!");
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(wifiClient, serverUrl); // Sử dụng WiFiClient để sửa lỗi
        int httpCode = http.GET();

        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            Serial.println("Server Response: " + payload);

            if (payload.indexOf("\"alert\": true") > 0) {
                digitalWrite(LED_PIN, HIGH);
                ledState = true;
                lastDetected = millis();
            } 
        }
        http.end();
    }

    // Tự động tắt LED sau 5 giây nếu không có cảnh báo mới
    if (ledState && millis() - lastDetected > ledTimeout) {
        digitalWrite(LED_PIN, LOW);
        ledState = false;
    }

    delay(1000);
}
