% This program graphs simulated doppler values
close all;
clear;
clc;

% Constants
EARTH_GRAV = 3.986008e14;
EARTH_ROT = 7.292115167e-5;

% Open file
fileID = fopen('zeck205h.18n', 'r');

% Read past header
for i = 1:7
    fgetl(fileID);
end

% Read in ephemeris
formatSpec = [ '%d %d %d %d %d %d %f %f %f %f\n' ...
    '%f %f %f %f\n' ...
    '%f %f %f %f\n' ...
    '%f %f %f %f\n' ...
    '%f %f %f %f\n' ...
    '%f %f %f %f\n' ...
    '%f %f %f %f\n' ...
    '%f\n' ];

A = fscanf(fileID, formatSpec, [35, 34]);

figure('units','normalized','outerposition',[0 0 1 1]);

% Create a 1-day sized array to store doppler values
dops = zeros(81600, 4);


% Start sat loop
for i = 1:34
    
    % ----- Parse ephemeris -----
    PRN =           A(1, i);
    svClkBias =     A(8, i);
    svClkDft =      A(9, i);
    svClkDftRt =    A(10, i);
    
    % Broadcast Orbit - 1
    IODE =          A(11, i);
    crs =           A(12, i);
    dn =            A(13, i);
    m0 =            A(14, i);
    
    % Broadcast Orbit - 2
    cuc =           A(15, i);
    ecc =           A(16, i);
    cus =           A(17, i);
    sqrta =         A(18, i);
    
    % Broadcast Orbit - 3
    t =             A(19, i);
    cic =           A(20, i);
    oldOmega =         A(21, i);
    cis =           A(22, i);
    
    % Broadcast Orbit - 4
    oldInc =        A(23, i);
    crc =           A(24, i);
    w =             A(25, i);
    oldOmegaDot =   A(26, i);
    
    % Broadcast Orbit - 5
    IDOT =          A(27, i);
    
    % ----- Calculate doppler -----
    % Loop through the velocities
    for frame = 1:4
        
        % ----- Initialization -----
        % Time from start to calculate doppler
        dt = 0;
        
        
        % Start position/velocity
        recieverPos = [6378100, 0, 0];
        recieverVel = [0, 0, 0];
        recieverSpd = (frame - 1) * 10;
        angleSpd = recieverSpd * 2 * pi / (40.075e6); % rad/sec
        
        % Loop through each second
        for sec = 1:81600
            % Increment vars
            dt = dt + 1;
            angle = angleSpd * dt;
            recieverPos = [cos(angle), 0, sin(angle)] * 6378100;
            recieverVel = [-sin(angle), 0, cos(angle)] * recieverSpd;
            
            % Semimajor axis
            a = sqrta ^ 2;
            
            % Calculate mean motion
            n = sqrt(EARTH_GRAV / (a ^ 3)) + dn;
            
            % Calculate mean anomoly
            M = m0 + n * dt;
            
            % Calculate eccentric anomaly using Newton-Raphson
            E = M;
            count = 0;
            isStarted = 0;
            
            while isStarted == 0 || abs(E - EOld) > 1.0E-14
                isStarted = 1;
                EOld = E;
                temp1 = 1.0 - ecc * cos(EOld);
                E = E + (M - EOld + ecc * sin(EOld)) / temp1;
                count = count + 1;
                if count > 5
                    break;
                end
            end
            
            EDot = n / temp1;
            
            % Begin calc for True anomaly and Argument of latitude
            temp2 = sqrt(1.0 - (ecc ^ 2));
            phi = atan2(temp2 * sin(E), cos(E) - ecc) + w;
            phiDot = temp2 * EDot / temp1;
            
            % Calculate corrected argument of latitude based on position
            mu = phi + cus * sin(2.0 * phi) + cuc * cos(2.0 * phi);
            muDot = phiDot * (1.0 + 2.0 * (cus * cos(2.0 * phi) - cuc * sin(2.0 * phi)));
            
            % Calculate corrected radius based on argument of latitude
            rad = a * temp1 + crc * cos(2.0 * phi) + crs * sin(2.0 * phi);
            radDot = a * ecc * sin(E) * EDot + 2.0 * phiDot * (crs * cos(2.0 * phi) - crc * sin(2.0 * phi));
            
            % Calculate inclination based on argument of latitude
            inc = oldInc + IDOT * dt + cic * cos(2.0 * phi) + cis * sin(2.0 * phi);
            incDot = IDOT + 2 * phiDot * (cis * cos(2.0 * phi) - cic * sin(2.0 * phi));
            
            % Calculate position and velocity in orbital plane
            orb = [rad * cos(mu), rad * sin(mu)];
            orbDot = [radDot * cos(mu) - orb(2) * muDot, radDot * sin(mu) + orb(1) * muDot];
            
            % Corrected longitude of ascending node
            omegaDot = oldOmegaDot - EARTH_ROT;
            omega = oldOmega + dt * omegaDot - EARTH_ROT * t;
            
            % Calculate coordinates
            pos = [orb(1) * cos(omega) - orb(2) * cos(inc) * sin(omega), ...
                orb(1) * sin(omega) + orb(2) * cos(inc) * cos(omega), ...
                orb(2) * sin(inc)];
            
            % Calculate velocity
            temp1 = orbDot(2) * cos(inc) - orb(2) * sin(inc) * incDot;
            vel = [-omegaDot * pos(2) + orbDot(1) * cos(omega) - temp1 * sin(omega), ...
                omegaDot * pos(1) + orbDot(1) * sin(omega) + temp1 * cos(omega), ...
                orb(2) * cos(inc) * incDot + orbDot(2) * sin(inc)];
            
            % Doppler
            relativePos = recieverPos - pos;
            relativeVel = recieverVel - vel;
            relativeSpd = relativeVel * relativePos' / norm(relativePos);
            
            doppler = -1.57542e9 * relativeSpd / 299792458.0;
            
%             if (recieverPos * pos') / (norm(recieverPos) * norm(pos)) < 0.475
%                 doppler = 0;
%             end
            
            dops(sec, frame) = doppler;
        end
    end
    % ----- Plotting -----
    clf;
    hold on;
    plot(1:81600, dops(:,1), '.', 'MarkerSize', 10, 'color', 'r');
    plot(1:81600, dops(:,2), '.', 'MarkerSize', 10, 'color', 'g');
    plot(1:81600, dops(:,3), '.', 'MarkerSize', 10, 'color', 'b');
    plot(1:81600, dops(:,4), '.', 'MarkerSize', 10, 'color', 'm');
    
    title(strcat('G', num2str(PRN), ': Doppler Simulation Going North from Equator'));
    xlabel('Time from start (sec)');
    ylabel('Doppler (Hz)');
    legend('Stationary', '10 m/s North', '20 m/s North', '30 m/s North');
    grid on;
    
    saveas(gcf, strcat('G', num2str(PRN), '.png'));
end
