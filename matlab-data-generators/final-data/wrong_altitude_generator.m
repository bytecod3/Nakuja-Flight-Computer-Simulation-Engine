%{
Generate wrong altitude simulation 

%}

% flight time 
num_samples = 1000;
time = 0:0.01:50;
time = time(2:end); 
xu = 0.0005;

f = length(time)/10;
y = linspace(0, 10000, num_samples);  % y-axis
x = linspace(500,0, num_samples); % x-axis

y1 = linspace(0, 10000, num_samples);
x1 = linspace(500, 1000, num_samples);

% join the vectors 
x_joined = [x x1];
y_joined = [y y1];

% send to csv
m = [x_joined' y_joined'];
dlmwrite('wrong_altitude_data.csv', m); 

plot(x_joined ,y_joined);
xlabel( 'Time(sec)' );
ylabel('Altitude (m)');
title("Altitude test data");
grid on

