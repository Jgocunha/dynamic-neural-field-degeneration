%% Clear the MATLAB environment
clear;
clc;
clf;
%% Add floder and sub-folders to PATH
% Determine where your m-file's folder is.
folder = fileparts(which(mfilename)); 
% Add that folder plus all subfolders to the path.
addpath(genpath(folder));


%% Setup variables
experiments = {'deactivate weights', 'weight'; ...
               'reduce 0.05 weights', 'weight'; ... 
               %'reduce 0.6 weights', 'weight'; ...
               %'reduce 0.8 weights', 'weight'; ...
               'randomize weights', 'weight'; ...
                'deactivate pre-synaptic neurons', 'pre-synaptic neuron'; ...
               'deactivate post-synaptic neurons', 'post synaptic neuron'; ...
               };

positions = {'2.0'; '6.0'; '10.0'; '14.0'; '18.0'; '22.0'; '26.0'};%};  
targetCentroids = {2.0 ; 6.0; 10.0; 14.0; 18.0; 22.; 26.00};%;

centroidsFilePath = '';
resultsFilePath = '';
plotFilePath = './plots/';
acceptableDeviation = 2.0;




%% Run analysis

for experiment = 1:size(experiments,1)
    % Initialize a variable to keep track of the total number of trials
    totalNumTrials = 0;
    dataTable = table();
    for position = 1:size(positions,1)
        %% Variable setup
        centroidsFilePath = ['../', positions{position}, ' ', experiments{experiment}, ' - centroids.txt'];
        resultsFilePath = ['./analysis/', positions{position}, ' ', experiments{experiment}, ' - analysis.txt'];
        targetCentroid = targetCentroids{position};
        %delete(resultsFilePath)
        %diary(resultsFilePath)
     
         %% Read data
         data = readData(centroidsFilePath);

                  %% Plot data
        switch experiments{experiment, 1}
            case 'deactivate weights'
                title = ['Randomly deactivating one unique ' experiments{experiment, 2}];
            case 'reduce 0.05 weights' 
                title = ['Randomly reducing one ' experiments{experiment, 2} ' by 95%'];
            case 'randomize weights'
                title = ['Randomly randomizing one ' experiments{experiment, 2}];
            case 'deactivate pre-synaptic neurons'
                title = ['Deactivating one unique ' experiments{experiment, 2}];
            case 'deactivate post-synaptic neurons'
                title = ['Deactivating one unique ' experiments{experiment, 2}];
        end
        subTitle = ['Target centroid position ', positions{position}];
        %plotData(data, plotFilePath, title, subTitle, targetCentroid, acceptableDeviation);
        plotStdDevOfDeviations(data, plotFilePath, title, subTitle, targetCentroid, acceptableDeviation);
         

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
        %% Remove negative values from the numeric array
        data_ = cell2mat(data);  % Convert cell array to numeric array
        data_ = data_(data_ >= 0);  % Filter out negative values
        
        % Calculate the average of all values in the file
        averageValue = mean(data_) - targetCentroid;

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
        %% Adapt values to percentages
        [avgNumIterations, avgIterationsMisbehavior] = adaptToPercentage(experiments{experiment, 1}, avgNumIterations, avgIterationsMisbehavior);
        %% Store data in table
        newRow = table(cellstr(experiments{experiment, 1}), ...
                       targetCentroid, ...
                       numTrials, ...
                       avgNumIterations, ...
                       avgIterationsMisbehavior(1), ...
                       averageValue, ...
                       maxDeviations, ...
                       'VariableNames', {'Condition', ...
                                         'Target centroid', ...
                                         'Trials', ...
                                         'Avg. % of affected elements until dissapearance of bump', ...
                                         'Avg. % of affected elements until misbehaviour', ...
                                         'Avg. deviation', ...
                                         'Max. deviation', ...
                                         });
        dataTable = [dataTable; newRow];

        % Increment the total number of trials for this experiment
        totalNumTrials = totalNumTrials + numTrials;
    
        %% Display results
        %disp(['Experiment ', experiments{experiment, 1}]);
        %disp(['Target centroid value: ' , num2str(targetCentroid), '| Acceptable deviation: ' , num2str(acceptableDeviation)]);
        %disp(['Number of trials: ', num2str(numTrials)]);
        %disp(['Average number of iterations until disapearance of peak: ', num2str(avgNumIterations)]);
        %disp(['Overall maximum deviation from target centroid: ', num2str(maxDeviations)]);
        %disp(['Average number of iterations until misbehavior: ', num2str(avgIterationsMisbehavior(1))]);
        %disp("--------------");
        diary off;
        %disp("===============");
    end

%     %% Calculate and display average number of trials per experiment
%     avgTrialsPerExperiment = mean(dataTable{:,'NumTrials'});
%     disp(['Average number of trials per experiment: ', num2str(avgTrialsPerExperiment)]);
%     %% Calculate and display the standard deviation of average trials
%     stdDeviationAvgTrials = std(dataTable{:,'NumTrials'}, 1);
%     disp(['Standard deviation of average trials: ', num2str(stdDeviationAvgTrials)]);
    %% Calculate and display average AvgNumIterations per experiment
    avgAvgNumIterations = mean(dataTable{:,'Avg. % of affected elements until dissapearance of bump'});
    disp(['Average AvgNumIterations per experiment: ', num2str(avgAvgNumIterations)]);
    %% Calculate and display the standard deviation of AvgNumIterations
    stdDeviationAvgNumIterations = std(dataTable{:,'Avg. % of affected elements until dissapearance of bump'}, 1);
    %disp(['Standard deviation of AvgNumIterations: ', num2str(stdDeviationAvgNumIterations)]);

    %% Calculate and display average AvgIterationsMisbehavior per experiment
    avgAvgIterationsMisbehavior = mean(dataTable{:,'Avg. % of affected elements until misbehaviour'}, 1);
    disp(['Average AvgIterationsMisbehavior per experiment: ', num2str(avgAvgIterationsMisbehavior)]);
    %% Calculate and display the standard deviation of AvgIterationsMisbehavior
    stdDeviationAvgIterationsMisbehavior = std(dataTable{:,'Avg. % of affected elements until misbehaviour'}, 1);
    %disp(['Standard deviation of AvgIterationsMisbehavior: ', num2str(stdDeviationAvgIterationsMisbehavior)]);

    %% Calculate and display average Avg. deviation per experiment
    avgDeviation = mean(dataTable{:,'Avg. deviation'},1);
    disp(['Average deviation per experiment: ', num2str(avgDeviation)]);
    %% Calculate and display the Avg. deviation of MaxDeviation
    stdDeviationDeviation = std(dataTable{:,'Avg. deviation'}, 1);
    %disp(['Standard deviation of MaxDeviation: ', num2str(stdDeviationDeviation)]);

    %% Calculate and display average MaxDeviation per experiment
    avgMaxDeviation = mean(dataTable{:,'Max. deviation'},1);
    disp(['Average MaxDeviation per experiment: ', num2str(avgMaxDeviation)]);
    %% Calculate and display the standard deviation of MaxDeviation
    stdDeviationMaxDeviation = std(dataTable{:,'Max. deviation'}, 1);
    %disp(['Standard deviation of MaxDeviation: ', num2str(stdDeviationMaxDeviation)]);

    %% Display table
    disp(dataTable);
end

