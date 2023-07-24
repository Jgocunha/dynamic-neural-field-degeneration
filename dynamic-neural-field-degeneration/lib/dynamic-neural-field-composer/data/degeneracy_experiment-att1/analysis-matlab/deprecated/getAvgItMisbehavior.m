function [averageIterations] = getAvgItMisbehavior(lines, numTrials, targetCentroid, acceptableDeviation)
% Define the threshold values
lowerThreshold = targetCentroid - acceptableDeviation;
upperThreshold = targetCentroid + acceptableDeviation;

% Find the first occurrence of values below the lower threshold or above the upper threshold
iterations = zeros(numTrials, 1);

for i = 1:numTrials
    line = lines{i};
    indices = find(line < lowerThreshold | line > upperThreshold);
    
    if ~isempty(indices)
        iterations(i) = indices(1);
    else
        iterations(i) = numel(line) + 1;
    end
end

% Calculate the average number of iterations
averageIterations = mean(iterations);
end

