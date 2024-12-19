rawtable = readtable('flight_log.xlsx');

x = rawtable.Time;
y = rawtable.Altitude;

p = rawtable.Pressure;

figure;
plot(x, y);
title("Altitude curve");
xlabel("Time(sec)");
ylabel("Altitude (m)");
grid on;

figure;
plot(x, p); % pressure
title("Pressure curve");
xlabel("Time(sec)");
ylabel("Pressure");