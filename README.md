# Using the Pi Pico and pico-sdk

This project contains firmware for the Raspberry Pi Pico. It includes source files for core logic, display handling, and network features.

- **CYW43** is the driver used to interface with the onboard network card.  
- **LWIP** is used in the construction of the UDP stack.  
- **ILI9341** and **XPT2046** are modified drivers for the display.

---

## Project Structure

- [`src`](https://gitlab.doc.ic.ac.uk/lab2425_summer/armv8_13/-/tree/pi_pico/extension/pico_files/src) – Main application logic  
  - [`display`](https://gitlab.doc.ic.ac.uk/lab2425_summer/armv8_13/-/tree/pi_pico/extension/pico_files/src/display) – Display-related code  
  - [`network`](https://gitlab.doc.ic.ac.uk/lab2425_summer/armv8_13/-/tree/pi_pico/extension/pico_files/src/network) – Network features

---

## Flashing the Pico

You can flash the firmware to your Raspberry Pi Pico in two main ways:

### Method 1: Flashing via UF2 File

1. **Build the UF2 File**

   Make sure you have the [Pico SDK](https://github.com/raspberrypi/pico-sdk) and CMake toolchain set up correctly.

   ```bash
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
- A Raspberry Pi Pico

---

## Troubleshooting

- If `cmake` fails, ensure the Pico SDK path is set correctly (`PICO_SDK_PATH`).
- If RPI-RP2 doesn’t appear, try another USB cable or port. Or Pi.
- For `picotool` to work, make sure `pico_enable_stdio_usb()` is set in your `CMakeLists.txt`.
- You can view the debug output using a serial connection, for example: `sudo screen /dev/ttyACM0 115200`.

## Using the Software

It's best to begin by monitoring the serial output, a delay is included in the main loop to give you time to connect and see the output. 
The WiFi network should connect and return an IP, Gateway and Subnet Mask. If not, please check the credentials, or use the optional scan_networks() function.
(scan_networks() is currently unfinished.)

Once the WiFi connection is established, the program tries to start a UDP server.
