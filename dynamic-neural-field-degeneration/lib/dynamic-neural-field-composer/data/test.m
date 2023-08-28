filename = 'per - dec_perceptual field.txt';  % Replace with your filename
%filename = 'u - v_temp_input.txt';  % Replace with your filename
fieldSize = 360;

% Read the file
fid = fopen(filename, 'r');
data = textscan(fid, '%f');  % Read the numeric data
fclose(fid);

% Reshape the data into a matrix with each line as a separate row
numLines = numel(data{1}) / fieldSize;  % Assuming each line has 3 elements, adjust accordingly
dataMatrix = reshape(data{1}, fieldSize, numLines)';

% Plot each line
figure;
hold on;
for i = 1:size(dataMatrix, 1)
    plot(dataMatrix(i, :));
end
hold off;

% Add labels and title
xlabel('Time');
ylabel('Value');
title('Data Plot');
