
# Number of CSV files to load
num_files = 8;

# Load CSV files
for i = 1:num_files
  var_uncalib_x(:,:,i) = csvread(["xcal_" char(i+48) ".csv"]);
  var_uncalib_y(:,:,i) = csvread(["ycal_" char(i+48) ".csv"]);
endfor

# Meshgrid for true x/y velocities
xy_max = 120;
dxy = 1;
num_points = xy_max*2+1;
tx = ty = linspace (-xy_max, xy_max, num_points);
[xx_true, yy_true] = meshgrid (tx, ty);

# Plot scales
xy_ticks = -xy_max:10:xy_max;
z_limits = [0.8, 1.2];
z_ticks = 0.8:0.01:1.2;
c_limits_uncal_var = [.9, 1.0];
c_limits_scalar_var = [1.0, 1.1];
c_limits_cal_var = [.98, 1.02];

# Clip out of bounds data points
var_uncalib_x(var_uncalib_x < 0.8) = 0.8;
var_uncalib_y(var_uncalib_y < 0.8) = 0.8;
var_uncalib_x(var_uncalib_x > 1.2) = 1.2;
var_uncalib_y(var_uncalib_y > 1.2) = 1.2;

# Compute average variation of uncalibrated sensors
var_uncalib_avg_x = mean(var_uncalib_x, 3);
var_uncalib_avg_y = mean(var_uncalib_y, 3);

% Plot average X-axis variation
figure 1;
clf;
mesh(xx_true, yy_true, var_uncalib_avg_x);
xlim([-xy_max, xy_max])
ylim([-xy_max, xy_max])
xticks(xy_ticks)
yticks(xy_ticks)
zticks(z_ticks)
zlim(z_limits)
caxis(c_limits_uncal_var)
title("Average X-Axis Variation")
xlabel("True X Velocity (IPS)")
ylabel("True Y Velocity (IPS)")
zlabel("Resolution Variation (Measured Velocity / True Velocity)")

% Plot average Y-axis variation
figure 2;
clf;
mesh(xx_true, yy_true, var_uncalib_avg_y);
xlim([-xy_max, xy_max])
ylim([-xy_max, xy_max])
xticks(xy_ticks)
yticks(xy_ticks)
zticks(z_ticks)
zlim(z_limits)
caxis(c_limits_uncal_var)
title("Average Y-Axis Variation")
xlabel("True X Velocity (IPS)")
ylabel("True Y Velocity (IPS)")
zlabel("Resolution Variation (Measured Velocity / True Velocity)")

# Compute scalars versus measured velocity
for i = 1:num_points
  for j = 1:num_points
    xx_meas(i,j) = var_uncalib_avg_x(i,j) * xx_true(i,j);
    yy_meas(i,j) = var_uncalib_avg_y(i,j) * yy_true(i,j);

    scalar_uncalib_avg_x(i,j) = 1 / var_uncalib_avg_x(i,j);
    scalar_uncalib_avg_y(i,j) = 1 / var_uncalib_avg_y(i,j);
  end
end

# Plot average X-axis scalars
figure 3;
clf;
mesh(xx_meas, yy_meas, scalar_uncalib_avg_x);
xlim([-xy_max, xy_max])
ylim([-xy_max, xy_max])
xticks(xy_ticks)
yticks(xy_ticks)
zticks(z_ticks)
zlim(z_limits)
caxis(c_limits_scalar_var)
title("Average X-Axis Scalars")
xlabel("Measured X Velocity (IPS)")
ylabel("Measured Y Velocity (IPS)")
zlabel("Resolution Scalars (True Velocity / Measured Velocity)")

# Plot average Y-axis scalars
figure 4;
clf;
mesh(xx_meas, yy_meas, scalar_uncalib_avg_y);
xlim([-xy_max, xy_max])
ylim([-xy_max, xy_max])
xticks(xy_ticks)
yticks(xy_ticks)
zticks(z_ticks)
zlim(z_limits)
caxis(c_limits_scalar_var)
title("Average Y-Axis Scalars")
xlabel("Measured X Velocity (IPS)")
ylabel("Measured Y Velocity (IPS)")
zlabel("Resolution Scalars (True Velocity / Measured Velocity)")

# Re-mesh scalars
scalar_uncalib_avg_x_remesh = griddata(xx_meas, yy_meas, scalar_uncalib_avg_x, xx_true, yy_true);
scalar_uncalib_avg_y_remesh = griddata(xx_meas, yy_meas, scalar_uncalib_avg_y, xx_true, yy_true);

# Plot average X-axis scalars
figure 5;
clf;
mesh(xx_true, yy_true, scalar_uncalib_avg_x_remesh);
xlim([-xy_max, xy_max])
ylim([-xy_max, xy_max])
xticks(xy_ticks)
yticks(xy_ticks)
zticks(z_ticks)
zlim(z_limits)
caxis(c_limits_scalar_var)
title("Average X-Axis Scalars")
xlabel("Measured X Velocity (IPS)")
ylabel("Measured Y Velocity (IPS)")
zlabel("Resolution Scalars (True Velocity / Measured Velocity)")

# Plot average Y-axis scalars
figure 6;
clf;
mesh(xx_true, yy_true, scalar_uncalib_avg_y_remesh);
xlim([-xy_max, xy_max])
ylim([-xy_max, xy_max])
xticks(xy_ticks)
yticks(xy_ticks)
zticks(z_ticks)
zlim(z_limits)
caxis(c_limits_scalar_var)
title("Average Y-Axis Scalars")
xlabel("Measured X Velocity (IPS)")
ylabel("Measured Y Velocity (IPS)")
zlabel("Resolution Scalars (True Velocity / Measured Velocity)")

# Sparse grid for lookup table, values chosen by hand
#lut_indices = [-100 -90 -80 -70 -65 -57 -50 -45 -40 -33 -27 -21 -15 -10 -7 -2 2 7 10 15 21 27 33 40 45 50 57 65 70 80 90 100];
lut_indices = [-100 -93 -88 -83 -78 -73 -68 -64 -59 -55 -50 -45 -41 -37 -33 -28 -26 -21 -17 -15 -12 -10 -7 -5 -3 0 3 5 7 10 12 15 17 21 26 28 33 37 41 45 50 55 59 64 68 73 78 83 88 93 100];
[xx_lut, yy_lut] = meshgrid (lut_indices, lut_indices);

# Compute lookup table values
#lut_scalars_x = griddata(xx_meas, yy_meas, scalar_uncalib_avg_x, xx_lut, yy_lut);
#lut_scalars_y = griddata(xx_meas, yy_meas, scalar_uncalib_avg_y, xx_lut, yy_lut);
lut_scalars_x = griddata(xx_true, yy_true, scalar_uncalib_avg_x_remesh, xx_lut, yy_lut);
lut_scalars_y = griddata(xx_true, yy_true, scalar_uncalib_avg_y_remesh, xx_lut, yy_lut);

# Overlap lookup table on scalar plots
figure 5;
hold on;
mesh(xx_lut, yy_lut, lut_scalars_x)
figure 6;
hold on;
mesh(xx_lut, yy_lut, lut_scalars_y)

# Compute expected variation after implementing lookup table
var_calib_avg_x = var_uncalib_avg_x .* interp2(xx_lut, yy_lut, lut_scalars_x, xx_true, yy_true);
var_calib_avg_y = var_uncalib_avg_y .* interp2(xx_lut, yy_lut, lut_scalars_y, xx_true, yy_true);

% Plot average X-axis variation after implementing lookup table
figure 7;
clf;
mesh(xx_true, yy_true, var_calib_avg_x);
xlim([-xy_max, xy_max])
ylim([-xy_max, xy_max])
xticks(xy_ticks)
yticks(xy_ticks)
zticks(z_ticks)
zlim(z_limits)
caxis(c_limits_cal_var)
title("Average X-Axis Variation")
xlabel("True X Velocity (IPS)")
ylabel("True Y Velocity (IPS)")
zlabel("Resolution Variation (Measured Velocity / True Velocity)")

% Plot average Y-axis variation after implementing lookup table
figure 8;
clf;
mesh(xx_true, yy_true, var_calib_avg_y);
xlim([-xy_max, xy_max])
ylim([-xy_max, xy_max])
xticks(xy_ticks)
yticks(xy_ticks)
zticks(z_ticks)
zlim(z_limits)
caxis(c_limits_cal_var)
title("Average Y-Axis Variation")
xlabel("True X Velocity (IPS)")
ylabel("True Y Velocity (IPS)")
zlabel("Resolution Variation (Measured Velocity / True Velocity)")

# Compute integer lookup table values. These are in increments of 0.2% per LSB
# to minimize flash size
lut_scalers_x_int = round((lut_scalars_x - 1) / .002);
lut_scalers_y_int = round((lut_scalars_y - 1) / .002);

lut_scalers_x_int(lut_scalers_x_int > 127) = 127;
lut_scalers_x_int(lut_scalers_x_int < -128) = -128;
lut_scalers_y_int(lut_scalers_y_int > 127) = 127;
lut_scalers_y_int(lut_scalers_y_int < -128) = -128;

# Save lookup table to CSV files
csvwrite("lut_x.csv", lut_scalers_x_int)
csvwrite("lut_y.csv", lut_scalers_y_int)

