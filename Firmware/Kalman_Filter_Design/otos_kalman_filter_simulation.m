################################################################################
# Simulation setup
################################################################################

# The OTOS velocity measurement was calculated to have a standard deviation of
# about 3 inches per second
velStd = 3 * .0254;

# The IMU acceleration measurement was calculated to have a standard deviation
# of about 0.03 meters per second^2
accStd = 0.03;

# The OTOS has 2.4ms loop period. With 1500 time steps, this simulates about 3.5
# seconds
dt = 0.0024;
numSteps = 1500;
t = 0 : dt : dt*(numSteps-1);

# Acceleration offset error
accOffset = 0.5;

# Generate measurement noise
velNoise = randn([numSteps, 1]) * velStd;
accNoise = randn([numSteps, 1]) * accStd + accOffset;

# Pre-allocate arrays for position, velocity, and acceleration
a = zeros([numSteps, 1]);
v = zeros([numSteps, 1]);
p = zeros([numSteps, 1]);

# Simulate acceleration at start and end
a(t < .3) = 5;
a(t > 3) = -5;

# Initialize Kalman filter state vector X:
# [Position]
# [Velocoty]
# [Accleration]
X = zeros([3,1]);

# Initialize Kalman filter state transition model F (X_{k+1} = F * X_k):
# [P_{k+1}] = P_k + V_k * dt = [1 dt  0]   [P_k]
# [V_{k+1}] = V_k + A_k * dt = [0  1 dt] * [V_k]
# [A_{k+1}] = A_k            = [0  0  1]   [A_k]
F = eye(3);
F(1,2) = dt;
F(2,3) = dt;

# Initialize Kalman filter state estimate covariance matrix P:
P = eye(3)*0;

# Initialize Kalman filter observation models H (Z_k = H * X_k):
# Z_{vk} = [0 1 0] * [P_k]
#                    [V_k]
#                    [A_k]
# Z_{ak} = [0 0 1] * [P_k]
#                    [V_k]
#                    [A_k]
Hv = [0 1 0];
Ha = [0 0 1];

# Initialize Kalman filter observation noise variance elements R:
Rv = velStd^2;
Ra = accStd^2;

# Initialize Kalman filter process noise covariance matrix Q:
Q = eye(3);
Q(1,1) = 1e-8;
Q(2,2) = 1e-6;
Q(3,3) = 1e-4;

################################################################################
# Simulation loop
################################################################################

# Loop through all time steps
for i = 1:numSteps

  # Physics time step, calculate next true velocity and position
  v(i+1) = v(i) + a(i) * dt;
  p(i+1) = p(i) + v(i) * dt;

  # Sensor measurement
  za = [a(i) + accNoise(i)];
  zv = [v(i) + velNoise(i)];

  # Simulate temporary loss of optical data
  if(i >= 500 && i < 700)
    zv = 0;
  end

  % Kalman filter prediction step
  X = F*X;
	P = F*P*F' + Q;

  # Kalman filter measurement update (accleration)
  y = za - Ha*X;
  S = Ha*P*Ha' + Ra;
  K = P*Ha'*inv(S);
  X = X + K*y;
  P = (eye(3) - K*Ha)*P;

  # Ignore velocity measurement if measurement is very different from estimate
  if(abs(zv - X(2)) < .5)
    # Kalman filter measurement update (velocity)
    y = zv - Hv*X;
    S = Hv*P*Hv' + Rv;
    K = P*Hv'*inv(S);
    X = X + K*y;
    P = (eye(3) - K*Hv)*P;
  else
    # Log velocity for plots to know where measurements are being ignored
    v(i) = 0;
  end

  # Log Kalman filter state and diagonal elements of covariance matrix
  ps(i) = X(1);
	pvs(i) = P(1,1);
  vs(i) = X(2);
	vvs(i) = P(2,2);
  as(i) = X(3);
	avs(i) = P(3,3);
end

# Remove last couple elements of these arrays to get sizes correct for plots
v(end) = [];
p(end) = [];

################################################################################
# Output plots
################################################################################

# Use Figure 1 and clear it
figure 1
clf

# Position subplot
subplot(3,1,1)
hold on
grid on

# True position
plot(t, p, 'g')

# Estimated position and standard deviation
plot(t, ps, 'b')
plot(t, ps - sqrt(pvs), 'r')
plot(t, ps + sqrt(pvs), 'r')

# Velocity subplot
subplot(3,1,2)
hold on
grid on

# True velocity and measured velocity
plot(t, v, 'g')
plot(t, v + velNoise, 'k')

# Estimated velocity and standard deviation
plot(t, vs, 'b')
plot(t, vs - 2*sqrt(vvs), 'r')
plot(t, vs + 2*sqrt(vvs), 'r')

# Acceleration subplot
subplot(3,1,3)
hold on
grid on

# True acceleration and measured velocity
plot(t, a, 'g')
plot(t, a + accNoise, 'k')

# Estimated acceleration and standard deviation
plot(t, as, 'b')
plot(t, as - 2*sqrt(avs), 'r')
plot(t, as + 2*sqrt(avs), 'r')

