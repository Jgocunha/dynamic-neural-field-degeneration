%% Run the statistical analysis of the results from the Robustness Experiment

%% Clear the MATLAB environment
clear;
clc;

%% Add floder and sub-folders to PATH
folder = fileparts(which(mfilename)); 
addpath(genpath(folder));

%% Setup variables
experiments = {%'deactivate weights', 'weight'; ...
               'reduce 0.005 weights', 'weight'; ... 
               %'randomize weights', 'weight'; ...
               %'deactivate pre-synaptic neurons', 'pre-synaptic neuron'; ...
               %'deactivate post-synaptic neurons', 'post synaptic neuron'; ...
               };

positions = {'78.0'; '82.0'; '86.0'; '90.0'; '94.0'; '98.0'; '102.0'};  
targetCentroids = {78.0; 82.0; 86.0; 90.0; 94.0; 98.0; 102.0}; 
percentageJumps = {2};
% 100% - 720 pre-synaptic
% 100% - 360 post_synaptic

% 100% - 720 * 360 / 1000 = 259.2
% 0.08% - 1.296

centroidsFilePath = '';
analysisFilePath = '';
plotFilePath = './plots/';
acceptableDeviation = 2.0;

% Define RGB values for dark green, dark yellow, and dark red
darkGreen = [0, 0.5, 0]; % RGB: [0, 128, 0]
darkYellow = [0.8, 0.8, 0]; % RGB: [204, 204, 0]
darkRed = [0.8, 0, 0]; % RGB: [204, 0, 0]

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

        %% Analyse data
        result = categorize_centroids(data, targetCentroids{position}, acceptableDeviation);
        max_length = find_max_length(result);
        result = fill_data_cells(result, max_length);

        % Initialize arrays to store sums for each degree percentage
        sum_zeros = zeros(max_length, 1);
        sum_ones = zeros(max_length, 1);
        sum_twos = zeros(max_length, 1);
        
        % Iterate through each column
        for col = 1:percentageJumps{experiment}:max_length
            % Get the current column
            current_column = cellfun(@(x) x(col), result);
            
            % Count occurrences of 0, 1, and 2
            sum_zeros(col) = sum(current_column == 0);
            sum_ones(col) = sum(current_column == 1);
            sum_twos(col) = sum(current_column == 2);
        end
        
        % Display the sums for each degree percentage
        disp('Sum of counts of 0, 1, and 2 for each degree percentage:');
        disp('Deg. %   Sum(0)   Sum(1)   Sum(2)');
        percentage = 0;
        for col = 1:percentageJumps{experiment}:max_length
            disp([num2str(percentage) blanks(9 - length(num2str(col))) num2str(sum_zeros(col)) blanks(9 - length(num2str(sum_zeros(col)))) num2str(sum_ones(col)) blanks(9 - length(num2str(sum_ones(col)))) num2str(sum_twos(col))]);
            percentage = percentage + 0.77;
        end
        
        % Calculate percentages
        total = sum_zeros + sum_ones + sum_twos;
        percentages_zeros = (sum_zeros ./ total) * 100;
        percentages_ones = (sum_ones ./ total) * 100;
        percentages_twos = (sum_twos ./ total) * 100;
        
        
        % Add 100% no behaviors at the next degeneration percentage
        deg_percentage = 0:percentageJumps{experiment}:((max_length-1)*percentageJumps{experiment} + 0);
        extra_percentage = ones(1, numel(deg_percentage)) * 100; % Ensure dimensions match

        percentages_zeros(end+1) = 0;
        percentages_ones(end+1) = 0;
        percentages_twos(end+1) = 100;
        deg_percentage(end+1) = deg_percentage(end)+1;
        

        % Define the range of the last 10 elements
        last_ten_indices = max(1, numel(deg_percentage)-20:numel(deg_percentage));
        
        % Plot grouped bar plot with adjusted spacing
        barWidth = 1; % Adjust bar width
        spacing = 1; % Adjust spacing between bars
        figure;
        bar(deg_percentage(last_ten_indices) - spacing, percentages_zeros(last_ten_indices), barWidth, 'FaceColor', darkGreen);
        hold on;
        bar(deg_percentage(last_ten_indices), percentages_ones(last_ten_indices), barWidth, 'FaceColor', darkYellow);
        bar(deg_percentage(last_ten_indices) + spacing, percentages_twos(last_ten_indices), barWidth, 'FaceColor', darkRed);
        hold off;

%         % Plot grouped bar plot with adjusted spacing
%         barWidth = 100; % Adjust bar width
%         spacing = 100; % Adjust spacing between bars
%         figure;
%         bar(deg_percentage - spacing, percentages_zeros, barWidth, 'FaceColor', darkGreen);
%         hold on;
%         bar(deg_percentage, percentages_ones, barWidth, 'FaceColor', darkYellow);
%         bar(deg_percentage + spacing, percentages_twos, barWidth, 'FaceColor', darkRed);
%         hold off;

        
        xlabel('Degeneration Percentage');
        ylabel('Percentage');
        title(['Behavioural analysis for Experiment: ', experiments{experiment}, ', Position: ', positions{position}]);
        legend('Percentage of correct behaviours', 'Percentage of misbehaviours', 'Percentage of no behaviours');
        ylim([0 100]); % Set y-axis limits to 0-100%
        grid on;
    end
    disp('______________________________________');


end




