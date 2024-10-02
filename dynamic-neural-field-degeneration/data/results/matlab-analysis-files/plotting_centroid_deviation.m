%% Plot the evolution of centroid deviation as a function of the degeneration percentage

%% Clear the MATLAB environment
clear;
clc;
clf;

%% Add folder and sub-folders to PATH
folder = fileparts(which(mfilename)); 
addpath(genpath(folder));

%% Setup variables

numberOfPresynapticNeurons = 720;
numberOfPostsynapticNeurons = 280;
numberOfWeights = numberOfPresynapticNeurons * numberOfPostsynapticNeurons;
numberOfWeightsDegeneratedPerIteration = 1000;
numberOfNeuronsDegeneratedPerIteration = 1;
experiments = {'deactivate weights', 'weight', numberOfWeights, numberOfWeightsDegeneratedPerIteration, 0.55; ...
               'reduce 0.005 weights', 'weight', numberOfWeights, numberOfWeightsDegeneratedPerIteration, 0.5; ... 
               'randomize weights', 'weight', numberOfWeights, numberOfWeightsDegeneratedPerIteration, 1.0; ...
               'deactivate pre-synaptic neurons', 'pre-synaptic neuron', numberOfPresynapticNeurons, numberOfNeuronsDegeneratedPerIteration, 0.0; ...
               'deactivate post-synaptic neurons', 'post synaptic neuron', numberOfPostsynapticNeurons, numberOfNeuronsDegeneratedPerIteration, 0.0; ...
               };


positions = {'2.0'; '6.0'; '10.0'; '14.0'; '18.0'; '22.0'; '26.0'};  
targetCentroids = {2.0 ; 6.0; 10.0; 14.0; 18.0; 22.0; 26.00};

centroidsFilePath = '';
analysisFilePath = '';
plotFilePath = './plots/5-random-centroid-deviation-as-degeneration-occurs/';
acceptableDeviation = 1.0;

%% Run analysis
for experiment = 1:size(experiments,1)
    for position = 1:size(positions,1)
        %% Variable setup
        centroidsFilePath = ['../', positions{position}, ' ', experiments{experiment}, ' - centroids.txt'];
        targetCentroid = targetCentroids{position};

        %% Read data
        data = read_data(centroidsFilePath);
        data = select_data_plots(data, experiments{experiment, 5});
        
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
        
        numberOfElementsTotal = experiments{experiment, 3};
        numberOfElementsDegeneratedPerIteration = experiments{experiment, 4};
        deviationsFileName =  [title, ' ', subTitle];
        deviations = get_centroid_deviations(deviationsFileName, data, targetCentroid);
        plot_centroid_deviation(deviations, plotFilePath, title, subTitle, targetCentroid, acceptableDeviation, numberOfElementsTotal, numberOfElementsDegeneratedPerIteration);
    end
end