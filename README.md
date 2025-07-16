# Plant Health Analysis - Getting Started

## 1. Setting up the Pi Pico

This project contains firmware for the Raspberry Pi Pico. It includes source files for core logic, display handling, and network features.

- **CYW43** is the onboard network chip, we interface with this using *wifi_handler*.  
- **LWIP** is used in the construction of the UDP stack.  
- **ST7789** and **XPT2046** are the display controllers for render and touch respectively, we have a custom driver for the display.

---

## Project Structure

- [`pico_files`](https://github.com/Arg2006/ARM_Presentation/tree/main/pico_files/) – All files pertaining to the Pi Pico
   - [`include`](https://github.com/Arg2006/ARM_Presentation/tree/main/pico_files/include) – Configuration options for networking
   - [`src`](https://github.com/Arg2006/ARM_Presentation/tree/main/pico_files/src/) –  Main application logic for Pico
      - [`network`](https://github.com/Arg2006/ARM_Presentation/tree/main/src/network) – Network features
      - [`display`](https://github.com/Arg2006/ARM_Presentation/tree/main/pico_files/src/display) – Display-related code
- [`sensors`](https://github.com/Arg2006/ARM_Presentation/tree/main/sensors) – Program for reading sensor data
- [`src`](https://github.com/Arg2006/ARM_Presentation/tree/main/src) – Image processing and analysis algorithm


---

## Flashing the Pico

You can flash the firmware to your Raspberry Pi Pico in two main ways:

### Method 1 (recommended): Flashing via UF2 File

1. **Build the UF2 File**

   Make sure you have the [Pico SDK](https://github.com/raspberrypi/pico-sdk) and CMake toolchain set up correctly.

   ```bash
   cd pico_files
   mkdir build
   cd build
   cmake -DPICO_BOARD=pico_w -DWIFI_SSID="ssid_here" -DWIFI_PASSWORD="top_secret" ..
   make
   ```

   After the build, you'll find the `.uf2` file in the `build` directory.
   (Compiler flag for board type to be removed in a future version.)

2. **Put the Pico into Bootloader Mode**

   - Hold down the **BOOTSEL** button on the Pico.
   - Plug it into your computer via USB.
   - Release the button once it's connected.
   - A new mass storage device (RPI-RP2) should appear.

3. **Copy the UF2**

   - Drag and drop the `.uf2` file onto the RPI-RP2 drive.
   - The Pico will reboot and start running the flashed firmware.

---

### Method 2: Flashing via Pico SDK (Using `picotool`)

1. **Build the ELF File**

   ```bash
   cd pico_files
   mkdir build
   cd build
   cmake ../pico_files
   make
   ```

   This will generate a `.elf` file in the `build` directory.

2. **Connect the Pico Normally (no BOOTSEL)**

   - Plug in the Pico (no need to hold BOOTSEL if already flashed before).
   - Make sure `picotool` is installed ([Install Instructions](https://github.com/raspberrypi/picotool)).

3. **Use `picotool` to Flash**

   ```bash
   sudo picotool load your_program.elf -f
   ```

4. **Verify Flash**

   ```bash
   picotool info
   ```

---

## Requirements

- CMake (for pico-sdk)
- GCC for ARM (`arm-none-eabi-gcc`)
- Pico SDK
- `picotool` (for SDK flashing, optional)
- A Raspberry Pi Pico W or equivalent RP2040 device with CYW43 WiFi chip.

---

## Troubleshooting

- If `cmake` fails, ensure the Pico SDK path is set correctly (`PICO_SDK_PATH`).
- If RPI-RP2 doesn’t appear, try another USB cable or port. Or Pi.
- For `picotool` to work, make sure `pico_enable_stdio_usb()` is set in your `CMakeLists.txt`.
- You can view the debug output using a serial connection, for example: `sudo screen /dev/ttyACM0 115200`.

## Using the Software

It's best to begin by monitoring the serial output, a delay is included in the main loop to give you time to connect and see the output. 
The WiFi network should connect and return an IP, Gateway and Subnet Mask. If not, please check the credentials, or use the optional scan_networks() function.
(scan_networks() is not included in this version to save space, see version history, scan_networks.c.)

Once the WiFi connection is established, the program attempts to start a UDP server. The output is verbose, so if this fails, debug at the current step.

Once the UDP server is active, the display will load the default monitoring page, and you're ready to move onto the next step!

## 2. Setting up the Pi 3B (or similar)

### Setting up the sensors

The program expects the sensors to be connected in the following arrangement:

   SHT30-D I2C Temperature and Humidity Sensor:
   SAA - GPIO 8
   SCL - GPIO 9

   LDR:
   Connect in RC charging circuit, connect +ve leg of capacitor to GPIO 0

   Moisture Sensor:
   As above, but connect to GPIO 2.

   Relay for pump:
   Connect to GPIO 3.

You are now ready to run the algorithm, simply point the camera at the plant, ```make extension``` and voila!
