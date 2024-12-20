% plot the given data to continou time graphs
% this is the data from the model function 
% extract the data to get the arrays anf use them 
% to visualize the curves

% time of flight
t_flight = 12;

% generated powered flight velocity
powered_flight_velocity = out.yout.signals(1).values(:,:,1);

% generated coasting flight velocity
coasting_flight_velocity = out.yout.signals(2).values(:,:,1);

% generated powered flight time vector - for plotting
powered_time_vec = out.yout.signals(3).values(:,:,1);

% generated coasting flight time vector - for now we assume that 
% the acceleration time is same as deceleration time to apogee 
coasting_time_vec = powered_time_vec(end):1:t_flight;

% rocket is coming downwards after apogee
% the graph shows ideal velocity behaviour until terminal velocity
negative_velocity = out.yout.signals(4).values(:,:,1);
negative_velocity_time_vec = 12:1:18;

% terminal velocity 
terminal_velocity = out.yout.signals(5).values(:,:,1);
terminal_velocity_time_vec = 18:1:24;

figure(1);
plot(powered_time_vec, powered_flight_velocity); % powered acceleration
hold on; % draw on the same figure
plot(coasting_time_vec, coasting_flight_velocity); % coasting acceleration
hold on;
plot(negative_velocity_time_vec, negative_velocity); % negative velocity
hold on;
plot(terminal_velocity_time_vec, terminal_velocity); % negative velocity
hold on;

% plot labelling
grid on;
title("Rocket velocity data");
xlabel("Time (s)");
ylabel("Velocity (m/s)");
legend("Powered-flight", "Coasting");


%////////////// x acceleration plot /////////////////////
figure(2);
t = out.yout.signals(7).values(:,:,1);
t = t(1,:);
x_acc = out.yout.signals(8).values(:,:,1);
x_acc = x_acc(1,:);
plot(t, x_acc);
grid on;
title("X acceleration");
xlabel("Time (s)");
ylabel("Acceleration (g) ");
legend("X acceleration");


%////////////// y acceleration plot /////////////////////
figure(3);
t = out.yout.signals(7).values(:,:,1);
t = t(1,:);
y_acc = out.yout.signals(6).values(:,:,1);
y_acc = y_acc(1,:);
plot(t, x_acc);
grid on;
title("Y acceleration");
xlabel("Time (s)");
ylabel("Acceleration (g) ");
legend("Y acceleration");


%////////////// altitude and apogee plot /////////////////////

