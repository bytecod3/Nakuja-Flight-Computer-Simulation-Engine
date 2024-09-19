% t = 0:0.1:10*pi;
% x = cos(t);
% y = sin(t);
% z = sqrt(x.^2-y.^2);
% plot3(x, y,z);
% grid on;

x = 0:0.1:20;
y = 0:0.1:20;
z = -0.25 .* x.^2 + 1500 .* x;

plot3(x, y,z);
grid on;