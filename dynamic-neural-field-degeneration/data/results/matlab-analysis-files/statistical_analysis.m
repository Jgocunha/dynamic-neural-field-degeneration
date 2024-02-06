%% Run the statistical analysis of the results from the Robustness Experiment

%% Clear the MATLAB environment
clear;
clc;

%% Add floder and sub-folders to PATH
folder = fileparts(which(mfilename)); 
addpath(genpath(folder));

%% Setup variables
experiments = {%'deactivate weights', 'weight'; ...
               %'reduce 0.05 weights', 'weight'; ... 
               %'randomize weights', 'weight'; ...
               'deactivate pre-synaptic neurons', 'pre-synaptic neuron'; ...
               'deactivate post-synaptic neurons', 'post synaptic neuron'; ...
               };

positions = {'78.0'; '82.0'; '86.0'; '90.0'; '94.0'; '98.0'; '102.0'};  
targetCentroids = {78.0; 82.0; 86.0; 90.0; 94.0; 98.0; 102.0}; 

centroidsFilePath = '';
analysisFilePath = '';
plotFilePath = './plots/';
acceptableDeviation = 2.0;

%% Run analysis
for experiment = 1:size(experiments,1)
    totalNumTrials = 0;
    dataTable = table();
    analysisFilePath = ['./analysis/ ', experiments{experiment}, ' - analysis.txt'];

    for position = 1:size(positions,1)
        %% Variable setup
        centroidsFilePath = ['../', positions{position}, ' ', experiments{experiment}, ' - centroids.txt'];
        targetCentroid = targetCentroids{position};

        %% Read data
        data = read_data(centroidsFilePath);
        data = clean_data(data);

        %% Analyse data
        % get number of trials
        numTrials = numel(data);
        % get iterations until disapearance of peak
        [numIterationsPerTrial, avgNumIterations] = get_iterations_trial(data);
        % get iterations until misbehavior of peak
        [aboveOrBelowThreshold,avgIterationsMisbehavior] = get_iterations_misbehavior(data,targetCentroid, acceptableDeviation);
        % get deviations, and maximum deviation
        [trialDeviations, maxDeviations] = get_max_deviations(data, targetCentroid);
        % get average centroid value
        [avgCentroidValuePerTrial,avgCentroidValue] = get_avg_centroid(data);

        %% Adapt values to percentages
        [avgNumIterations, avgIterationsMisbehavior] = adapt_to_percentage(experiments{experiment, 1}, avgNumIterations, avgIterationsMisbehavior);

        %% Store data in table
        newRow = table(cellstr(experiments{experiment, 1}), ...
                       targetCentroid, ...
                       numTrials, ...
                       avgNumIterations, ...
                       avgIterationsMisbehavior(1), ...
                       maxDeviations, ...
                       'VariableNames', {'Condition', ...
                                         'Target centroid', ...
                                         'Trials', ...
                                         'Avg. % of affected elements until disapearance of bump', ...
                                         'Avg. % of affected elements until misbehaviour', ...
                                         'Max. deviation', ...
                                         });
        dataTable = [dataTable; newRow];

        % Increment the total number of trials for this experiment
        totalNumTrials = totalNumTrials + numTrials;

    end
    %% Display and save table
    delete(analysisFilePath)
    diary(analysisFilePath)

    % Calculate and display Avg. % of affected elements until disapearance of bump
    avgAvgNumIterations = mean(dataTable{:,'Avg. % of affected elements until disapearance of bump'});
    disp(['Avg. % of affected elements until disapearance of bump: ', num2str(avgAvgNumIterations)]);

    % Calculate and display average Avg. % of affected elements until misbehaviour
    avgAvgIterationsMisbehavior = mean(dataTable{:,'Avg. % of affected elements until misbehaviour'}, 1);
    disp(['Avg. % of affected elements until misbehaviour: ', num2str(avgAvgIterationsMisbehavior)]);

    % Calculate and display average Max. deviation per experiment
    avgMaxDeviation = mean(dataTable{:,'Max. deviation'},1);
    disp(['Average Max. deviation per experiment: ', num2str(avgMaxDeviation)]);

    disp(dataTable);
    diary off;
end