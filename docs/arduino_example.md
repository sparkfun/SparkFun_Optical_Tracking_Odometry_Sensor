---
icon: simple/arduino
---


## Calibration

### System Calibration
To get any sort of useful data out of your Soil Moisture Sensor, it is advised that you calibrate it to whatever soil you plan to monitor. Different types of soil can affect the sensor, and you may get different readings from one composition tot he next. Before you start storing moisture data or triggering events based on that value, you should see what values you are actually getting from your sensor. Using the sketch above, note what values your sensor outputs when the sensor is completely dry vs when the sensor is completely submerged in a shallow cup of water. Depending on what microcontoller you're using, the operating voltage of that microcontoller, and the resolution of its analog-to-digital converter, you're results will vary.

For example, using the same circuit above, if I detach the VCC pin from D7 and attach it directly to the 5V supply on the RedBoard, you'll see the close to the following values in the serial monitor when the sensor is dry (~0) and when it is completely saturated with moisture (~880).



<figure markdown>
[![5V Calibration Values](assets/img/5V_Cal.png){ width="200" }](assets/img/5V_Cal.png "Click to enlarge")
<figcaption markdown>5V Calibration Values</figcaption>
</figure>


But, if I take the VCC pin and connect it to the 3.3V supply on the RedBoard, the values change. As expected, they get lower since there is less resolution between 0V and 3.3V than there is between 0V and 5V.

<figure markdown>
[![3.3V Calibration Values](assets/img/3_3V_Cal.png){ width="200" }](assets/img/3_3V_Cal.png "Click to enlarge")
<figcaption markdown>3.3V Calibration Values</figcaption>
</figure>

Thus, it is difficult to write an example sketch that works for all platforms. It really depends on the operating voltage and ADC resolution of the board you're using.

### Soil Calibration
Once you have an idea what values your sensor is outputting in completely dry and completely wet situations, it's time to calibrate your sensor for the specific soil you want to monitor. Do the same test above, only this time test your soil when it is as dry as possible, then measure it when the soil is completely saturated with moisture. Getting these values and comparing them to the ones from the previous calibration will give you the best insight into what values mean for your specific plant and soil. This test may take some trial and error and patience. Be careful not to over-water (or under-water) your plants during these tests.

Once you have a good handle on the values you can expect, you can use the map() function to adjust your code accordingly.


## Example 1 - Basic Readings

For this first example, we will connect the sensor to a RedBoard or other Arduino-compatible board in a minimalist fashion to show its output over the serial terminal.

Example 1 lives in the "examples" folder in the GitHub repository. Alternatively, you can expand the link below and copy and paste the code into a shiny new Arduino sketch: 

<!--
??? "Example 1 Arduino Code"
	```
	--8<-- "https://raw.githubusercontent.com/sparkfun/SparkFun_BMI270_Arduino_Library/main/examples/Example01_BasicReadingsI2C/Example01_BasicReadingsI2C.ino"
	```
-->

Make sure you've selected the correct board and port in the Tools menu and then hit the upload button. Once the code has finished uploading, go ahead and open a [Serial Monitor](https://learn.sparkfun.com/tutorials/terminal-basics). You should see something similar to the following. 


<figure markdown>
[![5V Calibration Values](assets/img/5V_Cal.png){ width="200" }](assets/img/5V_Cal.png "Click to enlarge")
<figcaption markdown>5V Calibration Values</figcaption>
</figure>

















