% v1 = 0;         % initial velocity
% g = 9.8;        % acceleration due to gravity 
% t_flight = 12;  % time of flight 
% 
% [v_p, v_c] = GenerateV(v1, g, t_flight);
% 
% p_flight_vec = 0:1:6;
% c_flight_vec = 6:1:t_flight;
% 
% disp("V Powered");
% disp(v_p);
% disp("V Coasting");
% disp(v_c);

% plot the curve
% figure(1);
% plot(p_flight_vec, v_p); hold on;
% plot(c_flight_vec, v_c)
% %plot(c_flight_vec, v_c);
% title("Rocket velocity data");
% xlabel("Time (s)");
% ylabel("Velocity (m/s)");
% legend("Powered-flight", "Coasting");

Fs = 1000;
Ts = 1/Fs;
t = 0:Ts:3;
t = t(:);

% 12 to 18 in steps of 1
x = 12:1:18;

% ideal accelration in the -ve direction from apogee
y = x .* 2;
y = -1 * y;

plot(x, y);
hold on;

% terminal velocity
x = 18:1:24; 
y_terminal = ones(length(x));
y = y(end) .* y_terminal;

plot(x, y, 'r');
title("Model rocket fall");
legend("Negative deceleration", "Terminal velocity");
hold on;

% x = 0.5; % x can be any number greater than 1. Use 1 for linear 
% a = linspace(18, 12, 6); a(:);
% fadeout = a .^ x;
% fadeout = -1 .* fadeout;

% a = linspace(18, 12, 6); a(:);
% fadein = a .^ x;
