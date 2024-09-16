v1 = 0; % initial velocity
g = 9.8; % acceleration due to gravity 
t_flight = 12; % time of flight 


[v_p, v_c] = GenerateVelocity(v1, g, t_flight);

% plot the curve 
figure(1);
plot(p_flight_vec, v_p); hold on;
plot(c_flight_vec, v_c);
title("Rocket velocity data");
xlabel("Time (s)");
ylabel("Velocity (m/s)");
legend("Powered-flight", "Coasting");