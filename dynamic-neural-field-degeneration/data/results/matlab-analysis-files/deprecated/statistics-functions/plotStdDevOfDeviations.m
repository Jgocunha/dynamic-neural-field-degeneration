function plotStdDevOfDeviations(lines, locationDirPath, figTitle, subTitle, targetCentroid, acceptableDeviation)
    % Initialize a matrix to store deviations for each value
    deviations = zeros(numel(lines), max(cellfun(@numel, lines)));

    % Calculate deviations for each trial and iteration
    for trial = 1:numel(lines)
        for iteration = 1:numel(lines{trial})
            deviations(trial, iteration) = min(abs(lines{trial}(iteration) - targetCentroid), abs(28 - abs(lines{trial}(iteration) + targetCentroid)));
        end
    end

    stdLine = std(deviations, 1); % Calculate the mean along the columns
    avgLine = mean(deviations, 1); % Calculate the mean along the columns

    % Plot the deviations
    figure;
    plot(stdLine', 'LineWidth', 1.5);
    hold on;
    plot(avgLine', 'LineWidth', 1.5);

    hold off;
    
    xlabel('Iteration');
    ylabel('Deviation');
    fullTitle = sprintf('%s\n%s', figTitle, subTitle);
    title(fullTitle);

    grid on;

    % Set font style and size
    set(gca, 'FontName', 'Garamond');
    set(gca, 'FontSize', 12);

    % Save the plot
    filename = [figTitle, ' ', subTitle, '_deviations'];
    valid_title = regexprep(filename, '[^\w\s]', '');
    valid_title = [locationDirPath, valid_title];

    resolution = 1000;
    %print([valid_title '.png'], '-dpng', ['-r' num2str(resolution)]);
end

