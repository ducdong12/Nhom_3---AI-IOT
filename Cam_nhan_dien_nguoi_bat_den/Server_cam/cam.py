from ultralytics import YOLO
import cv2
from urllib.request import urlopen
import numpy as np
from flask import Flask, Response, render_template, jsonify

app = Flask(__name__)
model = YOLO("yolo11n.pt")
url = r'http://192.168.82.42/capture'

# 🟢 Định nghĩa biến toàn cục alert_status
alert_status = False  

def generate_frames():
    global alert_status  # Khai báo alert_status là biến toàn cục
    while True:
        try:
            img_resp = urlopen(url)
            imgnp = np.asarray(bytearray(img_resp.read()), dtype="uint8")
            frame = cv2.imdecode(imgnp, -1)
            
            results = model(frame)
            alert_status = False  # Mặc định không có người
            
            for result in results:
                for box in result.boxes:
                    x1, y1, x2, y2 = map(int, box.xyxy[0])
                    confidence = box.conf[0].item()
                    class_id = int(box.cls[0])

                    if class_id == 0:  # Nếu phát hiện người
                        alert_status = True
                        cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 0), 2)
                        cv2.putText(frame, f'Person {confidence:.2f}', (x1, y1 - 10),
                                    cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)
            
            _, buffer = cv2.imencode('.jpg', frame)
            frame_bytes = buffer.tobytes()
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + frame_bytes + b'\r\n')
        except Exception as e:
            print(f"Error: {e}")
            continue


@app.route('/')
def index():
    return render_template("index.html")

@app.route('/video_feed')
def video_feed():
    return Response(generate_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

# 🟢 API gửi trạng thái cảnh báo
@app.route('/alert_status')
def alert_status_route():
    global alert_status  # Khai báo biến toàn cục
    return jsonify({"alert": alert_status})

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=5000, debug=True)
