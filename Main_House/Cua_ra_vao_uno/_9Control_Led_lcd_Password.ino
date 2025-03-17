#include <Keypad.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>      // Khai báo thư viện LCD sử dụng I2C
#include <SPI.h>                     // Thư viện SPI
#include <MFRC522.h>                 // Thư viện MFRC522

LiquidCrystal_I2C lcd(0x27, 16, 2); // 0x27 địa chỉ LCD, 16 cột và 2 hàng
Servo myservo;                       // Tạo biến myServo của loại Servo

// Khai báo các chân kết nối RC522
#define SS_PIN A3   // Slave Select (SS) / SDA
#define RST_PIN 9    // Reset pin

//Sử dụng các chân SPI cứng
#define MOSI_PIN 11
#define MISO_PIN 12
#define SCK_PIN 13

//Các chân đã thay đổi
#define SERVO_PIN A0   // Chân điều khiển servo
#define LED_PIN A2    // Chân điều khiển LED (tùy chọn)

MFRC522 mfrc522(SS_PIN, RST_PIN); // Tạo đối tượng MFRC522

// Các UID hợp lệ (lưu dưới dạng String)
String validUID1 = "03 A3 5F FC";
String validUID2 = "23 CA 7E F5";
String validUID3 = "C0 E4 48 12";
String validUID4 = "C3 0F 4B F7";

// Tạo ký tự đặc biệt &
byte va[] = {
  B00000,
  B01100,
  B10100,
  B10100,
  B01000,
  B10101,
  B10010,
  B01101
};
const byte ROWS = 4;
const byte COLS = 4;
char password[5];
char On_equip[] = "1111"; // Mật khẩu mở cửa
char Off_equip[] = "2222"; // Mật khẩu đóng cửa
int i = 0;
int on = 0;
int servo = 10;
int RedLed = 13;

char MatrixKey[ROWS][COLS] =
{
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {2, 3, 4, 5}; // R1,R2,R3,R4
byte colPins[COLS] = {6, 7, 8, A1}; // C1,C2,C3,C4

Keypad Mykeys = Keypad(makeKeymap(MatrixKey), rowPins, colPins, ROWS, COLS);

//=================================================================================================================

void setup() {
  Serial.begin(9600);
  pinMode(RedLed, OUTPUT);
  pinMode(LED_PIN, OUTPUT); // Cấu hình chân LED

  lcd.init();         // Khởi tạo màn hình Màn hình
  lcd.backlight();    // Bật đèn màn hình Màn hình
  lcd.createChar(0, va); // Tạo ký tự đặc biệt &

  myservo.attach(SERVO_PIN); //Gán chân servo
  myservo.write (0);

  
  lcd.clear();
  lcd.print(" Enter Password");

  // Khởi tạo SPI và RC522
  SPI.begin();        // Khởi tạo SPI bus
  mfrc522.PCD_Init(); // Khởi tạo MFRC522

  // Cấu hình các chân cho RC522 (Quan trọng khi dùng chân Analog làm Digital)
  pinMode(SS_PIN, OUTPUT); //A3
  pinMode(A1, OUTPUT);  // MOSI
  pinMode(A2, INPUT);   // MISO
   pinMode(7, OUTPUT);     //RST

  Serial.println("RC522 - Sẵn sàng");
}

//=================================================================================================================

void loop() {
  // Kiểm tra xem có thẻ mới không
  if (mfrc522.PICC_IsNewCardPresent()) {
    // Chọn một thẻ
    if (mfrc522.PICC_ReadCardSerial()) {
      Serial.print("UID tag :");
      String cardUID = "";
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        cardUID += (mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        cardUID += String(mfrc522.uid.uidByte[i], HEX);
      }
      Serial.println();
      Serial.print("Message : ");
      cardUID.toUpperCase();
      cardUID = cardUID.substring(1);  //Loại bỏ khoảng trắng thừa
      Serial.println(cardUID);

      // Kiểm tra UID với các UID hợp lệ
      if (cardUID == validUID1 || cardUID == validUID2 || cardUID == validUID3 || cardUID == validUID4) {
        Serial.println("Thẻ hợp lệ! Mở cửa...");
        lcd.clear();
        lcd.print("    Card OK!");
        myservo.write(90); // Mở cửa!
        digitalWrite(RedLed, LOW);
        digitalWrite(LED_PIN, HIGH); //Bật đèn Led
        delay(1000);
        digitalWrite(LED_PIN, LOW); //Tắt đèn led
        lcd.clear();
        lcd.print("     Opened!");
        delay(2000);
        myservo.write(0);  //Sau 1s thì đóng cửa lại
        lcd.clear();
        lcd.print(" Enter Password");
        digitalWrite(RedLed, HIGH);
      } else {
        Serial.println("Thẻ không hợp lệ!");
        lcd.clear();
        lcd.print("  Card Invalid!");
        delay(2000);
        lcd.clear();
        lcd.print(" Enter Password");
      }

      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();
    }
  }
   else {
        delay(50);  // Không tìm thấy thẻ, tạm dừng một chút
  }

  char EnterKey = Mykeys.getKey();

  if (EnterKey) {
    password[i] = EnterKey;
    i++;
    on++;
    Serial.println(password);
    // Nhập mật khẩu
    if (i == 0) {
      password[0] = EnterKey;
      lcd.setCursor(6, 1);
      lcd.print(password[0]);
      delay(500);
      lcd.setCursor(6, 1);
      lcd.print("*");
    }
    if (i == 1) {
      password[1] = EnterKey;
      lcd.setCursor(7, 1);
      lcd.print(password[1]);
      delay(500);
      lcd.setCursor(7, 1);
      lcd.print("*");
    }
    if (i == 2) {
      password[2] = EnterKey;
      lcd.setCursor(8, 1);
      lcd.print(password[2]);
      delay(500);
      lcd.setCursor(8, 1);
      lcd.print("*");
    }
    if (i == 3) {
      password[3] = EnterKey;
      lcd.setCursor(9, 1);
      lcd.print(password[3]);
      delay(500);
      lcd.setCursor(9, 1);
      lcd.print("*");
    }
  }
  if (on == 4) {
    if (!strcmp(password, On_equip)) {
      Serial.println("Mở cửa bằng mật khẩu");
      lcd.clear();
      lcd.print("    Correct!");
      myservo.write(90);
      digitalWrite(RedLed, LOW);
      digitalWrite(LED_PIN, HIGH);
      delay(1000);
      digitalWrite(LED_PIN, LOW);
      lcd.clear();
      lcd.print("     Opened");
      i = 0;
      Serial.println(" Dung mat khau mo cua!");
    }

    if (!strcmp(password, Off_equip)) {
       Serial.println("Đóng cửa bằng mật khẩu");
      lcd.clear();
      myservo.write(0);
      lcd.print("     Closed!");
      digitalWrite(RedLed, LOW);
      digitalWrite(LED_PIN, HIGH);
      delay(2000);
      digitalWrite(LED_PIN, LOW);
      lcd.clear();
      lcd.print(" Enter Password");
      i = 0;
    }

    if (strcmp(password, On_equip) && strcmp(password, Off_equip)) {
      Serial.println("Mật khẩu không chính xác");
        lcd.clear();
        lcd.print("   Incorrect!");
        delay(1000);
        lcd.clear();
        lcd.print("   Try Again!");
        delay(1000);
        lcd.clear();
        lcd.print(" Enter Password");
        i = 0;
        Serial.println(" Sai mat khau nhap lai.............");
        digitalWrite(RedLed, HIGH);
      
    }
    on = 0;
  }
}
