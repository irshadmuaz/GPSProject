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
        
        % Predicted & Measured Doppler Plot
        subplot(3, 1, 1);
        
        hold on;        
        plot(corrTime(startIndex:i-1), A(3, startIndex:i-1), '.', ...
        'MarkerSize', 10, 'color', 'b');
        plot(corrTime(startIndex:i-1), A(4, startIndex:i-1), '.', ...
        'MarkerSize', 10, 'color', 'r');
    
        title(strcat(satName, ': Measured and Predicted Doppler Effects'));
        xlabel('Time from start (sec)');
        ylabel('Doppler (Hz)');
        legend({'Measured Doppler', 'Predicted Doppler'});
        grid on;
        
        % Predicted - Measured Comparison Plot
        subplot(3, 1, 2);
        
        plot(corrTime(startIndex:i-1), A(4, startIndex:i-1) - A(3, startIndex:i-1), ...
        '.', 'MarkerSize', 10, 'color', 'b'); 
        
        title(strcat(satName, ': Doppler Effects Comparison'));
        xlabel('Time from start (sec)'); 
        ylabel('Difference = Predicted - Measured Doppler (Hz)');
        grid on;

        % SNR Plot
        subplot(3, 1, 3);
        
        plot(corrTime(startIndex:i-1), A(2, startIndex:i-1), '.', ...
        'MarkerSize', 10, 'color', 'b');
        
        title(strcat(satName, ': Signal to Noise Ratio'));
        xlabel('Time from start (sec)'); 
        ylabel('SNR (unitless)');
        grid on;

        startIndex = i;
        
        saveas(gcf, strcat(satName, '.png'));
    end
    PRN = A(1, i);
end





