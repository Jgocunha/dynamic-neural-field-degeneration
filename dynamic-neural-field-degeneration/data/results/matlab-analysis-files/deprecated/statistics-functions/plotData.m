function plotData(lines, locationDirPath, figTitle, subTitle, targetCentroid, acceptableDeviation)
% Plot each trial with a different color
figure;
hold on;
for i = 1:numel(lines)
    plot(lines{i}, 'Color', rand(1, 3), 'LineWidth', 1.5);
end

% Define the acceptable range values
acceptableRange = [targetCentroid-acceptableDeviation, targetCentroid+acceptableDeviation];

% Find the maximum size of the lines
maxSize = max(cellfun(@numel, lines));
% Get the x-axis range
xRange = 1:maxSize;

% Create x-values for the shaded area
xArea = [xRange, fliplr(xRange)];

% Create y-values for the shaded area
yArea = [acceptableRange(1)*ones(size(xRange)), fliplr(acceptableRange(2)*ones(size(xRange)))];

% Plot the shaded area
fill(xArea, yArea, 'g', 'FaceAlpha', 0.3, 'EdgeColor', 'none');
hold off;

xlabel('Degeneration iterations');
ylabel('Centroid value of the field');
fullTitle = sprintf('%s\n%s', figTitle, subTitle);
title(fullTitle);

grid on;

% Create a custom legend for the plot
%legend('Trials', 'Acceptable Range');


% % Adjust the axes limits
% data_size = size(data(1, :));
% min_data_value = min(data(1, :));
% max_data_value = max(data(1, :));
% xlim([0, data_size(2)]);
% ylim([min_data_value, max_data_value]);
% 
% Set font style and size
set(gca, 'FontName', 'Garamond');
set(gca, 'FontSize', 12);
% 
% % Adjust the tick marks
% %xticks(0:2:10);
% %yticks(-1:0.5:1);

        
%% Save the plot
%current_title = get(gca, 'Title').String;

% Remove any special characters from the title to create a valid file name
filename = [figTitle, ' ', subTitle];
valid_title = regexprep(filename, '[^\w\s]', '');
valid_title = [locationDirPath, valid_title];

% Set the desired resolution in DPI (e.g., 300)
resolution = 1000;

% Set the figure size to match the desired resolution
%set(gcf, 'Units', 'pixels', 'Position', [100 100 1200 800]); % Adjust the figure size as needed

% Save the plot as an image file with the desired resolution
%print([valid_title '.png'], '-dpng', ['-r' num2str(resolution)]);

% Restore the original figure size (optional)
%set(gcf, 'Units', 'normalized');
end

