<h1 align="center">🏠 HỆ THỐNG NHÀ THÔNG MINH TÍCH HỢP AI & IOT 🤖</h1>

<div align="center">
  
  <p align="center">
    <img src="docs/images/logo.png" alt="Dai Nam Logo" width="200"/>
    <img src="docs/images/AIoTLab_logo.png" alt="AIoTLab Logo" width="200"/>
  </p>

  [![Made by AIoTLab](https://img.shields.io/badge/Made%20by%20AIoTLab-blue?style=for-the-badge)](https://www.facebook.com/DNUAIoTLab)
  [![Fit DNU](https://img.shields.io/badge/Fit%20DNU-green?style=for-the-badge)](https://fitdnu.net/)
  [![DaiNam University](https://img.shields.io/badge/DaiNam%20University-red?style=for-the-badge)](https://dainam.edu.vn)

</div>

<h2 align="center">💡 Giải pháp giám sát & điều khiển thông minh trong ngôi nhà hiện đại</h2>

<p align="left">
  Hệ thống Nhà Thông Minh tích hợp công nghệ AI và IoT, cho phép giám sát môi trường và điều khiển các thiết bị trong nhà một cách tự động. Dự án kết hợp phần cứng (ESP32/ESP8266, cảm biến, RFID, LCD, servo, LED, buzzer, v.v...) với phần mềm (Flask Server, giao diện Web, YOLO cho phát hiện người) để tạo ra một hệ thống điều khiển và giám sát toàn diện. 🔌🌐
</p>

---

## 🌟 GIỚI THIỆU 🚀

- **Giám sát đa phòng:** Hệ thống thu thập dữ liệu từ các phòng (Bếp 🍳, Khách 🛋️, Ngủ 🛏️, Cửa Ra Vào 🚪) với các cảm biến nhiệt độ, độ ẩm, khí gas, chuyển động, mưa, và sử dụng RFID để quản lý truy cập.
- **Điều khiển từ xa:** Trạng thái thiết bị được đồng bộ qua WiFi về Server Flask, cho phép điều khiển từ giao diện Web. 📡💻
- **Phát hiện người:** Một hệ thống riêng biệt sử dụng ESP32-CAM, YOLO và ESP8266 để phát hiện người trong không gian, từ đó kích hoạt LED cảnh báo. 👤🔍
- **Giao diện trực quan:** Giao diện web hiển thị dữ liệu cảm biến cập nhật theo thời gian thực và cho phép người dùng điều khiển thiết bị từ xa. 🖥️🎛️

---

## 🏗️ HỆ THỐNG 🔧

<p align="center">
  <img src="docs/images/image.png" alt="Kiến trúc hệ thống" width="800"/>
</p>

- **Lớp Cảm Biến & Thiết Bị (Edge Devices):** Các module ở từng phòng sử dụng ESP32/ESP8266 để thu thập dữ liệu từ cảm biến và điều khiển thiết bị (LED, quạt, servo, buzzer…). ⚙️📡
- **Lớp Giao Tiếp & Xử Lý Dữ Liệu:** Dữ liệu được gửi qua WiFi về Server Flask thông qua các API RESTful. 🌐🔗
- **Lớp Server & Giao Diện Web:** Flask Server xử lý dữ liệu, lưu trữ trạng thái và cung cấp giao diện điều khiển qua Web. Hệ thống phát hiện người cũng có server riêng với video stream và trạng thái cảnh báo. 💻🌟

---

## 📂 CẤU TRÚC DỰ ÁN 🗂️

```
Project
├── docs/
│   ├── images/                 # Hình ảnh minh họa, logo, sơ đồ hệ thống, poster 📸
│   │   ├── logo.png
│   │   ├── AIoTLab_logo.png
│   │   ├── POSTER_XIN_XO_CON_BO_pages-to-jpg-0001.jpg
│   │   └── image.png
├── Main_House/
│   ├── Phong_bep_uno/                # Code cho Phòng Bếp 🍳
│   │   └── Phong.bepp.ino
│   ├── Phong_khach_uno/            # Code cho Phòng Khách 🛋️
│   │   └── Phong_khachhh.ino
│   ├── Phong_ngu_uno/                # Code cho Phòng Ngủ 🛏️
│   │   └── PHONG_NGU.ino
│   └── Cua_ra_vao_uno/                   # Code cho Cửa Ra Vào 🚪
│       └── _9Control_Led_Icd_Pasword.ino
├── Server_main_house/
│   ├── app.py                  # Flask Server của Nhà Thông Minh
│   └── templates/
│       └── index.html         # Giao diện Web điều khiển hệ thống
├── Cam_nhan_dien_nguoi_bat_den/
│   ├── CameraWebServer/              # Code cho ESP32-CAM (thu hình) 📷
│   │   └── CameraWebServer.ino
│   ├── server_people/          # Flask Server với YOLO, video stream, cảnh báo 🚨
│   │   ├── cam.py
│   │   └── templates/
│   │       └── index.html         # Giao diện Web cho hệ thống phát hiện người
│   ├── ESP8266-BTL-CAM/           # Code cho ESP8266 bật LED theo trạng thái cảnh báo 💡
│   │   └── ESP8266-BTL-CAM.ino
└── README.md                   # Tệp hướng dẫn dự án (bạn đang xem) 📖
```

---

## 🛠️ CÔNG NGHỆ SỬ DỤNG 💻

<div align="center">

### 📡 Phần cứng
[![ESP32](https://img.shields.io/badge/ESP32-4MB%20Flash-blue?style=for-the-badge&logo=espressif)](https://www.espressif.com/)
[![ESP8266](https://img.shields.io/badge/ESP8266-80MHz-orange?style=for-the-badge&logo=espressif)](https://www.espressif.com/)
[![Arduino](https://img.shields.io/badge/Arduino-IDE-00979D?style=for-the-badge&logo=arduino)](https://www.arduino.cc/)
[![RFID](https://img.shields.io/badge/RFID-MFRC522-green?style=for-the-badge)](https://github.com/miguelbalboa/rfid)
[![Camera](https://img.shields.io/badge/ESP32--CAM-Yes-green?style=for-the-badge)](https://www.ai-thinker.com/)

### 🖥️ Phần mềm
[![Python](https://img.shields.io/badge/Python-3.x-blue?style=for-the-badge&logo=python)]()
[![Flask](https://img.shields.io/badge/Flask-Framework-black?style=for-the-badge&logo=flask)]()
[![YOLO](https://img.shields.io/badge/YOLO-Model-red?style=for-the-badge)]()
[![OpenCV](https://img.shields.io/badge/OpenCV-Computer%20Vision-blue?style=for-the-badge)]()
</div>

---

## 🛠️ YÊU CẦU HỆ THỐNG 🔌

### 🔌 Phần cứng
- **ESP32/ESP8266:** Dùng để lập trình các module cảm biến. 🤖
- **Cảm biến:**  
  - DHT11 (nhiệt độ, độ ẩm) 🌡️
  - Cảm biến khí gas (Phòng Bếp) ⛽
  - HC-SR04 (Phòng Khách) 📡
  - Cảm biến mưa (Phòng Ngủ) ☔
  - RFID RC522 và bàn phím ma trận (Cửa Ra Vào) 🛂
  - ESP32-CAM (cho phát hiện người) 📷
- **Thiết bị điều khiển:**  
  - LCD I2C 🖥️
  - LED, buzzer, quạt, servo 🔄

### 💻 Phần mềm
- **Python 3**
- **Flask Framework** 🍶
- **Thư viện YOLO (ultralytics)** và OpenCV cho hệ thống phát hiện người 👁️

---

## 🚀 HƯỚNG DẪN CÀI ĐẶT & CHẠY ⚙️

### 1. Nạp chương trình cho các module
- **Sử dụng Arduino IDE:**  
  Mở từng file mã nguồn trong thư mục `Main_House/` (`Phong_bep_uno/Phong.bepp.ino`, `Phong_khach_uno/Phong_khachhh.ino`, `Phong_ngu_uno/PHONG_NGU.ino`, `Cua_ra_vao_uno/_9Control_Led_Icd_Pasword.ino`) và nạp lên board tương ứng.
- **ESP32-CAM:**  
  Mở file `CameraWebServer/CameraWebServer.ino` và nạp lên board ESP32-CAM.
- **ESP8266 (Phát Hiện Người – LED):**  
  Nạp file `ESP8266-BTL-CAM/ESP8266-BTL-CAM.ino` lên board ESP8266.

### 2. Cài đặt Python và các thư viện cần thiết
Cài đặt Python 3 và sau đó cài đặt các thư viện bằng lệnh:
```bash
pip install flask pyserial pymongo opencv-python ultralytics numpy
```

### 3. Cấu hình Server
- **Flask Server (Nhà Thông Minh):**  
  Chạy file `Server_main_house/app.py`:
  ```bash
  python app.py
  ```
- **Flask Server (Phát Hiện Người):**  
  Chạy file `Server_cam/cam.py`:
  ```bash
  python cam.py
  ```

### 4. Chạy giao diện quản lý (nếu có)
- Mở trình duyệt và truy cập vào `http://localhost:5000` để xem giao diện của Nhà Thông Minh.
- Với hệ thống phát hiện người, truy cập vào URL tương ứng (ví dụ: `http://localhost:5000`) để xem video stream và cảnh báo.

---

## 📖 HƯỚNG DẪN SỬ DỤNG 👨‍💻

1. **Giám sát & điều khiển nhà thông minh:**
   - Các module tự động gửi dữ liệu cảm biến lên Flask Server.
   - Giao diện Web hiển thị thông tin từng phòng và cho phép điều khiển từ xa (bật/tắt LED, quạt, cửa sổ…). 
2. **Phát hiện người:**
   - ESP32-CAM thu hình và gửi dữ liệu về Flask Server chạy YOLO để phân tích.
   - Khi phát hiện người, trạng thái cảnh báo được cập nhật và ESP8266 sẽ bật LED.
   - Giao diện Web của hệ thống phát hiện người hiển thị video stream và thông báo cảnh báo.

---

## ⚙️ CẤU HÌNH & GHI CHÚ 🔧

1. **Cổng kết nối Arduino:**  
   Mặc định cổng COM được cấu hình trong file Python (có thể điều chỉnh nếu cần).
2. **Cấu hình WiFi:**  
   Cập nhật SSID, mật khẩu trong các file mã nguồn (Arduino & ESP32-CAM, ESP8266).
3. **Endpoint API:**  
   Đảm bảo các endpoint trên Flask Server được cập nhật chính xác với địa chỉ IP và cổng của máy chủ.
4. **Thời gian cập nhật:**  
   Các module gửi dữ liệu định kỳ, giao diện Web tự động refresh sau 5 giây.

---

## 📰 POSTER 🖼️

<p align="center">
  <img src="docs/images/POSTER_XIN_XO_CON_BO_pages-to-jpg-0001.jpg" alt="Poster dự án" width="800"/>
</p>

---

## 🤝 ĐÓNG GÓP 👥

Dự án được phát triển bởi:
| Họ và Tên        | Vai trò                                                    |
|------------------|------------------------------------------------------------|
| Vũ Văn Hiệp      | Phát triển mã nguồn, thiết kế hệ thống, thiết kế giao diện, thuyết trình |
| Trần Thị Thu Lan | Đề xuất cải tiến, PowerPoint, thiết kế mô hình, hỗ trợ bài tập lớn   |
| Xa Đức Đồng      | Thiết kế slide PowerPoint, thiết kế mô hình, đề xuất cải tiến, hỗ trợ bài tập lớn      |
| Trần Hồng Quân   | Phát triển mã nguồn, thiết kế Poster, hỗ trợ bài tập lớn    |

© 2025 NHÓM 3, CNTT16-02, TRƯỜNG ĐẠI HỌC ĐẠI NAM.  
