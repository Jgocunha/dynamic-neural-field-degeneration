function [aboveOrBelowThreshold,avgIterations] = getIterationsUntilMisbehavior(data,targetCentroid, acceptableDeviation)
% Determine the number of trials
numTrials = numel(data);

% Initialize variables to store the iteration indices
aboveOrBelowThreshold = zeros(1, 1);

% Iterate over each trial
for trial = 1:numTrials
    % Get the actual number of iterations for the current trial
    numIterations = numel(data{trial});
    
    % Iterate over each iteration
    for iteration = 1:numIterations
        % Check if the centroid value exceeds or falls below the threshold
        deviation = min(abs(data{trial}(iteration) - targetCentroid), abs(28 - (abs(data{trial}(iteration) + targetCentroid))));
        if deviation >= acceptableDeviation
            aboveOrBelowThreshold(trial) = iteration;
            break;  % Exit the loop if the condition is met
        else
            aboveOrBelowThreshold(trial) = iteration;
        end
    end

end

% Calculate the average number of iterations
avgIterations = mean(aboveOrBelowThreshold);

end

