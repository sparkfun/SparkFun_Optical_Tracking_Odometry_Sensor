---
icon: first
---

!!! attention
	Please ensure you have done the Software Setup before attempting to run the example! You need to have at least version v9.2 of the SDK running on the Control Hub, and add the sensor to your Hardware Config.

Unlike our Arduino and Python libraries that include several examples, there is only a single but comprehensive example for FTC. This sample OpMode is available in Blocks, OnBot Java, and Android Studio; setup instructions are avaiable for each, but running the sample is the same.

# Blocks

Create a new OpMode based on the sample called `SensorSparkFunOTOS`.

TODO

# OnBot Java

Create a new OpMode based on the sample called `SensorSparkFunOTOS`.

TODO

# Android Studio

Navigate to the sample OpModes under FtcRobotController -> java -> `org.firstinspires.ftc.robotcontroller.external.samples` -> `SensorSparkFunOTOS`. Make a copy of this file in the TeamCode folder along with the rest of your code.

!!! attention
	It is strongly recommended that you do not modify the sample OpModes within the FtcRobotController sample folder! Always make a copy so you have clean examples to work with!

After making a copy, comment out or delete the `@Disabled` to enable the OpMode. Then upload the code to the robot.


<figure markdown>
[![](assets/img/ftc_example/android_studio.png){ width="100%" }](assets/img/ftc_example/android_studio.png "Click to enlarge")
<figcaption markdown></figcaption>
</figure>

# Run the Example

On the Driver Station, select the TeleOp dropdown menu, and you should see the `Sensor: SparkFun OTOS` sample OpMode; select it.

When you press the `INIT` button, the sensor will initialize, the IMU will be calibrated, and the tracking algorithm will be reset. The telemetry on the driver station will display the version numbers read from the device.

When you press the `Start` button, the telemetry will display the (x,y) coordinates and heading angle of the robot, as measure by the sensor. If you have gamepad #1 connected, you can press Y (triangle) to reset the tracking, or press X (square) to re-calibrate the IMU.

<figure markdown>
[![](assets/img/ftc_example/driver_station.jpg){ width="400" }](assets/img/ftc_example/driver_station.jpg "Click to enlarge")
<figcaption markdown></figcaption>
</figure>