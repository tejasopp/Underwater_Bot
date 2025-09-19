from flask import Flask, render_template, redirect, url_for
import serial
import time

# Serial Port Settings
stm32_port = "/dev/stm32"  # Change if needed
baud_rate = 115200

try:
    ser = serial.Serial(stm32_port, baud_rate, timeout=1)
    time.sleep(2)  # Allow STM32 reset time
except serial.SerialException as e:
    print(f"Error opening serial port: {e}")
    ser = None

app = Flask(__name__)

# Commands mapping
COMMANDS = {
    "forward": "forward",
    "backward": "backward",
    "left": "left",
    "right": "right",
    "stop": "stop",
    "lon": "lon",
    "loff": "loff",
    "up": "up",
    "down": "down"
}

def send_command(cmd):
    """Send command to STM32 via USB CDC."""
    if ser and ser.is_open:
        ser.write((cmd + "\n").encode('utf-8'))
        ser.flush()
        print(f"Sent command: {cmd}")
    else:
        print("Serial port not available.")

@app.route("/")
def index():
    return render_template("index.html")

@app.route("/control/<action>")
def control(action):
    if action in COMMANDS:
        send_command(COMMANDS[action])
    return redirect(url_for("index"))

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)
