%{
Generate altitude simulation 

%}

% flight time 
samples = 100000;
time = 0:0.001:50; % half the flight time is 50 seconds, 50 seconds to reach apogee
time = time(2:end); 

max_alt = 4000; % 4km 
min_alt = 0;

a = linspace(0, max_alt, samples/2); a = a(:);
x = 1.5;
powered_flight = 0.05*a.^x; % exponential growth function

% descent stage 
% straight line from 50 seconds to 100 seconds 
t = 50:0.001:100;
t = t(2:end);
desc = linspace(powered_flight(end), 0, samples/2); desc = desc(:);

plot(time, powered_flight, t, desc);
xlabel( 'x' )
grid on