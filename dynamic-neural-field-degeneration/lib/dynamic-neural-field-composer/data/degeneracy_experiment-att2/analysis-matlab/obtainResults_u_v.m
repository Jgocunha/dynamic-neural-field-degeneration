%% Clear the MATLAB environment
clear;
clc;

%% Setup variables
experiments = {'WEIGHTS_DEACTIVATE', 'weights', 'iteration'; ...
               'WEIGHTS_DEACTIVATE_PERCENTAGE', 'weights', 'percentage'; ...
               'WEIGHTS_REDUCE_0.400000', 'weights', 'iteration'; ...
               'WEIGHTS_REDUCE_0.600000', 'weights', 'iteration'; ...
               'WEIGHTS_REDUCE_0.800000', 'weights', 'iteration'; ...
               'WEIGHTS_RANDOMIZE', 'weights', 'iteration'; ...
               'NEURONS_DEACTIVATE_field u_field v', 'neurons', 'iteration'; ...
               'NEURONS_DEACTIVATE_PERCENTAGE_field u_field v', 'neurons', 'percentage'; ...
               };

locations = {'p1', 59.5; ...
             %'p2', 58.5; ...
             %'p3', 78.5; ...
             %'p4', 18.7; ...
             };

experimentDirPath = '';
locationDirPath = '';
centroidFilePath = '';
resultsFilePath = '';
targetCentroid = 0;
acceptableDeviation = 0.1;

dataTable = table();

%% Run analysis
for experiment = 1:size(experiments,1)
    experimentDirPath = ['../', experiments{experiment}, '/'];    
    for location = 1:size(locations,1)
        %% Variable setup
        locationDirPath = [experimentDirPath, locations{location}, '/'];
        targetCentroid = locations{location,2};
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
            case 'NEURONS_DEACTIVATE_PERCENTAGE_field u_field v'
                title = ['Deactivating 10% of the ' experiments{experiment, 2} ' (unique) in each iteration'];
        end
        subTitle = '';
        %plotData(data, locationDirPath, title, subTitle, targetCentroid, acceptableDeviation);

        %% Store data in table
        newRow = table(cellstr(experiments{experiment, 1}), ...
                       cellstr(locations{location, 1}), ...
                       targetCentroid, ...
                       acceptableDeviation, ...
                       numTrials, ...
                       avgNumIterations, ...
                       avgIterationsMisbehavior(location), ...
                       maxDeviations, ...
                       'VariableNames', {'Experiment', ...
                                         'Location', ...
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
        disp(['Location: ', locations{location,1}, '| Target centroid value: ' , num2str(targetCentroid), '| Acceptable deviation: ' , num2str(acceptableDeviation)]);
        disp(['Number of trials: ', num2str(numTrials)]);
        

        %disp('Amount of iterations per trial: ');
        %disp(numIterationsPerTrial);
        disp(['Average number of iterations until disapearance of peak: ', num2str(avgNumIterations)]);

        %disp("Maximum deviations from the target centroid:");
        %disp(trialDeviations);
        disp(['Overall maximum deviation from target centroid: ', num2str(maxDeviations)]);

        %disp("Iterations where centroid exceeds or falls below the threshold:");
        %disp(aboveOrBelowThreshold);
        disp(['Average number of iterations until misbehavior: ', num2str(avgIterationsMisbehavior(location))]);

        disp("--------------");
        diary off;
    end
    % Display the overall maximum deviations for the experiment
    %disp(['Overall maximum deviations for experiment ', experiments{experiment, 1}, ':']);
    %disp(maxDeviations);
    %disp(['Overall average number of iterations for experiment ', experiments{experiment, 1}, ':']);
    %disp(avgIterations);
    disp("===============");
end

%% Display table
disp(dataTable);