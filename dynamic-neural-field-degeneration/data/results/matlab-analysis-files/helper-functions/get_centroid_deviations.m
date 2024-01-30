function [deviations] = get_centroid_deviations(deviationsFileName, data, targetCentroid)
    % Initialize a matrix to store deviations for each value
    deviations = zeros(numel(data), max(cellfun(@numel, data)));

    % Calculate deviations for each trial and iteration
    for trial = 1:numel(data)
        for iteration = 1:numel(data{trial})
            deviations(trial, iteration) = min(abs(data{trial}(iteration) - targetCentroid), abs(28 - abs(data{trial}(iteration) + targetCentroid)));
        end
    end

    % Save deviations in .txt file
    path = './analysis/centroid-deviations/';

    % Create the full path for the text file
    filename = fullfile(path, ['deviations_' , deviationsFileName , '.txt']);

    try
        % Open the text file for writing
        fileID = fopen(filename, 'w');

        % Loop through each row and find the last non-zero element
        for row = 1:size(deviations, 1)
            lastNonZeroIndex = find(deviations(row, :), 1, 'last');
            
            % Write only the non-zero part of the row to the text file
            fprintf(fileID, '%s\n', num2str(deviations(row, 1:lastNonZeroIndex)));
        end

        % Close the text file
        fclose(fileID);

    catch
        % Display an error message if there's an issue with file operations
        disp('Error occurred while writing to the file.');
        if exist('fileID', 'var') && fileID ~= -1
            fclose(fileID);
        end
    end
end
