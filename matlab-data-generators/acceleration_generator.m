% generate acceleration values with a maximum value of 16G 
% as configured in the flight software MPU6050

Fs = 1000; % sampling frequency
Ts = 1/Fs; 

flight_time = 15; % 15 seconds 
no_samples = Fs * flight_time;

noise = rand(1, no_samples+1);

% prepare the time vector
t = 0:Ts:flight_time;
t = t(:);

plot(t, noise);
