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

function [v_powered, v_coast] = GenerateV(v_initial, g, t_flight)
    
    % generating time vectors 
    t = 0:1:t_flight;
    powered_flight_time = 6; % time taken for powered flight
    coasting_flight_time = 4; % time taken for coasting

    p_flight_vec = 0:1:powered_flight_time;
    c_flight_vec = powered_flight_time:1:t_flight;

    % velocity equation 
    % calculate velocity for powered flight. The acceleration due to gravity is +ve
    v_powered = v_initial + g .* p_flight_vec; 

    % calculate velocity for coasting. The acceleration due to gravity is -ve
    % velocity for coasting starts at max velocity for powered 
    v_coast = v_powered(end) + (-g) .* p_flight_vec;
    
end 