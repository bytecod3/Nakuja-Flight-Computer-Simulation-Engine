t = linspace(0, 36000);
tmin = 50;
xmin = 16;
xmax = 45;
k = -log(xmin/xmax)/(60*tmin);
T = xmax * exp(-k*t);

plot(t/60, T, 'linewidth', 1.5);
grid on;
