function [maxDev,minDev] = getMaxMinDev(lines, targetCentroid)
% Concatenate vectors of different dimensions into a single vector
numLines = numel(lines);
maxSize = max(cellfun(@numel, lines));
allElements = NaN(maxSize, numLines);

for i = 1:numLines
    line = lines{i};
    allElements(1:numel(line), i) = line;
end

% Reshape into a column vector
allElements = reshape(allElements, [], 1);

% Find the maximum and minimum of all the elements
maxDev = max(allElements) - targetCentroid;
minDev = min(allElements) - targetCentroid;
end

