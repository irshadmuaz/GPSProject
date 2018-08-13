% This program plots data from the report
close all;
clear;
clc;

% File Specs
fileID = fopen('Report.txt', 'r');
formatSpec = '%d %d %f %f %f %d %d %d %d %d %f\n';
sizeA = [11 inf];

% Read in inputs
fgetl(fileID);
A = fscanf(fileID, formatSpec, sizeA);
A = sortrows(A', [1, 9, 10, 11]);
A = A';

% Calculate timeStamps
timeStamps = A(11, :) + 60 * (A(10, :) + 60 * A(9, :));
timeBegin = min(timeStamps);
corrTime = timeStamps - timeBegin;
maxTime = max(corrTime);

% Length of A
ASize = length(timeStamps);

% Set up loop vars
PRN = A(1, 1);
startIndex = 1;

figure('units','normalized','outerposition',[0 0 1 1]);

for i = 1:ASize
    if (PRN ~= A(1, i) || i == ASize)
        clf;
        
        % Satellite Name
        if (PRN >= 10)
            satName = strcat('G', num2str(PRN));
        else
            satName = strcat('G0', num2str(PRN));
        end
        
        % Differentials
        dCalc = diff(A(4, startIndex:i-1))./diff(corrTime(startIndex:i-1));
        %dMeas = diff(A(3, startIndex:i-1))./diff(corrTime(startIndex:i-1));
        Meas = smooth(A(3, startIndex:i-1), 600);
        dMeas = diff(Meas')./diff(corrTime(startIndex:i-1));
        
        % Predicted & Measured Doppler Plot
        subplot(3, 1, 1);
        
        hold on;  
        plot(corrTime(startIndex:i-1), A(3, startIndex:i-1), '.', ...
        'MarkerSize', 10, 'color', 'b');
        plot(corrTime(startIndex:i-1), A(4, startIndex:i-1), '.', ...
        'MarkerSize', 10, 'color', 'r');
        
        
    
        title(strcat(satName, ': Static and Dynamic (30 m/s North) Doppler Effects'));
        xlabel('Time from start (sec)');
        ylabel('Doppler (Hz)');
        legend({'Static Doppler', 'Dynamic Doppler'});
        grid on;
        %xlim([0,maxTime]);
        
        % Predicted & Measured Differential Plot
        subplot(3, 1, 2);
        
        hold on;   
        plot(corrTime(startIndex:i-1), A(3, startIndex:i-1) - A(4, startIndex:i-1), '.', ...
        'MarkerSize', 10, 'color', 'b');
        %plot(corrTime(startIndex:i-2), dCalc, '.', ...
        %'MarkerSize', 10, 'color', 'r');
    
        title(strcat(satName, ': Doppler Error'));
        xlabel('Time from start (sec)');
        ylabel('Doppler Error (Hz)');
        %legend({'Measured Differential', 'Predicted Differential'});
        grid on;
        %xlim([0,maxTime]);
        
        % Predicted - Measured Comparison Plot
%         subplot(3, 1, 2);
%         
%         plot(corrTime(startIndex:i-1), A(4, startIndex:i-1) - A(3, startIndex:i-1), ...
%         '.', 'MarkerSize', 10, 'color', 'b'); 
%         
%         title(strcat(satName, ': Doppler Effects Comparison'));
%         xlabel('Time from start (sec)'); 
%         ylabel('Difference = Predicted - Measured Doppler (Hz)');
%         grid on;
%         xlim([0,maxTime]);
%         ylim([50, 250]);

        % Differential Plot
         subplot(3, 1, 3);
%         
%         dy = diff(A(3, startIndex:i-1))./diff(corrTime(startIndex:i-1));
%         
%         plot(corrTime(startIndex:i-2), dy, '.', ...
%         'MarkerSize', 10, 'color', 'b');
%         
%         title(strcat(satName, ': Doppler Differential'));
%         xlabel('Time from start (sec)'); 
%         ylabel('Differential (Hz/Sec)');
%         grid on;
        %xlim([0,maxTime]);
        
        % SNR Plot
        plot(corrTime(startIndex:i-1), A(2, startIndex:i-1), '.', ...
        'MarkerSize', 10, 'color', 'b');
        
        title(strcat(satName, ': Signal to Noise Ratio'));
        xlabel('Time from start (sec)'); 
        ylabel('SNR (unitless)');
        grid on;
        xlim([0,maxTime]);

        startIndex = i;
        
        saveas(gcf, strcat(satName, '.png'));
    end
    PRN = A(1, i);
end





