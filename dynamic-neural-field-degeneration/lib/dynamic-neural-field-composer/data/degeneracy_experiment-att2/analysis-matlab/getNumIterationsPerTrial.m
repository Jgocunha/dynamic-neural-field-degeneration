function [numIterationsPerTrial,avgNumIterations] = getNumIterationsPerTrial(data)
% Determine the number of trials
numTrials = numel(data);
% Iterate over each trial
for trial = 1:numTrials
    % Get the actual number of iterations for the current trial
    numIterations = numel(data{trial});
    % Store the number of iterations for the current trial
    numIterationsPerTrial(trial) = numIterations;
end
avgNumIterations = mean(numIterationsPerTrial);
end

