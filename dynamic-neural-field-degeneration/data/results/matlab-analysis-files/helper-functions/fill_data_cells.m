function [data] = fill_data_cells(data, desired_length)
    for i = 1:numel(data)
        current_array = data{i};
        current_length = max(length(current_array));
        if current_length < desired_length
            padding_length = desired_length - current_length;
            padding_values = repmat(2, 1, padding_length);
            data{i} = [current_array, padding_values];
        end
    end
end
