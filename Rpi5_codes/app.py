from flask import Flask, render_template, redirect, url_for, request, jsonify, Response
import serial
import time
import cv2
from picamera2 import Picamera2
from ultralytics import YOLO

app = Flask(__name__)

# ------------------------------------------------------------
#                  SERIAL / STM32 ROBOT CONTROL
# ------------------------------------------------------------
stm32_port = "/dev/stm32"
baud_rate = 115200

try:
    ser = serial.Serial(stm32_port, baud_rate, timeout=1)
    time.sleep(2)
except serial.SerialException as e:
    print(f"Error opening serial port: {e}")
    ser = None

COMMANDS = {
    "forward": "forward",
    "backward": "backward",
    "left": "left",
    "right": "right",
    "stop": "stop",
    "lon": "lon",
    "loff": "loff",
    "up": "up",
    "down": "down",
    "start": "start",
    "dirstop": "dirstop"
}

last_speed = 0

def send_command(cmd):
    if ser and ser.is_open:
        ser.write((cmd + "\n").encode())
        ser.flush()
        print(f"Sent command: {cmd}")
    else:
        print("Serial not available")


# ------------------------------------------------------------
#                           CAMERA + YOLO
# ------------------------------------------------------------
picam2 = Picamera2()
picam2.preview_configuration.main.size = (640, 640)
picam2.preview_configuration.main.format = "RGB888"
picam2.preview_configuration.align()
picam2.configure("preview")
picam2.start()

model = YOLO("yolov8n_ncnn_model")

def generate_frames():
    while True:
        frame = picam2.capture_array()

        results = model(frame, classes=[0])  # Only detect person

        annotated = results[0].plot()

        # FPS overlay
        inference_time = results[0].speed['inference']
        fps = 1000 / inference_time
        text = f"FPS: {fps:.1f}"

        cv2.putText(annotated, text, (annotated.shape[1]-200, 40),
                    cv2.FONT_HERSHEY_SIMPLEX, 1, (255,255,255), 2)

        _, buffer = cv2.imencode(".jpg", annotated)
        frame_bytes = buffer.tobytes()

        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame_bytes + b'\r\n')


# ------------------------------------------------------------
#                      FLASK ROUTES (Merged)
# ------------------------------------------------------------

@app.route("/")
def index():
    return render_template("index.html", speed=last_speed)

@app.route("/control/<action>")
def control(action):
    if action in COMMANDS:
        send_command(COMMANDS[action])
    return redirect(url_for("index"))

@app.route("/speed", methods=["POST"])
def speed_set():
    global last_speed
    speed = request.json.get("speed")
    if speed is None:
        return jsonify({"status": "error"}), 400

    last_speed = int(speed)
    send_command(f"speed:{last_speed}")
    return jsonify({"status": "ok", "speed": last_speed})


# ------------------- CAMERA STREAM ROUTE -------------------
@app.route("/video_feed")
def video_feed():
    return Response(generate_frames(),
                    mimetype="multipart/x-mixed-replace; boundary=frame")


@app.route("/stream")
def stream_page():
    return "<h1>Camera Stream</h1><img src='/video_feed'>"


# ------------------------------------------------------------
#                        RUN SERVER
# ------------------------------------------------------------
if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=False, threaded=True)
