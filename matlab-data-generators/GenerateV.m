%% GenerateVelocity.m
% Generates velocity curve to mimic the rocket's velocity trajectory
%
% 
% Inputs
% v_initial The initial velocity of the rocket 
% g Acceleration due to gravity
% t_flight Time of flight
% 
% Outputs
% v_powered Velocity at powered flight stage
% v_coast Velocity at coasting flight stage
%

function [v_powered, v_coast, negative_velocity, terminal_velocity] = GenerateV(v_initial, g, t_flight)
    
    % generating time vectors 
    t = 0:1:t_flight;
    powered_flight_time = 6; % time taken for powered flight
    coasting_flight_time = 4; % time taken for coasting

    p_flight_vec = 0:1:powered_flight_time;
    c_flight_vec = powered_flight_time:1:t_flight;

    % velocity equation 
    % calculate velocity for powered flight. The acceleration due to gravity is +ve
    v_powered = v_initial + g * p_flight_vec; 

    % calculate velocity for coasting. The acceleration due to gravity is -ve
    % velocity for coasting starts at max velocity for powered 
    v_coast = v_powered(end) + (-g) * p_flight_vec;
    
    % generate the acceleration downwards after apogee
    % 12 to 18 in steps of 1
    x = 12:1:18;

    % ideal accelration in the -ve direction from apogee
    y = x .* 2;
    negative_velocity = -1 * y;
    
    % terminal velocity
    % terminal velocity is a constant 
    x = 18:1:24;
    y_terminal = ones(length(x));
    terminal_velocity = y(end) .* y_terminal;

    
end 