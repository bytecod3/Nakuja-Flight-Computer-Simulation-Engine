%{
Generate wrong altitude simulation 

%}

% flight time 
num_samples = 1000;
time = 0:0.01:50;
time = time(2:end); 

f = length(time)/10;
x = linspace(0, 10000, num_samples);  % y-axis
y = linspace(1000,0, num_samples); % x-axis

x1 = linspace(10000, 20000, num_samples);
y1 = linspace(0, 1000, num_samples);

% join the vectors 
x_joined = [x x1];
y_joined = [y y1];


% send to csv
m = [x_joined' y_joined'];
dlmwrite('wrong_altitude_data_1.csv', m); 

m1 = [x' y'];
dlmwrite('wrong_altitude_data_2_slope.csv', m1); 

%plot(x_joined ,y_joined);
plot(x' ,y');
xlabel( 'Time(sec)' );
ylabel('Altitude (m)');
title("Altitude test data");
grid on

