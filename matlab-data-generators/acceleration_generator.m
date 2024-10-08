% generate acceleration values with a maximum value of 16G 
% as configured in the flight software MPU6050

no_of_samples = 1000;
Ts = 1/no_of_samples;

% generate x_axis acceleration
x_acceleration_noisy = rand(16, no_of_samples+1);

% to hold acceleration values 
x_acc = ones(no_of_samples, 1);
noise = ones(no_of_samples, 1);

for n = 1:no_of_samples
    r = abs(random('Normal', 1, 16));
    noise(n) = r;
end

% add noise to the acceleration data
x_acc = x_acc .* noise;

% plot
flight_time = 10;
t = 0:Ts:10;
t = t(:);
% plot(t, x_acc);

% fit the curve to exponential functions to model 
% rocket acceleration
x = 0:0.0005:5;
y = exp(x/2);

j = length(x) - 1;
y_a = ones(length(x));
for n = 1:j
    y_a(n) = y .* x_acc(1:j);
end

disp(y_a);
%plot(x, y_a);




