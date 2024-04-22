###Example 1: Basic Readings

This first example just does some basic measurements to make sure everything is hooked up correctly. To find Example 1, go to **File** > **Examples** > **SparkFun Qwiic OTOS** > **Example1_BasicReadings**:


<figure markdown>
[![Optical Tracking Odometry Sensor Arduino Example 1](assets/img/Arduino_Example_1_Menu.jpg){ width="600" }](assets/img/Arduino_Example_1_Menu.jpg "Click to enlarge")
<figcaption markdown>Finding Example 1</figcaption>
</figure>

Alternatively, you can expand the link below and copy and paste the code into a shiny new Arduino sketch: 

??? "Example 1 Arduino Code"
	```
	--8<-- ""
	```

Make sure you've selected the correct board and port in the Tools menu and then hit the upload button. Once the code has finished uploading, go ahead and open a [Serial Monitor](https://learn.sparkfun.com/tutorials/terminal-basics). You should see something similar to the following.  



###Example 2: SetUnits

This example sets the desired units for linear and angular measurements. Can be either meters or inches for linear, and radians or degrees for angular. If not set, the default is inches and degrees. Note that this setting is not stored in the sensor, it's part of the library, so you need to set at the start of all your programs.

To find Example 2, go to **File** > **Examples** > **SparkFun Qwiic OTOS** > **Example2_SetUnits**:


<figure markdown>
[![Optical Tracking Odometry Sensor Arduino Example 2](assets/img/Arduino_Example_2_Menu.jpg){ width="600" }](assets/img/Arduino_Example_2_Menu.jpg "Click to enlarge")
<figcaption markdown>Finding Example 2</figcaption>
</figure>

Alternatively, you can expand the link below and copy and paste the code into a shiny new Arduino sketch: 



??? "Example 2 Arduino Code"
	```
	--8<-- ""
	```


###Example 3: Calibration

The IMU on the Optical Tracking Odometry Sensor includes a gyroscope and accelerometer, which could have an offset. The OTOS performs a quick calibration when it powers up, but it is recommended to perform a more thorough calibration at the start of all your programs. 

To find Example 3, go to **File** > **Examples** > **SparkFun Qwiic OTOS** > **Example3_Calibration**:


<figure markdown>
[![Optical Tracking Odometry Sensor Arduino Example 3](assets/img/Arduino_Example_3_Menu.jpg){ width="600" }](assets/img/Arduino_Example_3_Menu.jpg "Click to enlarge")
<figcaption markdown>Finding Example 3</figcaption>
</figure>

Alternatively, you can expand the link below and copy and paste the code into a shiny new Arduino sketch: 



??? "Example 3 Arduino Code"
	```
	--8<-- ""
	```



###Example 4: SetOffsetAndPosition 

This example shows how to set the offset for the sensor relative to the center of the robot. The units default to inches and degrees, but if you want to use different units, make sure you specify them before setting the offset.

!!! warning 

    Note that as of firmware version 1.0, these values will be lost after a power cycle, so you will need to set them each time you power up the sensor. 


To find Example 4, go to **File** > **Examples** > **SparkFun Qwiic OTOS** > **Example4_SetOffsetAndPosition**:


<figure markdown>
[![Optical Tracking Odometry Sensor Arduino Example 4](assets/img/Arduino_Example_4_Menu.jpg){ width="600" }](assets/img/Arduino_Example_4_Menu.jpg "Click to enlarge")
<figcaption markdown>Finding Example 4</figcaption>
</figure>

Alternatively, you can expand the link below and copy and paste the code into a shiny new Arduino sketch: 



??? "Example 4 Arduino Code"
	```
	--8<-- ""
	```


###Example 5: VelocityAndAcceleration

This example prints out the velocity and acceleration of the bot as it moves. 


To find Example 5, go to **File** > **Examples** > **SparkFun Qwiic OTOS** > **Example5_VelocityAndAcceleration**:


<figure markdown>
[![Optical Tracking Odometry Sensor Arduino Example 5](assets/img/Arduino_Example_5_Menu.jpg){ width="600" }](assets/img/Arduino_Example_5_Menu.jpg "Click to enlarge")
<figcaption markdown>Finding Example 5</figcaption>
</figure>

Alternatively, you can expand the link below and copy and paste the code into a shiny new Arduino sketch: 



??? "Example 5 Arduino Code"
	```
	--8<-- ""
	```


###Example 6: StandardDeviation

This example reads the standard deviation of the tracking. Note that these values are just the square root of the diagonal elements of the covariance matrices of the Kalman filters used in the firmware of the OTOS and not actual tracking error data. 

!!! warning 

    THESE VALUES DO NOT REPRESENT THE ACTUAL TRACKING ERROR! These are statistical quantities that assume a correct model of the system, but there could be unmodelled error sources that cause the physical error to become larger than these statistical error (eg. improper calibration, or rotating the OTOS to not be flat against the tracking surface). These are provided primarily for anyone wanting to perform sensor fusion with additional sensors, but they can be used to at least "get an idea" of the quality of the accuracy.
    


To find Example 6, go to **File** > **Examples** > **SparkFun Qwiic OTOS** > **Example6_StandardDeviation**:


<figure markdown>
[![Optical Tracking Odometry Sensor Arduino Example 6](assets/img/Arduino_Example_6_Menu.jpg){ width="600" }](assets/img/Arduino_Example_6_Menu.jpg "Click to enlarge")
<figcaption markdown>Finding Example 6</figcaption>
</figure>

Alternatively, you can expand the link below and copy and paste the code into a shiny new Arduino sketch: 



??? "Example 6 Arduino Code"
	```
	--8<-- ""
	```




###Example 7: GetVersion

This should be fairly self explanatory. This example gets the current hardware and firmware version of the board. 


To find Example 7, go to **File** > **Examples** > **SparkFun Qwiic OTOS** > **Example7_GetVersion**:


<figure markdown>
[![Optical Tracking Odometry Sensor Arduino Example 7](assets/img/Arduino_Example_7_Menu.jpg){ width="600" }](assets/img/Arduino_Example_7_Menu.jpg "Click to enlarge")
<figcaption markdown>Finding Example 7</figcaption>
</figure>

Alternatively, you can expand the link below and copy and paste the code into a shiny new Arduino sketch: 



??? "Example 7 Arduino Code"
	```
	--8<-- ""
	```



###Example 8: SelfTest 

This example shows how to set the offset for the sensor relative to the center of the robot. The units default to inches and degrees, but if you want to use different units, make sure you specify them before setting the offset.

To find Example 8, go to **File** > **Examples** > **SparkFun Qwiic OTOS** > **Example8_SelfTest**:


<figure markdown>
[![Optical Tracking Odometry Sensor Arduino Example 8](assets/img/Arduino_Example_8_Menu.jpg){ width="600" }](assets/img/Arduino_Example_8_Menu.jpg "Click to enlarge")
<figcaption markdown>Finding Example 8</figcaption>
</figure>

Alternatively, you can expand the link below and copy and paste the code into a shiny new Arduino sketch: 



??? "Example 8 Arduino Code"
	```
	--8<-- ""
	```