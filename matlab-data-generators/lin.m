rise_time = 4; % 4 seconds
drop_time = 4; % 4 seconds curve drop time 

const_time = 6; % 6 seconds const time 

samples = 1000;
ts =1/samples;

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
csvwrite('acc_y.csv',rise_acc_vector);
dlmwrite('acc_y.csv',drop_acc_vector, '-append');
dlmwrite('acc_y.csv',const_acc_vector, '-append');

%plot(b);
%hold on;
%disp(out);

acc_y = csvread('acc_y.csv');
plot(acc_y);

grid on;
