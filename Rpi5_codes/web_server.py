from flask import Flask, render_template, request

app = Flask(__name__)

# =================================================================================
# Placeholder Motor Control Functions
# You will replace these functions with your actual RPi.GPIO code.
# For now, they just print a message to the console.
# =================================================================================

def motor_forward():
    # Placeholder for motor control logic to move forward.
    print("Command received: Move Forward")
    # Example: You would add your GPIO code here, e.g.:
    # GPIO.output(Motor1A, GPIO.HIGH)
    # GPIO.output(Motor1B, GPIO.LOW)

def motor_backward():
    # Placeholder for motor control logic to move backward.
    print("Command received: Move Backward")

def motor_left():
    # Placeholder for motor control logic to turn left.
    print("Command received: Turn Left")

def motor_right():
    # Placeholder for motor control logic to turn right.
    print("Command received: Turn Right")

def motor_up():
    # Placeholder for motor control logic to move up.
    print("Command received: Move Up")

def motor_down():
    # Placeholder for motor control logic to move down.
    print("Command received: Move Down")

def motor_stop():
    # Placeholder for motor control logic to stop all motors.
    print("Command received: Stop")
    # Example: You would add your GPIO code here, e.g.:
    # GPIO.output(Motor1A, GPIO.LOW)
    # GPIO.output(Motor1B, GPIO.LOW)


# =================================================================================
# Flask Web Server Routes
# These routes link a URL to a Python function. When a command is sent from the
# browser, Flask executes the corresponding function.
# =================================================================================

# The home page route. Renders the index.html file.
@app.route('/')
def index():
    return render_template('index.html')

# This route receives commands from the web page.
# The '<action>' part of the URL is a variable that is passed to the function.
@app.route('/command/<action>')
def command(action):
    if action == 'forward':
        motor_forward()
    elif action == 'backward':
        motor_backward()
    elif action == 'left':
        motor_left()
    elif action == 'right':
        motor_right()
    elif action == 'up':
        motor_up()
    elif action == 'down':
        motor_down()
    elif action == 'stop':
        motor_stop()
    else:
        print(f"Unknown command: {action}")

    return f"Command '{action}' received succesfully" 

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
