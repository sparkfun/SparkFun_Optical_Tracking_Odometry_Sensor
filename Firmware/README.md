# SparkFun OTOS (Optical Tracking Odometry Sensor) Firmware

This folder contains the firmware used on the [SparkFun Optical Tracking Odometry Sensor](https://www.sparkfun.com/products/24904). Instructions are provided below for anyone wanting to change the firmware on their OTOS.

## Firmware Update

The recommended update procedure is to use the [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html) to flash the provided .hex firmware file using the UART interface. The SWD test points on the OTOS can also be used with a debugger, but instructions are not provided here.

### Required Materials

* A [SparkFun Optical Tracking Odometry Sensor](https://www.sparkfun.com/products/24904) to be updated
* A USB to serial adapter, such as the [SparkFun Serial Basic Breakout](https://www.sparkfun.com/products/15096)
* [Jumper wires](https://www.sparkfun.com/products/12795) to connect the serial adapter to the OTOS
* Download the [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html) and install it onto your computer
* Download the latest .hex file [from the releases tab](https://github.com/sparkfun/SparkFun_Optical_Tracking_Odometry_Sensor/releases) or [from the main branch](https://github.com/sparkfun/SparkFun_Optical_Tracking_Odometry_Sensor/tree/main/Firmware/Release)

### Procedure

1. The STM32 on the OTOS needs to be running its bootloader to flash the new firmware.
    * To do this, the BOOT0 pin must be held high when the STM32 boots up (during power up or a reset). This is broken out to the centermost test point on the bottom of the OTOS. It is suggested to touch a jumper wire connected to 3.3V to this test point while powering up the board - the STM32 will remain in the bootloader until it resets, loses power, or new code is flashed onto it.
2. Connect the serial converter TX and RX pins to IO10 and IO9 respectively.
3. Open the STM32CubeProgrammer. In the top right, select the UART interface, leave the UART configuration at the default settings, and click the Connect button.
    * If it fails to connect, ensure the right COM port is selected and is not used by anything else on your computer, ensure the OTOS is fully powered, ensure the bootloader is running (see Step 1), and ensure the TX and RX pins are correct.
4. Once connected, select the "Erasing & programming" tab on the left. In the "Download" section, select the .hex file to be flashed in the File path, then click "Start Programming"
5. Once complete, the OTOS should start running the new firmware immediately, and will disconnect from the STM32CubeProgrammer.

## Building the Firmware

The firmware was developed using the [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html), which you will need to install in order to build the firmware. It generates code for the low-level hardware functionality, so be aware that code within the `Core` directory may be modified by the code generator if not placed in the correct locations.

To import the project, open the STM32CubeIDE -> File -> Import... -> General -> Existing Projects into Workspace -> Next -> set root directory to this folder called Firmware -> Ensure SparkFun_OTOS_Firmware is selected -> Finish. You should now see the SparkFun_OTOS_Firmware project in the Project Explorer on the left. Once open, you should also change the build configuration from Debug to Release: Project -> Build Configurations -> Set Active -> 2 Release.

To build the project, within the STM32CubeIDE, -> Project -> Build Project. The output .hex file will be placed in the `Release` directory (assuming you've changed the build configuration from Debug to Release).
