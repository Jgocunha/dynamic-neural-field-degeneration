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
               %'deactivate post-synaptic neurons', 'post synaptic neuron'; ...
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
        %data = clean_data(data);

        %% Analyse data
        result = categorize_centroids(data, targetCentroids{position}, acceptableDeviation);

        % Count the occurrences of 0s, 1s, and 2s for each column
        counts = zeros(length(expectedValues), size(data, 2));
        
        for i = 1:length(expectedValues)
            for j = 1:size(data, 2)
                counts(i, j) = sum(result{i}(:, j) == 0);  % Count 0s
                counts(i, j + size(data, 2)) = sum(result{i}(:, j) == 1);  % Count 1s
                counts(i, j + 2 * size(data, 2)) = sum(result{i}(:, j) == 2);  % Count 2s
            end
        end
        
        % Create a bar graph
        figure;
        bar(counts);
        legend('0s - Column 1', '1s - Column 1', '2s - Column 1', '0s - Column 2', '1s - Column 2', '2s - Column 2', 'Location', 'Best');
        xlabel('Expected Values');
        ylabel('Counts');
        title('Counts of 0s, 1s, and 2s for Each Column');
    end
end