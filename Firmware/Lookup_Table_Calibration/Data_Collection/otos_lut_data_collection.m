# Load instrument control package to open a serial port
pkg load instrument-control

# If the serial port was opened before, ensure it's been closed before we try
# to re-open it
if(exist("ser", "var") == 1)
  fclose(ser);
end

# Open the serial port
ser = serial("COM12");

# Set the max speed (default: 120) and resolution (default: 1) of the test
maxSpeed = 120;
speedInterval = 1;

# Compute number of data points along each axis
numPoints = 2*maxSpeed/speedInterval+1;

# Generate a meshgrid of all surfave velocities
tx = ty = linspace (-maxSpeed, maxSpeed, numPoints)';
[xx, yy] = meshgrid (tx, ty);

# Pre-allocate data arrays
xcal = zeros(numPoints,numPoints);
ycal = zeros(numPoints,numPoints);

# Flag to know that the test is still in progress
testInProgress = true;

# Loop until test is done
while testInProgress

  # Reset integer array for incoming serial data
  int_array = uint8(1);

  # Reset index into integer array
  i = 1;

  # Set flag to indicate that we're still reading this data point
  not_terminated = true;

  # Index for which value we're looking at in this line from the serial port
  j = 1;

  # Loop until we're done with this data point
  while not_terminated

    # Read the next byte from the serial port
    val = srl_read(ser, 1);

    # Check if it's a newline character, and clear flag if so
    if(val == 10)
      not_terminated = false;
    end

    # Check for "Done" message
    if(val == 'D')

      # Clear flags to indicate the test is over
      testInProgress = false;
      not_terminated = false;

    # Check if next character is a tab or newline
    elseif(val == 9 || val == 10)

      # That's the end of this value in this line, convert in array to char array (string)
      char_array = char(int_array);

      # Convert string to number and store in value list
      foo(j) = str2num(char_array);

      # Increment index for which value we're looking at in this line
      j = j+1;

      # Reset the int array
      int_array = uint8(1);

      # Increment index within int array
      i = 1;

    else

      # Must be next byte of this data point. Store in array
      int_array(i) = val;

      # Increment index for next byte
      i = i+1;

    end
  endwhile

  # Display this line for the user to see
  disp(foo)

  # Separate out values within this line, 4 are expected:
  # True X velocity
  # True Y velocity
  # Measured X velocity
  # Measured Y velocity
  vtx = foo(1);
  vty = foo(2);
  vmx = foo(3);
  vmy = foo(4);

  # Compute index into data arrays
  idxX = (vtx+maxSpeed)/speedInterval + 1;
  idxY = (vty+maxSpeed)/speedInterval + 1;

  # Compute resolution variation, being careful to not divide by zero
  if(vtx != 0)
    xcal(idxY,idxX) = vmx / vtx;
  end
  if(vty != 0)
    ycal(idxY,idxX) = vmy / vty;
  end

  # Update plots once per row of data
  if(vtx == maxSpeed)

    # Select Figure 3 and clear it
    figure 3;
    clf;

    # Plot the x-axis resolution variation
    mesh(xx, yy, xcal);

    # Format plot
    title("X Variation")
    xlabel("True X Velocity (IPS)")
    ylabel("True Y Velocity (IPS)")
    xlim([-maxSpeed, maxSpeed])
    ylim([-maxSpeed, maxSpeed])
    zlim([.8, 1.2])
    caxis([.9, 1.0])
    xticks([-maxSpeed:10:maxSpeed])
    yticks([-maxSpeed:10:maxSpeed])
    zticks([.8:.01:1.2])
    view(0,90)

    # Select Figure 5 and clear it
    figure 4;
    clf;

    # Plot the x-axis resolution variation
    mesh(xx, yy, ycal);

    # Format plot
    title("Y Variation")
    xlabel("True X Velocity (IPS)")
    ylabel("True Y Velocity (IPS)")
    xlim([-maxSpeed, maxSpeed])
    ylim([-maxSpeed, maxSpeed])
    zlim([.8, 1.2])
    caxis([.9, 1.0])
    xticks([-maxSpeed:10:maxSpeed])
    yticks([-maxSpeed:10:maxSpeed])
    zticks([.8:.01:1.2])
    view(0,90)
  end
end

# Test is complete, close the serial port
fclose(ser);

# Write data to CSV files
csvwrite("xcal.csv", xcal)
csvwrite("ycal.csv", ycal)

