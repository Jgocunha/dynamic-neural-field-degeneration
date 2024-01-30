% function selectedData = select_data_plots(data, offset)
%     % Get the total number of rows in the data
%     totalRows = size(data, 1);
% 
%     % Check if there are at least 5 rows in the data
%     if totalRows >= 5
%         % Generate a random permutation of indices
%         randomIndices = randperm(totalRows);
% 
%         % Select the first 5 rows based on the random indices
%         selectedData = data(randomIndices(1:5), :);
%     else
%         % If there are less than 5 rows, return the entire data
%         selectedData = data;
%     end
% end


function selectedData = select_data_plots(data, offset)
    % Check if the input data is a cell array
    if ~iscell(data)
        error('Input data must be a cell array.');
    end

    % Initialize an empty cell array for selected data
    %selectedData = cell(1, 5);
    selectedIndex = 1;

    % Iterate over all lines in the data
    for i = 1:length(data)
        % Get the current line
        currentLine = data{i};

        % Get the last value of the current line
        %lastValue = currentLine(end);

        % Get the last value of the current line
        maxValue = max(currentLine);

        % Get the last value of the current line
        minValue = min(currentLine);

        % Get initial value
        initialValue = currentLine(1);

        % Check if the last value has a sufficient deviation
        if (abs(minValue - initialValue) > offset) || (abs(maxValue - initialValue) > offset)
            % If yes, add the line to the selected data
            selectedData{selectedIndex} = currentLine;
            selectedIndex = selectedIndex + 1;

            % Break the loop if 5 lines are selected
            if selectedIndex > 5
                break;
            end
        end
    end
end
