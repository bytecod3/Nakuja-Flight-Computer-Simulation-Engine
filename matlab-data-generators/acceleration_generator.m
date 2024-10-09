rise_time = 4; % 4 seconds
drop_time = 4; % 4 seconds curve drop time 

const_time = 6; % 6 seconds const time 

samples = 1000;
ts =1/samples;

%%%%%%%%%%%%%%%%%%%%%% GENERATE Y ACCELLERATION %%%%%%%%%%%%%%%%%%%%
% powered flight acceleration
t_rise_vector = 0:ts:rise_time;
t_rise_vector = t_rise_vector(:); % change to a column vector
rise_acc_vector = t_rise_vector .^2;

% descent stage acceleration
t_drop_vector = drop_time:-ts:0;
t_drop_vector = t_drop_vector(:);
drop_acc_vector = t_drop_vector .^ 2;
t_drop = 4:ts:8;

% free fall - zeroa acceleration
const_time_vector = 8:ts:12;
const_time_vector = const_time_vector(:);
const_acc_vector = 0 .^ const_time_vector;

% save this data to csv file 
filename_y = 'acc_y.csv';
csvwrite(filename_y,rise_acc_vector);
dlmwrite(filename_y,drop_acc_vector, '-append');
dlmwrite(filename_y,const_acc_vector, '-append');

%%%%%%%%%%%%%%%%%%%%%% GENERATE X ACCELLERATION %%%%%%%%%%%%%%%%%%%%

%{
If the MPU6050 is mounted vertically, our acceleration of interest is 
the y acceleration.
X acceleration remains approximately zero throughout unless at apogee 
where it changes BUT we cannot predict how the rocket orients iteself

%}

% powered flight acceleration
t_rise_vector = 0:ts:rise_time;
t_rise_vector = t_rise_vector(:); % change to a column vector
r = rand(length(t_rise_vector), 1) / 4; % very small x acceleration
x_rise_acc_vector = r .^2;

% t_rise_vector = 0:ts:rise_time;
% t_rise_vector = t_rise_vector(:); % change to a column vector
% r = rand(length(t_rise_vector), 1) / 4; % very small x acceleration
% rise_acc_vector = r .^2;
% 

% x_acc_time_vector = 0:ts:12;
% x_acc_time_vector = x_acc_time_vector(:);
% 
% m = rand(length(x_acc_time_vector),1) / 4; % very small values of x acceleration
% x_acc_vector = 0.2 .^ m;
% 
% disp(x_acc_vector);


acc_y = csvread('acc_y.csv');
t = 0:ts:12;
plot(t,acc_y(3:end)); % slice the matrix to make elements in acc_y equal to t vector
hold on;
plot(t_rise_vector, x_rise_acc_vector)

grid on;
