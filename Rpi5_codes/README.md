# Setup guide
## Update Raspberry Pi
```bash
sudo apt update
sudo apt upgrade -y  
```
## Install Dependencies  
```bash
sudo apt install python3-pip python3-opencv -y
pip3 install flask ultralytics
```
## Clone or Copy This Project  
```bash
git clone https://github.com/tejasopp/Underwater_Bot.git
cd Underwater_Bot/Rpi5_codes
```
## Running the Application
Inside the project folder run:  
```bash
python3 app.py
```  
Now you will find the app is running.  
Next open this on another laptop connected via **Ethernet**: http://raspberrypi.local:5000  
