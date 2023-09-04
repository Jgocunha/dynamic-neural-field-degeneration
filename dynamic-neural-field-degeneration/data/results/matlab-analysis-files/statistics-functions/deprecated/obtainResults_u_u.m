%% Clear the MATLAB environment
clear;
clc;

%% Setup variables
experiments = {
               'NEURONS_DEACTIVATE_field u_field u', 'neurons', 'iteration'; ...
               'NEURONS_DEACTIVATE_PERCENTAGE_field u_field u', 'neurons', 'percentage'; ...
               };

locations = {'p1', 28.5; ...
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

        %% Plot data
        switch experiments{experiment, 1}
            case 'NEURONS_DEACTIVATE_field u_field u'
                title = ['Deactivating one of the ' experiments{experiment, 2} ' (unique) in each iteration'];
                subTitle = 'Input field analysis';
            case 'NEURONS_DEACTIVATE_PERCENTAGE_field u_field u'
                title = ['Deactivating 10% of the ' experiments{experiment, 2} ' (unique) in each iteration'];
                subTitle = 'Input field analysis';
        end
        plotData(data, locationDirPath, title, subTitle, targetCentroid, acceptableDeviation);

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