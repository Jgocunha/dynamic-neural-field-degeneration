function max_length = find_max_length(data)
    max_length = 0;
    for i = 1:numel(data)
        current_length = length(data{i});
        if current_length > max_length
            max_length = current_length;
        end
    end
end
