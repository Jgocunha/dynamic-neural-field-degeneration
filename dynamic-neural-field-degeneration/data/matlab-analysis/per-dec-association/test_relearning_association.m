clear;
clc;

path = '../../';
filename_perceptual = [path, 'per - dec_perceptual field.txt'];  
filename_decision = [path, 'per - dec_decision field.txt'];
fieldSize_perceptual = 360;
fieldSize_decision = 180;

% Read the file
fid = fopen(filename_perceptual, 'r');
data = textscan(fid, '%f');  % Read the numeric data
fclose(fid);

% Reshape the data into a matrix with each line as a separate row
numLines = numel(data{1}) / fieldSize_perceptual;  
dataMatrix = reshape(data{1}, fieldSize_perceptual, numLines)';

% Plot each line
figure;
hold on;
for i = 1:size(dataMatrix, 1)
    plot(dataMatrix(i, :));
end
hold off;

% Add labels and title
xlabel('Neural position');
ylabel('Activation');
title('per-dec perceptual field Plot');

% Read the file
fid = fopen(filename_decision, 'r');
data = textscan(fid, '%f');  % Read the numeric data
fclose(fid);

% Reshape the data into a matrix with each line as a separate row
numLines = numel(data{1}) / fieldSize_decision; 
dataMatrix = reshape(data{1}, fieldSize_decision, numLines)';

% Plot each line
figure;
hold on;
for i = 1:size(dataMatrix, 1)
    plot(dataMatrix(i, :));
end
hold off;

% Add labels and title
xlabel('Neural position');
ylabel('Activation');
title('per-dec decision field Plot');