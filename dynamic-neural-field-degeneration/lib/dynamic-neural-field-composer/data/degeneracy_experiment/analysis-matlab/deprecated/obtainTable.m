% Clear the MATLAB environment
clear;
clc;

%% Setup variables
experiments = {'WEIGHTS_DEACTIVATE', 'weights', 'iteration'; ...
               'WEIGHTS_DEACTIVATE_PERCENTAGE', 'weights', 'percentage'; ...
               'WEIGHTS_REDUCE', 'weights', 'iteration'; ...
               'WEIGHTS_RANDOMIZE', 'weights', 'iteration'; ...
               'NEURONS_DEACTIVATE', 'neurons', 'iteration'; ...
               'NEURONS_DEACTIVATE_PERCENTAGE', 'neurons', 'percentage'; ...
               };

locations = {'p1', 59.5; ...
        	 %'p2', 58.5; ...
             %'p3', 78.5; ...
             %'p4', 18.7; ...
             };

% Create an empty table
dataTable = table();

% Run analysis
for experiment = 1:size(experiments, 1)
    experimentDirPath = ['../', experiments{experiment, 1}, '/'];
    %disp(['Experiment ', experiments{experiment, 1}]);
    for location = 1:size(locations, 1)
        % Variable setup
        locationDirPath = [experimentDirPath, locations{location, 1}, '/'];
        % Analyze the data
        resultsFilePath = [locationDirPath, 'results.txt'];

        % Read the data from the file
        fid = fopen(resultsFilePath, 'r');
        fileData = textscan(fid, '%s', 'Delimiter', '\n');
        fclose(fid);
        
        % Extract the variables
        numTrials = str2double(fileData{1}{2}); %+3
        avgIterations = str2double(fileData{1}{5});
        maxDeviation = str2double(fileData{1}{8});
        minDeviation = str2double(fileData{1}{11});
        avgMisbehaviour = str2double(fileData{1}{14});
        
        % Create a temporary table for the current experiment and location
        tempTable = table(numTrials, avgIterations, maxDeviation, minDeviation, avgMisbehaviour, ...
            'VariableNames', {'Number of trials', 'Average iterations until no peak', 'Max. deviation', 'Min. deviation', 'Average iterations until misbehaviour'});
        
        % Create a unique row name using experiment name and location
        rowName = [experiments{experiment, 1}, '_', locations{location, 1}];
        
        % Add the row name to the temporary table
        tempTable.Properties.RowNames = {rowName};
        
        % Append the temporary table to the main table
        dataTable = [dataTable; tempTable];
    end
end

% Display the final table
disp(dataTable);
