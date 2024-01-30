function [trialDeviations,maxDeviations] = get_max_deviations(data, targetCentroid)
% Determine the number of trials
numTrials = numel(data);

% Initialize variables to store the maximum deviations
trialDeviations = zeros(numTrials, 1);

% Iterate over each trial
for trial = 1:numTrials
    % Get the actual number of iterations for the current trial
    numIterations = numel(data{trial});
    
    % Initialize the maximum deviation for the current trial
    maxDeviation = 0;
    
    % Iterate over each iteration
    for iteration = 1:numIterations
        % Calculate the deviation from the target centroid
        %deviation = abs(data{trial}(iteration) - targetCentroid);
        deviation = min(abs(data{trial}(iteration) - targetCentroid), abs(28 - (abs(data{trial}(iteration) + targetCentroid))));

        % Update the maximum deviation if the current deviation is higher
        if deviation > maxDeviation
            maxDeviation = deviation;
        end

    end
    
    % Store the maximum deviation for the current trial
    trialDeviations(trial) = maxDeviation;

% Find the overall maximum deviation for the current location
maxDeviations = max(trialDeviations);
end

