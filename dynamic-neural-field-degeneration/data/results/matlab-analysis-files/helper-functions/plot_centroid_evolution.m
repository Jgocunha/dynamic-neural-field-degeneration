function plot_centroid_evolution(lines, locationDirPath, figTitle, subTitle, targetCentroid, acceptableDeviation, numberOfElementsTotal, numberOfElementsDegeneratedPerIteration)
    
    figure;
    maxSize = max(cellfun(@numel, lines));
    maxSize = (maxSize * 100) / numberOfElementsTotal;
    xlim([0, maxSize]);

    %% Plot the acceptable range
    % Define the acceptable range values
    acceptableRange = [targetCentroid-acceptableDeviation, targetCentroid+acceptableDeviation];
    % Find the maximum size of the lines
    % Get the x-axis range
    xRange = 0:maxSize+1;
    % Create x-values for the shaded area
    xArea = [xRange, fliplr(xRange)];
    % Create y-values for the shaded area
    yArea = [acceptableRange(1)*ones(size(xRange)), fliplr(acceptableRange(2)*ones(size(xRange)))];
    % Plot the shaded area
    fill(xArea, yArea, 'g', 'FaceAlpha', 0.3, 'EdgeColor', 'none');
    hold on;


    %% Plot each trial with a different color
    
    for i = 1:numel(lines)
        percentageValues = linspace(0, maxSize, length(lines{i}));  % Generate percentage values
        plot(percentageValues, lines{i}, 'Color', rand(1, 3), 'LineWidth', 2.0);
    end
    hold off;


%     figure;
%     maxSize = size(lines, 2);
%     maxSize = (maxSize * 100 / numberOfElementsTotal) * numberOfElementsDegeneratedPerIteration;
%     xlim([0, maxSize]);
%     hold on;
%     for i = 1:size(lines, 1)
%         percentageValues = linspace(0, maxSize, size(lines,2));  % Generate percentage values
%         plot(percentageValues, lines(i, :), 'Color', rand(1, 3), 'LineWidth', 2.0);
%     end
    
   
    
    %% Plot parameters
    xlabel('Degeneration percentage');
    ylabel('Centroid position of the output field bump');
    fullTitle = sprintf('%s\n%s', figTitle, subTitle);
    title(fullTitle);
    
    grid on;

    % Set font style and size
    set(gca, 'FontName', 'Garamond');
    set(gca, 'FontSize', 12);
    
            
    %% Save the plot    
    % Remove any special characters from the title to create a valid file name
    filename = [figTitle, ' ', subTitle];
    valid_title = regexprep(filename, '[^\w\s]', '');
    valid_title = [locationDirPath, valid_title];
    % Set the desired resolution in DPI (e.g., 300)
    resolution = 300;
    % Set the figure size to match the desired resolution
    set(gcf, 'Units', 'pixels', 'Position', [100 100 1200 800]); % Adjust the figure size as needed
    % Save the plot as an image file with the desired resolution
    print([valid_title '.png'], '-dpng', ['-r' num2str(resolution)]);
    % Restore the original figure size (optional)
    set(gcf, 'Units', 'normalized');
end

