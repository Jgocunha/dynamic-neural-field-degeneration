function [avgCentroidValuePerTrial,avgCentroidValue] = getAvgCentroidValue(data)
% Determine the number of trials
numTrials = numel(data);
% Iterate over each trial
for trial = 1:numTrials
    % Store the number of iterations for the current trial
    avgCentroidValuePerTrial(trial) = mean(data{trial});
end
avgCentroidValue = mean(avgCentroidValuePerTrial);
end

