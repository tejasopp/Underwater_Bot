# 🐠 Underwater Bot for Dead Body Detection

This project focuses on developing an **underwater robot** capable of **motion, depth control, illumination, and real-time image processing** for detecting dead bodies underwater. The system uses a **dual-controller architecture** — an **STM32F4 Blackpill** for low-level motor control and a **Raspberry Pi 5** for high-level computation and web-based operation.

---

## ⚙️ System Overview

The underwater bot integrates various hardware components for motion, sensing, and visibility:

| Component | Specification | Purpose |
|------------|----------------|----------|
| **BLDC Motor + 150A ESC** | 3670 Series | Forward/Backward motion |
| **Stepper Motor + A4988 Driver** | NEMA | Depth control via syringe mechanism |
| **STM32F4 Blackpill** | Cortex-M4 MCU | Motor & LED control via PWM |
| **Raspberry Pi 5** | Quad-core @ 2.4 GHz, 8 GB RAM | Web interface, ML integration |
| **Limit Switches** | Mechanical | Restrict syringe travel |
| **LEDs** | High-brightness | Illumination in turbid water |
| **Battery** | 12 V Li-ion/LiPo | Power supply |

---

## 🧠 Control Architecture

- **High-Level Control (Raspberry Pi 5)**  
  - Hosts a **Flask-based web GUI** for manual control (forward, backward, up, down, lights, speed).  
  - Handles **ML-based detection** and video streaming.  
  - Communicates with STM32 via **USB-CDC protocol**.  

- **Low-Level Control (STM32F4 Blackpill)**  
  - Generates PWM signals for **BLDC** and **stepper motor**.  
  - Parses serial commands from Raspberry Pi.  
  - Uses **limit switches** for safe depth control.

---

## 🔗 Communication Flow

1. Ground station (laptop) connects to RPi via **Ethernet tether**.  
2. User accesses the **web interface** to send control commands.  
3. RPi sends commands to STM32 via **USB-CDC serial link**.  
4. STM32 executes real-time motor and LED control.

---

## 🧰 Repository Structure

\`\`\`
📦 Underwater_Bot
├── RPi5_codes/         # Flask server, web GUI, and ML scripts
├── Stm32_src_codes/    # STM32 firmware (PWM, USB-CDC, motor control)
└── README.md
\`\`\`

---

## 💻 Software Details

- **Raspberry Pi 5:** Raspberry Pi OS (Debian Bookworm)  
- **Server Framework:** Python 3.11 + Flask  
- **MCU Firmware:** STM32CubeIDE (Bare-metal C)  
- **Communication Protocol:** USB-CDC (Custom Udev Rule)

---



🔗 *Upload working demo video here (YouTube/Drive link)*  
