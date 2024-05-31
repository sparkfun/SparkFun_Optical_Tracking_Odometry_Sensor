# SparkFun OTOS Kalman Filter Design

This folder contains the simulation that was used to design the Kalman filter for the [SparkFun Optical Tracking Odometry Sensor](https://www.sparkfun.com/products/24904). This script can be run in either [MATLAB](https://www.mathworks.com/products/matlab.html) (not tested) or [GNU Octave](https://octave.org/) (free). This is mainly provided for transparency on how the Kalman filter was designed and tuned, although you're welcome to play around with it and try to improve it!

The Kalman filter is what performs the sensor fusion between the acceleration measurements from the IMU (LSM6DSO), and the velocity measurements from the optical tracking sensor (PAA5160). The LSM6DSO acceleration measurements were measured to have a standard deviation of about 0.03 m/s^2, and the PAA5160 velocity measurements were measured to have a standard deviation of about 3 in/s (0.076 m/s).

The process noise covariance matrix Q was manually tuned to give a good balance of fast response times and low noise. The simulation also includes a handful of tests to verify robustness, such as loss of optical tracking data and acceleration offsets.
