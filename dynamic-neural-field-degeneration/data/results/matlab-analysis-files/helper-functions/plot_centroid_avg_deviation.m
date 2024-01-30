function plot_centroid_avg_deviation(deviations, locationDirPath, figTitle, subTitle, targetCentroid, acceptableDeviation, numberOfElementsTotal, numberOfElementsDegeneratedPerIteration)

    %% Plot the std. dev. and avg.
    stdLine = std(deviations, 1); % Calculate the mean along the columns
    avgLine = mean(deviations, 1); % Calculate the std. dev. along the columns
    
    figure;
    maxSize = size(deviations, 2);
    maxSize = (maxSize * 100 / numberOfElementsTotal) * numberOfElementsDegeneratedPerIteration;
    xlim([0, maxSize]);

    [~, maxIndex] = max(stdLine);
    percentageValues = linspace(0, maxSize, maxIndex);  % Generate percentage values
    plot(percentageValues, stdLine(1:maxIndex), 'LineWidth', 2.5);
    hold on;
    plot(percentageValues, avgLine(1:maxIndex), 'LineWidth', 2.5);    

   % Add legend in the bottom-left corner
    legend('Standard Deviation', 'Average', 'Location', 'southeast');

    %% Plot the acceptable range
%     % Define the acceptable range values
%     acceptableRange = [0, 0 + acceptableDeviation];
%     % Create x-values for the shaded area
%     xArea = [0, maxSize, maxSize, 0];
%     % Create y-values for the shaded area
%     yArea = [acceptableRange(1), acceptableRange(1), acceptableRange(2), acceptableRange(2)];
%     % Plot the shaded area
%     fill(xArea, yArea, 'g', 'FaceAlpha', 0.3, 'EdgeColor', 'none');
    hold off;

    %% Plot parameters
    xlabel('Degeneration percentage');
    ylabel(sprintf('Average and standard deviation \n of output fields bump centroid across all trials'));
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
