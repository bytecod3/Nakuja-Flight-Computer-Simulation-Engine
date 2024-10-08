%{
Velocity curve
This script generates a velocity curve for the rocket 
The velocity is initially zero, 
%}

% parameters 
v1 = 0; % initial velocity
g = 9.8; % acceleration due to gravity 
t_flight = 12; % time of flight 

% generating time vectors 
t = 0:1:t_flight;
powered_flight_time = 6; % time taken for powered flight
coasting_flight_time = 4; % time taken for coasting

p_flight_vec = 0:1:powered_flight_time;
c_flight_vec = powered_flight_time:1:t_flight;

% velocity equation 
% calculate velocity for powered flight. The acceleration due to gravity is +ve
v_powered = v1 + g .* p_flight_vec; 

% calculate velocity for coasting. The acceleration due to gravity is -ve
% velocity for coasting starts at max velocity for powered 
v_coast = v_powered(end) + (-g) .* p_flight_vec;

% plot the curve 
figure(1);
plot(p_flight_vec, v_powered); hold on;
plot(c_flight_vec, v_coast);
title("Rocket velocity data");
xlabel("Time (s)");
ylabel("Velocity (m/s)");
legend("Powered-flight", "Coasting");