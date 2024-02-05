function result = categorize_centroids(data, expectedValue, acceptableDeviation)
    result = cell(size(data));

    for i = 1:length(data)
        currentArray = data{i};

        for j = 1:length(currentArray)
            deviation = currentArray(j) - expectedValue;

            if currentArray(j) == 0
                result{i}(j) = 2;  % Value is 0
            elseif deviation <= acceptableDeviation
                result{i}(j) = 0;  % Value is within deviation
            else
                result{i}(j) = 1;  % Value is above deviation
            end
        end
    end
end
