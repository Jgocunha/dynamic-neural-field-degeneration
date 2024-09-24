%% Extract relearning analysis for deactivate pre-synaptic neurons condition 

%% Clear the MATLAB environment
clear;
clc;
clf;

%% Add folder and sub-folders to PATH
folder = fileparts(which(mfilename)); 
addpath(genpath(folder));

%% Experiment parameters
% deactivate weights Only-degenerated-cases  Epochs-100 MaxCycles-5 Update-all-weights-0
resultPath = '../results/';
degeneracyType = 'deactivate post-synaptic neurons';
relearningType = 'Only-degenerated-cases';
epochs = 1;
maximumLearningCycles = 50;
updateAllWeights = 0; % 0 || 1

filePath = [resultPath, degeneracyType, ' ', relearningType, ...
    '  Epochs-', num2str(epochs), ' ', 'MaxCycles-', num2str(maximumLearningCycles), ...
    ' Update-all-weights-', num2str(updateAllWeights), '.txt'];


%% Read data from file
[dataMatrix, maxColumns]= read_data(filePath);

%% Analysis

% Initialize variables to store results
initialPer = 80;
incPer = 0.36;
finalPer = initialPer + ( incPer * maxColumns  ) - 1 * incPer;
degenerationPercentages = (initialPer:incPer:finalPer)';


numCorrectBehaviour = zeros(1, size(dataMatrix, 2)); % Number of times exhibited correct behaviour
numFailedBehaviour = zeros(1, size(dataMatrix, 2));  % Number of times failed correct behaviour
numRecoveredBehaviour = zeros(1, size(dataMatrix, 2));  % Number of times recoverd behaviour
numDeadFields = zeros(1, size(dataMatrix, 2));  % Number of "dead" fields
isDead = 0;

avgRelearningCycles = zeros(1, size(dataMatrix, 2));  % Average relearning cycles

perCorrectBehaviour = zeros(1, size(dataMatrix, 2)); % Percentage of fields that exhibited correct behaviour
perFailedBehaviour = zeros(1, size(dataMatrix, 2)); % Percentage of fields that failed correct behaviour
perRecoveredBehaviour = zeros(1, size(dataMatrix, 2)); % Percentage of fields that were able to recover behaviour
perDeadFields = zeros(1, size(dataMatrix, 2)); % Percentage of fields that died

% Iterate through each column (degeneration percentage)
for col = 1:size(dataMatrix, 2)
    % Get the data for the current column
    columnData = dataMatrix(:, col);
    
    % Check for NaN followed by zeros
    %isDead = false;s
    for i = 1:length(columnData)
        if isnan(columnData(i))
            isDead = isDead + 1;
        end
    end
    
    % Filter out zeros that appear after NaN
    validData = columnData(~isnan(columnData));

    % Count the number of "dead" fields
    numDeadFields(col) = isDead;

    % Count the number of times it failed to exhibit the correct behaviour
    numFailedBehaviour(col) = sum(validData > 0) + numDeadFields(col); 
 
    % Count the number of times relearned (value > 0)
    numRecoveredBehaviour(col) = sum(validData > 0 & validData < maximumLearningCycles);

     % Count the number of fields that exhibited correct behaviour
    numCorrectBehaviour(col) = size(dataMatrix, 1) - numFailedBehaviour(col);
    
    % Count the number of times it failed to relearn (equal to maximumLearningCycles)
    %numFailedRelearn(col) = sum(validData == maximumLearningCycles);
    
    % Calculate the average relearning cycles needed
    avgRelearningCycles(col) = mean(validData(validData > 0 & validData < maximumLearningCycles));
    

    % Calculate percentage of fields that exhibited correct behaviour
    perCorrectBehaviour(col) = numCorrectBehaviour(col) / size(dataMatrix, 1) * 100;

    % Calculate the percentage of fields that recovered from degeneration
    perRecoveredBehaviour(col) = numRecoveredBehaviour(col) / numFailedBehaviour(col) * 100;

    % Calculate percentage of fields that failed behaviour
    perFailedBehaviour(col) = numFailedBehaviour(col) / size(dataMatrix, 1) * 100;

    % Calculate percentage of dead fields
    perDeadFields(col) = numDeadFields(col) / size(dataMatrix, 1) * 100;

end

disp(['Degeneracy type: ', degeneracyType]);
disp(['Training dataset: ', relearningType]);
disp(['Update all weights: ', num2str(updateAllWeights)]);
disp(['Epochs: ', num2str(epochs)]);
disp(['Max. demonstrations: ', num2str(maximumLearningCycles)]);
disp(['Number of trials: ', num2str(size(dataMatrix, 1))]);
disp('-----------------------------------------------------------------------------------------------------------------------------');

perFailedBehaviour(end-1) = 100;
perRecoveredBehaviour(end-1) = 0;
perCorrectBehaviour(end-1) = 0;
perFailedBehaviour(end) = 100;
perRecoveredBehaviour(end) = 0;
perCorrectBehaviour(end) = 0;

% Create a table
dataTable = table(degenerationPercentages, ...
    perCorrectBehaviour', ...
    perFailedBehaviour', ...
    perRecoveredBehaviour', ...
    avgRelearningCycles', ...
    perDeadFields', ...
    'VariableNames', ...
    {'Deg. %', ...
    'Correct behaviour %', ...
    'Failed behaviour %', ...
    'Recovered behaviour %', ...
    'Avg. relearning cycles', ...
    '"Dead" fields %'});

% Display the table
disp(dataTable);

%% Plot the evolution of failed behavior, recovered behavior, and average relearning cycles
figure;

% Plot all in the same plot
yyaxis left; % Left y-axis
plot(degenerationPercentages, perFailedBehaviour, 'o-', 'LineWidth', 2, 'DisplayName', 'Failed Behavior', 'Color', 'red');
ylabel('Recovered Behavior % / Failed Behavior %');
hold on;

darkGreenColor = [0, 0.5, 0]; % RGB values (dark green)
plot(degenerationPercentages, perRecoveredBehaviour, 's-', 'LineWidth', 2, 'DisplayName', 'Recovered Behavior',  'Color', darkGreenColor);


yyaxis right; % Right y-axis
plot(degenerationPercentages, avgRelearningCycles, '^-', 'LineWidth', 2, 'DisplayName', 'Average Relearning Cycles', 'Color', 'blue');
ylabel('Average Number of Learning Demonstrations');
hold off;

title('Evolution of Behavior and Relearning Cycles');
xlabel('Degeneration Percentage');

grid on;

% Set font style and size
set(gca, 'FontName', 'Garamond');
set(gca, 'FontSize', 12);

% Add legend
legend('Location', 'northwest');

%% Save the plot    
% Remove any special characters from the title to create a valid file name
filename = 'Relearning post-synaptic neurons';
valid_title = regexprep(filename, '[^\w\s]', '');
valid_title = ['./plots/', valid_title];
% Set the desired resolution in DPI (e.g., 300)
resolution = 300;
% Set the figure size to match the desired resolution
set(gcf, 'Units', 'pixels', 'Position', [100 100 1200 800]); % Adjust the figure size as needed
% Save the plot as an image file with the desired resolution
print([valid_title '.png'], '-dpng', ['-r' num2str(resolution)]);
% Restore the original figure size (optional)
set(gcf, 'Units', 'normalized');

