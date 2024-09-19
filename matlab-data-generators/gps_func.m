lat = linspace(0, 5, 20);
long = linspace(35, 40, 20);
alt = linspace(0, 3000, 20);

% parabolic fucntion to define the rockets trajectory 
alt = 

plot3(lat, long, alt)
grid on;
legend("Rocket trajectory");
xlabel("Latitude (deg)");
ylabel("Longitude (deg)");
zlabel("Altitude (m)");
title("Rocket trajectory with GPS coordinates")