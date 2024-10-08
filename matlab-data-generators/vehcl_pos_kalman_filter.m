function vehcl_pos_kalman_filter(duration, dt)

    %{
    this script implements a kalman filter to estimate the positon and
    velocity of a car moving along a road 

    INPUTS
    duration = length of simulation 
    dt = step size (seconds)

    %}

    measnoise = 10;
    accelnoise = 0.2;

    a = [1 dt; 0 1;]; % transition matrix
    b = [dt^2/2; dt]; % input matrix
    c = [1 0]; % measurement matrix
    x = [0;0]; % initial state vector 
    xhat = x; % initial state estimate

    Sz = measnoise^2; % measurement error covariance
    Sw = accelnoise^2 * [dt^4/4 dt^3/2; dt^3/2 dt^2]; % process noise covariance
    P = Sw; % initial estimation covariance 

    % initialize arrays for later plotting 
    pos = []; % true position array
    poshat = []; % estimated position array
    posmeas = []; % measured position array
    vel = []; % true velocity array
    velhat = []; % estimated velocity array 

    for t=0: dt:duration
        % use a constant commanded acceeration o 1 foot/second^2
        u = 1;
        % simulate the linear system 
        ProcessNoise = accelnoise * [(dt^2/2)*randn; dt*randn];
        x = a*x + b*u + ProcessNoise; 

        % simulate the noisy measurement 
        MeasNoise = measnoise * randn;
        y = c*x + MeasNoise;

        %extrapolate the most recent state estimate to the present time 
        xhat = a*xhat + b*u;

        % form the innovation vector 
        Inn = y - c * xhat;

        % compute the covariance of the innovation 
        s = c*P*c' + Sz;

        % form the Kalman gain matrix
        K = a*P*c'*inv(s);

        % update the state estimate
        xhat = xhat + K*Inn;

        % compute the covariance of the estimation error
        P = a*P*a'-a*P*c'*inv(s)*c*P*a'+Sw;

        % save parameters for plotting later
        pos = [pos; x(1)];
        posmeas = [posmeas; y];
        poshat = [poshat; xhat(1)];
        vel = [vel; x(2)];
        velhat = [velhat; xhat(2)];


    end

    % plot the results 
    close all;
    t = 0:dt:duration;
    
    % position
    figure;
    plot(t, pos, t, posmeas, t,poshat);
    legend("True Pos", "Measured Pos", "Estimated Pos");
    grid on;
    xlabel('Time(sec)');
    ylabel('Position (feet)');
    title('Figure 1 - Vehicle position (True, measured and estimated)');
    
    % velocity
    figure;
    plot(t, vel, t, velhat);
    grid on;
    xlabel('Time (sec)');
    ylabel('Velocity (feet/sec)');
    legend('Measured velocity', 'Estimated velocity');
    title('Figure 2- Vehicle velocity(true and estimated)');

end












