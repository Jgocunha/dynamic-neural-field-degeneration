clear;
clc;
close all;

% File paths
matrixFile = 'per - dec_weights.txt';
perceptualFieldFile = 'per - dec_perceptual field.txt';
outputFieldFile = 'per - dec_decision field.txt';

fieldSize_perceptual = 360;
fieldSize_decision = 28;

% Read matrix and arrays
matrix = load(matrixFile);
perceptualField = load(perceptualFieldFile);
outputField = load(outputFieldFile);

% Plot the matrix
figure;
imagesc(matrix);
%colormap('viridis');
colorbar;
title('Matrix Plot');

% Read the file
fid = fopen(perceptualFieldFile, 'r');
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
title('per-out perceptual field Plot');

% Read the file
fid = fopen(outputFieldFile, 'r');
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