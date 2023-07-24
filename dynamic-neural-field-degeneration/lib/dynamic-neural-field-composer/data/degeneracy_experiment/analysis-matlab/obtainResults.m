%% Clear the MATLAB environment
clear;
clc;

%% Setup variables
experiments = {'WEIGHTS_DEACTIVATE', 'weights', 'iteration', 59.5; ...
               'WEIGHTS_REDUCE_0.400000', 'weights', 'iteration', 59.5; ...
               'WEIGHTS_REDUCE_0.600000', 'weights', 'iteration', 59.5; ...
               'WEIGHTS_REDUCE_0.800000', 'weights', 'iteration', 59.5; ...
               'WEIGHTS_RANDOMIZE', 'weights', 'iteration', 59.5; ...
               'NEURONS_DEACTIVATE_field u_field v', 'neurons', 'iteration', 59.5; ...
               'NEURONS_DEACTIVATE_field u_field u', 'neurons', 'iteration', 29.5; ...
               'NEURONS_DEACTIVATE_field v_field v', 'neurons', 'iteration', 59.5; ...
               };

experimentDirPath = '';
locationDirPath = '';
centroidFilePath = '';
resultsFilePath = '';
targetCentroid = 0;
acceptableDeviation = 0.5;

dataTable = table();

%% Run analysis
for experiment = 1:size(experiments,1)
    experimentDirPath = ['../', experiments{experiment}, '/'];    
    %% Variable setup
    locationDirPath = [experimentDirPath, 'p1/'];
    targetCentroid = experiments{experiment, 4};
    centroidFilePath = [locationDirPath, 'centroids.txt'];
    resultsFilePath = [locationDirPath, 'results.txt'];
    %delete(resultsFilePath)
    diary(resultsFilePath);

    %% Read data
    data = readData(centroidFilePath);

    %% Analyse data

    % get number of trials
    numTrials = numel(data);
    % get iterations until disapearance of peak
    [numIterationsPerTrial, avgNumIterations] = getNumIterationsPerTrial(data);
    % get iterations until misbehavior of peak
    [aboveOrBelowThreshold,avgIterationsMisbehavior] = getIterationsUntilMisbehavior(data,targetCentroid, acceptableDeviation);
    % get deviations, and maximum deviation
    [trialDeviations, maxDeviations] = getMaxDeviations(data, targetCentroid);
    % get average centroid value
    [avgCentroidValuePerTrial,avgCentroidValue] = getAvgCentroidValue(data);

    %% Plot data
    switch experiments{experiment, 1}
        case 'WEIGHTS_DEACTIVATE'
            title = ['Randomly deactivating one of the ' experiments{experiment, 2} ' (unique) in each iteration'];
        case 'WEIGHTS_DEACTIVATE_PERCENTAGE'
            title = ['Randomly deactivating 10% of the ' experiments{experiment, 2} ' (unique) in each iteration'];
        case 'WEIGHTS_REDUCE_0.400000' 
            title = ['Randomly reducing one of the ' experiments{experiment, 2} ' in each iteration (x0.4)'];
        case 'WEIGHTS_REDUCE_0.600000' 
            title = ['Randomly reducing one of the ' experiments{experiment, 2} ' in each iteration (x0.6)'];
        case 'WEIGHTS_REDUCE_0.800000' 
            title = ['Randomly reducing one of the ' experiments{experiment, 2} ' in each iteration (x0.8)'];
        case 'WEIGHTS_RANDOMIZE'
            title = ['Randomly randomizing one of the ' experiments{experiment, 2} ' in each iteration'];
        case 'NEURONS_DEACTIVATE_field u_field v'
            title = ['Deactivating one of the ' experiments{experiment, 2} ' (unique) in each iteration'];
        case 'NEURONS_DEACTIVATE_field u_field u'
            title = ['Deactivating one of the ' experiments{experiment, 2} ' (unique) in each iteration'];
        case 'NEURONS_DEACTIVATE_field v_field v'
            title = ['Deactivating one of the ' experiments{experiment, 2} ' (unique) in each iteration'];
    end
    subTitle = '';

% 
%     %plotDeviation(absDiff, locationDirPath, title, subTitle, acceptableDeviation);
%     %plotData(data, locationDirPath, title, subTitle, targetCentroid, acceptableDeviation);
% 
% 
%     avgValues = [];
%     % Calculate the average value for each iteration across all trials
%     % Convert absDiff to a numeric array
%     % Calculate the average value for each iteration within the trial
%     for i = 1:length(absDiff)
%         avgValues = [avgValues, mean(absDiff{i})];
%     end
%     %plotDeviation(avgValues, locationDirPath, title, subTitle, acceptableDeviation);
%     % Plot the averaged values for each iteration
%     figure;
%     plot(avgValues);
%     xlabel('Iteration');
%     ylabel('Average Value');
%     %title('Averaged Values for Each Iteration');


% % Calculate absolute difference between each value and the centroid value
% absDiff = cellfun(@(x) abs(x - avgCentroidValue), data, 'UniformOutput', false);
% 
% % Determine the maximum size of the elements in the cell array
% maxSize = max(cellfun(@numel, absDiff));
% 
% % Pad the smaller elements with NaNs to make them the same size
% absDiffPadded = cellfun(@(x) [x; NaN(maxSize - numel(x), 1)], absDiff, 'UniformOutput', false);
% 
% % Convert padded absDiff data to a matrix
% absDiffMat = cell2mat(absDiffPadded);
% 
% % Remove rows with all NaN values
% absDiffMat = absDiffMat(~all(isnan(absDiffMat), 2), :);
% 
% % Convert absDiffMat to a column vector
% absDiffVec = absDiffMat(:);
% 
% % Define the nonlinear model function
% modelFun = @(b, x) b(1) * exp(b(2) * x) + b(3);
% 
% % Create an initial parameter guess
% initialParams = [1, -0.1, 0];
% 
% % Perform the nonlinear regression using fitnlm
% mdl = fitnlm((1:numel(absDiffVec))', absDiffVec, modelFun, initialParams);
% 
% % Extract the fitted parameters
% fittedParams = mdl.Coefficients.Estimate;
% 
% % Evaluate the goodness of fit
% rsquared = mdl.Rsquared.Adjusted;
% 
% % Generate predicted values from the fitted model
% predictedValues = predict(mdl, (1:numel(absDiffVec))');
% 
% % Plot the actual data and the fitted curve
% figure;
% plot((1:numel(absDiffVec))', absDiffVec, 'bo', 'MarkerSize', 5); % Actual data
% hold on;
% plot((1:numel(absDiffVec))', predictedValues, 'r-', 'LineWidth', 2); % Fitted curve
% xlabel('Iteration');
% ylabel('absDiff');
% title('Nonlinear Regression of absDiff Data');
% legend('Actual Data', 'Fitted Curve');
% 
% % Display the fitted parameters and goodness of fit
% disp('Fitted Parameters:');
% disp(fittedParams);
% disp('Adjusted R-squared:');
% disp(rsquared);
% 




    %% Store data in table
    newRow = table(cellstr(experiments{experiment, 1}), ...
                   targetCentroid, ...
                   acceptableDeviation, ...
                   numTrials, ...
                   avgNumIterations, ...
                   avgIterationsMisbehavior(1), ...
                   maxDeviations, ...
                   'VariableNames', {'Experiment', ...
                                     'TargetCentroid', ...
                                     'AcceptableDeviation', ...
                                     'NumTrials', ...
                                     'AvgNumIterations', ...
                                     'AvgIterationsMisbehavior', ...
                                     'MaxDeviation ', ...
                                     });
    dataTable = [dataTable; newRow];

    %% Display results
    disp(['Experiment ', experiments{experiment, 1}]);
    disp(['Target centroid value: ' , num2str(targetCentroid), '| Acceptable deviation: ' , num2str(acceptableDeviation)]);
    disp(['Number of trials: ', num2str(numTrials)]);
    

    %disp('Amount of iterations per trial: ');
    %disp(numIterationsPerTrial);
    disp(['Average number of iterations until disapearance of peak: ', num2str(avgNumIterations)]);

    %disp("Maximum deviations from the target centroid:");
    %disp(trialDeviations);
    disp(['Overall maximum deviation from target centroid: ', num2str(maxDeviations)]);

    %disp("Iterations where centroid exceeds or falls below the threshold:");
    %disp(aboveOrBelowThreshold);
    disp(['Average number of iterations until misbehavior: ', num2str(avgIterationsMisbehavior(1))]);

    disp("--------------");
    diary off;
    disp("===============");
end

%% Display table
disp(dataTable);