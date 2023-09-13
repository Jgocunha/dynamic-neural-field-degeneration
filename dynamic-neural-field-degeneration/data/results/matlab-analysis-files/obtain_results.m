%% Clear the MATLAB environment
clear;
clc;

%% Setup variables
experiments = {%'deactivate weights', 'weight'; ...
               %'reduce 0.4 weights', 'weight'; ...
               %'reduce 0.6 weights', 'weight'; ...
               %'reduce 0.8 weights', 'weight'; ...
               'randomize weights', 'weight'; ...
               %'deactivate pre-synaptic neurons', 'pre-synaptic neuron'; ...
               %'deactivate post-synaptic neurons', 'post synaptic neuron'; ...
               };

positions = {'15.0'; '40.0'; '65.0'; '90.0'; '115.0'; '140.0'; '165.0'};
centroidsFilePath = '';
resultsFilePath = '';
plotFilePath = '../plots/';
targetCentroids = {15.0; 40.0; 65.0; 90.0; 115.0; 140.0; 165.0};
acceptableDeviation = 0.5;


dataTable = table();

%% Run analysis
for position = 1:size(positions,1)
    for experiment = 1:size(experiments,1)
        %% Variable setup
        centroidsFilePath = ['../', positions{position}, ' ', experiments{experiment}, ' - centroids.txt'];
        resultsFilePath = ['../analysis/', positions{position}, ' ', experiments{experiment}, ' - analysis.txt'];
        targetCentroid = targetCentroids{position};
        %delete(resultsFilePath)
        diary(resultsFilePath)
     
         %% Read data
         data = readData(centroidsFilePath);

        %% Remove negative values from the cell array
        newData = {};  % Initialize a new cell array to store filtered values
        for i = 1:numel(data)
            if isnumeric(data{i})
                if data{i} >= 0
                    % Keep only non-negative numeric values
                    newData{end+1} = data{i};
                end
            elseif islogical(data{i}) || ischar(data{i})
                % Keep logical and char values as-is
                newData{end+1} = data{i};
            end
            % You can add more data type checks as needed
        end
        data = newData;  % Replace the original data with the filtered data
        
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
            case 'reduce 0.6 weights' 
                title = ['Randomly reducing one ' experiments{experiment, 2} ' by a factor of 0.6'];
            case 'reduce 0.8 weights' 
                title = ['Randomly reducing one ' experiments{experiment, 2} ' by a factor of 0.8'];
            case 'randomize weights'
                title = ['Randomly randomizing one ' experiments{experiment, 2}];
            case 'deactivate pre-synaptic neurons'
                title = ['Deactivating one unique ' experiments{experiment, 2}];
            case 'deactivate post-synaptic neurons'
                title = ['Deactivating one unique ' experiments{experiment, 2}];
        end
        subTitle = ['Target centroid position ', positions{position}];
        %plotData(data, plotFilePath, title, subTitle, targetCentroid, acceptableDeviation);
    
        %% Adapt values to percentages
        [avgNumIterations, avgIterationsMisbehavior] = adaptToPercentage(experiments{experiment, 1}, avgNumIterations, avgIterationsMisbehavior);
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
end

%% Display table
disp(dataTable);