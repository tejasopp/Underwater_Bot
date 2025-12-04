# RPi Setup guide
## Clone the repository and start the virtual environment  
```bash
sudo apt update
sudo apt upgrade -y
sudo apt install python3-pip python3-opencv -y  
git clone https://github.com/tejasopp/Underwater_Bot.git
cd Underwater_Bot/Rpi5_codes
python3 -m venv myenv
source venv/bin/activate   
```
## Install Dependencies  
```bash
pip3 install flask ultralytics
```
## Running the Application
Inside the project folder run:  
```bash
python3 app.py
```  
Now you will find the app is running.  
Next open this on another laptop connected via **Ethernet**: http://raspberrypi.local:5000  
# Run a Python Script at Boot on Raspberry Pi (Using Virtual Environment)

This guide shows how to automatically run a Python script at startup on a Raspberry Pi using a specific Python virtual environment. The method uses `systemd` for reliability.

---

## Setup Instructions

### Create a systemd service  
```bash
sudo nano /etc/systemd/system/myscript.service
```
Paste the following :  
```
[Unit]
Description=Run Python script in virtual environment at startup
After=network.target

[Service]
Type=simple
User=your username
WorkingDirectory=path to working directory
ExecStart=path to python script
Restart=always

[Install]
WantedBy=multi-user.target
```
Make sure to update the paths and username according to your setup.  

### Then run the following:  
```bash
sudo systemctl daemon-reload
sudo systemctl enable myscript.service
sudo systemctl start myscript.service
```
### Check the logs using:  
```bash
sudo journalctl -u myscript.service -f
```  
