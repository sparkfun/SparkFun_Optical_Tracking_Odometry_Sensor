# Load instrument control package to open a serial port
pkg load instrument-control

# If the serial port was opened before, ensure it's been closed before we try
# to re-open it
if(exist("ser", "var") == 1)
  fclose(ser);
end

# Open the serial port
ser = serial("COM12");

# Flag to know if the test is still in progress
testInProgress = true;

# Index for which angle we're currently logging data for
angleIdx = 0;

# Initialize array of velocity measurements
vel = [];

# Loop until test is done
while testInProgress

  # Set flag to indicate that we're collecting data for a new angle
  collectingData = true;

  # Reset index for which measurement we're recording into the array
  velIdx = 1;

  # Loop until we've collected all the data for this angle
  while collectingData

    # Reset integer array for incoming serial data
    int_array = uint8(1);

    # Reset index into integer array
    i = 1;

    # Set flag to indicate that we're still reading this data point
    not_terminated = true;

    # Loop until we've read this data point
    while not_terminated

      # Read next character from serial port
      val = srl_read(ser, 1);

      # Check if next character is a newline
      if(val == 10)

        # Clear flag to know that we're done with this data point
        not_terminated = false;

        # Convert integer array into character array (string)
        char_array = char(int_array);

        # Check if message is "New angle" or "Done"
        if(char_array(1) == 'N' || char_array(1) == 'D')

          # We're done with this angle, clear flag
          collectingData = false;

          # Check if the whole test is done, in which case clear that flag
          if(char_array(1) == 'D')
            testInProgress = false;
          end

          # Dispay this line for the user to see
          disp(char_array)

        else

          # Not a message, must be a data point. Convert from string to number and store it
          vel(angleIdx, velIdx) = str2num(char_array);

          # Increment index for next data point
          velIdx = velIdx + 1;

        end
      else

        # Not a newline character, must be next byte of this data point. Store in array
        int_array(i) = val;

        # Increment index for next byte
        i = i+1;

      end
    endwhile
  end

  # Increment index for next angle
  angleIdx = angleIdx + 1;

end

# Test is done, close the serial port
fclose(ser);

# Create array of true velocities
vTrue = -100:1:100;

# Select Figure 1 and clear it
figure 1
clf

# Plot the resolution variation in both the forwards and reverse directions,
# remembering to flip the reverse direction so it matches
hold on
plot(vTrue,abs(vel(1,:)./vTrue), 'b')
plot(-vTrue,abs(vel(3,:)./-vTrue), 'r')

# Format plot
xlim([-100 100])
xticks(-100:10:100)
ylim([.85 1.05])
yticks(.85:.01:1.05)
title("X-axis")
xlabel("True Velocity (IPS)")
ylabel("Resolution Variation (Measured Velocity / True Velocity")
legend("Forward", "Backward")
grid on

# Select Figure 2 and clear it
figure 2
clf

# Plot the resolution variation in both the forwards and reverse directions,
# remembering to flip the reverse direction so it matches
hold on
plot(vTrue,abs(vel(2,:)./vTrue), 'b')
plot(-vTrue,abs(vel(4,:)./-vTrue), 'r')

# Format plot
xlim([-100 100])
xticks(-100:10:100)
ylim([.85 1.05])
yticks(.85:.01:1.05)
title("Y-axis")
xlabel("True Velocity (IPS)")
ylabel("Resolution Variation (Measured Velocity / True Velocity")
legend("Forward", "Backward")
grid on

