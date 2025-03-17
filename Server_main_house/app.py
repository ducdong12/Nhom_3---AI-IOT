from flask import Flask, request, render_template, jsonify

app = Flask(__name__)

# Lưu trữ dữ liệu cảm biến cho từng phòng
sensor_data = {
    "kitchen": {
        "temp": "N/A",
        "hum": "N/A",
        "gas": "N/A",
        "alarm": "SAFE",
        "led": "OFF"
    },
    "living_room": {
        "temp": "N/A",
        "hum": "N/A",
        "distance": "N/A",
        "motion": "NO",
        "led": "OFF",
        "fan": "OFF",
        "sensor": "YES"  # YES: cảm biến bật, NO: cảm biến tắt
    },
    "bedroom": {
        "temp": "N/A",
        "hum": "N/A",
        "rain": "DRY",    # "RAIN" nếu mưa, "DRY" nếu không mưa
        "led": "OFF",
        "fan": "OFF",
        "window": "CLOSED"  # "OPEN" hoặc "CLOSED"
    }
}

# Endpoint nhận dữ liệu cập nhật từ ESP32
@app.route('/update')
def update():
    room = request.args.get('room', 'kitchen')
    if room == "kitchen":
        temp = request.args.get('temp')
        hum = request.args.get('hum')
        gas = request.args.get('gas')
        alarm = request.args.get('alarm')
        # Không yêu cầu tham số 'led' ở đây
        if temp and hum and gas and alarm:
            sensor_data["kitchen"].update({
                "temp": temp,
                "hum": hum,
                "gas": gas,
                "alarm": alarm
            })
            return "Kitchen Data Updated", 200
        return "Missing parameters", 400
    elif room == "living_room":
        temp = request.args.get('temp')
        hum = request.args.get('hum')
        distance = request.args.get('distance')
        motion = request.args.get('motion')
        if temp and hum and distance and motion:
            # Giữ nguyên trạng thái LED, Quạt, Cảm biến hiện tại không bị ghi đè
            led = sensor_data["living_room"].get("led", "OFF")
            fan = sensor_data["living_room"].get("fan", "OFF")
            sensor = sensor_data["living_room"].get("sensor", "YES")
            sensor_data["living_room"].update({
                "temp": temp,
                "hum": hum,
                "distance": distance,
                "motion": motion,
                "led": led,
                "fan": fan,
                "sensor": sensor
            })
            return "Living Room Data Updated", 200
        return "Missing parameters", 400
    elif room == "bedroom":
        temp = request.args.get('temp')
        hum = request.args.get('hum')
        rain = request.args.get('rain')
        if temp and hum and rain:
            # Giữ nguyên trạng thái điều khiển hiện tại
            led = sensor_data["bedroom"].get("led", "OFF")
            fan = sensor_data["bedroom"].get("fan", "OFF")
            window = sensor_data["bedroom"].get("window", "CLOSED")
            sensor_data["bedroom"].update({
                "temp": temp,
                "hum": hum,
                "rain": rain,
                "led": led,
                "fan": fan,
                "window": window
            })
            return "Bedroom Data Updated", 200
        return "Missing parameters", 400

# Endpoint cập nhật trạng thái LED (cho cả 3 phòng)
@app.route('/set_led')
def set_led():
    room = request.args.get('room', 'kitchen')
    state = request.args.get('state')
    if state:
        if room in sensor_data and "led" in sensor_data[room]:
            sensor_data[room]["led"] = "ON" if state.lower() == "on" else "OFF"
            return "LED state updated", 200
        return "Invalid room", 400
    return "Missing parameter", 400

# Endpoint cập nhật trạng thái Quạt (cho phòng khách & phòng ngủ)
@app.route('/set_fan')
def set_fan():
    room = request.args.get('room', 'living_room')
    state = request.args.get('state')
    if state:
        if room in sensor_data and "fan" in sensor_data[room]:
            sensor_data[room]["fan"] = "ON" if state.lower() == "on" else "OFF"
            return "Fan state updated", 200
        return "Invalid room", 400
    return "Missing parameter", 400

# Endpoint cập nhật trạng thái Cửa sổ (chỉ cho phòng ngủ)
@app.route('/set_window')
def set_window():
    room = request.args.get('room', 'bedroom')
    state = request.args.get('state')
    if state and room == "bedroom":
        sensor_data["bedroom"]["window"] = "OPEN" if state.lower() == "open" else "CLOSED"
        return "Window state updated", 200
    return "Missing parameter", 400

# Endpoint cập nhật trạng thái Cảm biến (cho phòng khách)
@app.route('/set_sensor')
def set_sensor():
    room = request.args.get('room', 'living_room')
    state = request.args.get('state')
    if state:
        if room in sensor_data and "sensor" in sensor_data[room]:
            sensor_data[room]["sensor"] = "YES" if state.lower() == "on" else "NO"
            return "Sensor state updated", 200
        return "Invalid room", 400
    return "Missing parameter", 400

# Endpoint trả về trạng thái LED (cho từng phòng)
@app.route('/get_led')
def get_led():
    room = request.args.get('room', 'kitchen')
    if room in sensor_data and "led" in sensor_data[room]:
        return jsonify({"led": sensor_data[room]["led"]})
    return jsonify({"led": "N/A"})

# Endpoint trả về trạng thái Quạt cho phòng khách và phòng ngủ
@app.route('/get_fan')
def get_fan():
    room = request.args.get('room', 'living_room')
    if room in sensor_data and "fan" in sensor_data[room]:
        return jsonify({"fan": sensor_data[room]["fan"]})
    return jsonify({"fan": "N/A"})

# Endpoint trả về trạng thái Cửa sổ cho phòng ngủ
@app.route('/get_window')
def get_window():
    room = request.args.get('room', 'bedroom')
    if room in sensor_data and "window" in sensor_data[room]:
        return jsonify({"window": sensor_data[room]["window"]})
    return jsonify({"window": "N/A"})

# Endpoint trả về trạng thái Cảm biến cho phòng khách
@app.route('/get_sensor')
def get_sensor():
    room = request.args.get('room', 'living_room')
    if room in sensor_data and "sensor" in sensor_data[room]:
        return jsonify({"sensor": sensor_data[room]["sensor"]})
    return jsonify({"sensor": "N/A"})

# Trang chính hiển thị dữ liệu cảm biến cho cả 3 phòng
@app.route('/')
def index():
    return render_template('index.html', sensor_data=sensor_data)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
