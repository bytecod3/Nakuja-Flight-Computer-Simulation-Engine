% generate data to test ring buffer 
x = (-10:0.1:10)';
y = (36-abs(x).^2)/9;

figure
plot(x,y)
title( 'Parable' )