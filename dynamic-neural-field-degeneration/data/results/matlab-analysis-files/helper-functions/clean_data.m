function [data] = clean_data(data)
    % Remove negative values from the cell array
    newData = {};  % Initialize a new cell array to store filtered values
    for i = 1:numel(data)
        if isnumeric(data{i})
            if data{i} >= 0
                % Keep only non-negative numeric values
                newData{end+1} = data{i};
            end
        elseif islogical(data{i}) || ischar(data{i})
            % Keep logical and char values as-is
            newData{end+1} = data{i};
        end
        % You can add more data type checks as needed
    end
    data = newData;  % Replace the original data with the filtered data
end

