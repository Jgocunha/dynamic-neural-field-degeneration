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


function selectedData = select_data_plots_evolution(data, expectedCentroid)
    % Check if the input data is a cell array
    if ~iscell(data)
        error('Input data must be a cell array.');
    end

    value = expectedCentroid;
    dataIndex = 1;
    
    % Iterate through each row
    for i = 1:numel(data)
        % Get the last value of the current row
        lastValue = data{i}(end);

        if lastValue < value
            value = lastValue;
            selectedData{dataIndex} = data{i};
            dataIndex = dataIndex + 1;
            if(dataIndex > 3)
                break;
            end
        end
    end

    value = expectedCentroid;
    
    % Iterate through each row
    for i = 1:numel(data)
        % Get the last value of the current row
        lastValue = data{i}(end);

        if lastValue > value
            value = lastValue;
            selectedData{dataIndex} = data{i};
            dataIndex = dataIndex + 1;
            if(dataIndex > 5)
                break;
            end
        end
    end
end


