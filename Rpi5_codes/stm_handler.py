# server.py
import os
import glob
import time
from threading import Lock
from flask import Flask, render_template, jsonify
from flask_socketio import SocketIO, emit
import serial

# ===== Flask + SocketIO setup =====
app = Flask(__name__)
socketio = SocketIO(app, cors_allowed_origins="*", async_mode="eventlet")

# ===== Serial config =====
SYMLINK = '/dev/stm32'   # udev symlink if you set it
BAUD = 115200

ser = None
ser_lock = Lock()
limit_status = "NONE"  # "NONE", "UP", "DOWN"

def find_serial_port():
    """Return a serial port path (prefer symlink if present)."""
    if os.path.exists(SYMLINK):
        return SYMLINK
    # fallback: common device patterns
    for pattern in ('/dev/ttyACM*', '/dev/ttyUSB*', '/dev/serial*'):
        lst = glob.glob(pattern)
        if lst:
            return sorted(lst)[0]
    return None

def init_serial():
    """Try to open a serial port; returns True if opened."""
    global ser
    port = find_serial_port()
    if not port:
        print("[serial] No candidate port found, retrying...")
        return False
    try:
        ser = serial.Serial(port, BAUD, timeout=0.1)
        print(f"[serial] Opened {port} @ {BAUD}")
        return True
    except Exception as e:
        print(f"[serial] Failed to open {port}: {e}")
        ser = None
        return False

# ===== Serial reader background task =====
def serial_reader():
    global ser, limit_status
    while True:
        if not ser or not getattr(ser, 'is_open', False):
            init_serial()
            time.sleep(1.0)
            continue

        try:
            if ser.in_waiting:
                raw = ser.readline()
                try:
                    line = raw.decode(errors='ignore').strip()
                except Exception:
                    line = str(raw)
                if line:
                    print("[STM32]", line)
                    if "LIMIT:UP" in line:
                        limit_status = "UP"
                        socketio.emit('limit_update', {'status': 'UP'}, broadcast=True)
                    elif "LIMIT:DOWN" in line:
                        limit_status = "DOWN"
                        socketio.emit('limit_update', {'status': 'DOWN'}, broadcast=True)
                    elif "LIMIT:CLEAR" in line or "LIMIT:NONE" in line:
                        limit_status = "NONE"
                        socketio.emit('limit_update', {'status': 'NONE'}, broadcast=True)
            else:
                time.sleep(0.02)
        except Exception as e:
            print("[serial] Read error:", e)
            try:
                ser.close()
            except Exception:
                pass
            ser = None
            time.sleep(1.0)

# ===== WebSocket connect handler =====
@socketio.on('connect')
def handle_connect():
    emit('limit_update', {'status': limit_status})

# ===== HTTP routes =====
@app.route('/')
def index():
    return render_template('index.html')

@app.route('/command/<action>')
def command(action):
    with ser_lock:
        if ser and getattr(ser, 'is_open', False):
            try:
                ser.write((action.strip() + '\n').encode())
            except Exception as e:
                return f"Serial write error: {e}", 500
        else:
            return "Serial not available", 500
    return f"Sent: {action}"

@app.route('/limit_status')
def get_limit_status():
    return jsonify({"status": limit_status})

# ===== Main entry =====
if __name__ == '__main__':
    print("🚀 Flask-SocketIO Limit Switch Server")
    print("📡 Listening on: http://0.0.0.0:5000/")
    print("💡 Waiting for STM32 connection... (will retry automatically)")

    socketio.start_background_task(serial_reader)
    socketio.run(app, host='0.0.0.0', port=5000)
