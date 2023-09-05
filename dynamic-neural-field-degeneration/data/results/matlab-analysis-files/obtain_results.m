%% Clear the MATLAB environment
clear;
clc;

%% Setup variables
experiments = {'deactivate weights', 'weight'; ...
               %'reduce 0.4 weights', 'weight'; ...
               %'reduce-weights-0.6', 'weight'; ...
               %'reduce-weights-0.8', 'weight'; ...
               'randomize weights', 'weight'; ...
               'deactivate pre-synaptic neurons', 'pre-synaptic neuron'; ...
               'deactivate post-synaptic neurons', 'post synaptic neuron'; ...
               };

centroidsFilePath = '';
resultsFilePath = '';
targetCentroid = 40;
acceptableDeviation = 0.1;


dataTable = table();

%% Run analysis
for experiment = 1:size(experiments,1)
    %% Variable setup
    centroidsFilePath = ['../', experiments{experiment}, ' - centroids.txt'];
    resultsFilePath = ['../', experiments{experiment}, ' - results.txt'];
    %delete(resultsFilePath)
    %diary(resultsFilePath)
 
     %% Read data
     data = readData(centroidsFilePath);
 
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
        case 'deactivate weights'
            title = ['Randomly deactivating one unique ' experiments{experiment, 2}];
        case 'reduce 0.4 weights' 
            title = ['Randomly reducing one ' experiments{experiment, 2} ' by a factor of 0.4'];
        case 'reduce-weights-0.6' 
            title = ['Randomly reducing one ' experiments{experiment, 2} ' by a factor of 0.6'];
        case 'reduce-weights-0.8' 
            title = ['Randomly reducing one ' experiments{experiment, 2} ' by a factor of 0.8'];
        case 'randomize weights'
            title = ['Randomly randomizing one ' experiments{experiment, 2}];
        case 'deactivate pre-synaptic neurons'
            title = ['Deactivating one unique ' experiments{experiment, 2}];
        case 'deactivate post-synaptic neurons'
            title = ['Deactivating one unique ' experiments{experiment, 2}];
    end
    subTitle = '';

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