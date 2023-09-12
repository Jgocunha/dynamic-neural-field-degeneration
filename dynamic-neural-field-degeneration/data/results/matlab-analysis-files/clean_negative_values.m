%% Clear the MATLAB environment
clear;
clc;

%% Setup variables
experiments = {'deactivate weights', 'weight'; ...
               'reduce 0.4 weights', 'weight'; ...
               'reduce 0.6 weights', 'weight'; ...
               'reduce 0.8 weights', 'weight'; ...
               %'randomize weights', 'weight'; ...
               'deactivate pre-synaptic neurons', 'pre-synaptic neuron'; ...
               'deactivate post-synaptic neurons', 'post synaptic neuron'; ...
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
     
         %% Read data
         data = readData(centroidsFilePath);
    end
end
