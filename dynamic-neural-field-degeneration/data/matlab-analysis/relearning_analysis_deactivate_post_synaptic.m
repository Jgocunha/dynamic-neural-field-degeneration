clear;
clc;

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
dataMatrix = read_data(filePath);

%% Analysis

% Initialize variables to store results
initialPer = 10;
maxColumns = 34;
incPer = 3.6;
finalPer = 92
degenerationPercentages = (initialPer:incPer:finalPer)';


numCorrectBehaviour = zeros(1, size(dataMatrix, 2)) % Number of times exhibited correct behaviour
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

% % Create a table
% dataTable = table(degenerationPercentages, ...
%     numCorrectBehaviour', perCorrectBehaviour', ...
%     numFailedBehaviour', perFailedBehaviour', ...
%     numRecoveredBehaviour', perRecoveredBehaviour', ...
%     avgRelearningCycles', ...
%     numDeadFields', perDeadFields', ...
%     'VariableNames', ...
%     {'Deg. %', ...
%     'Num. correct behaviour','Correct behaviour %', ...
%     'Num. failed behaviour', 'Failed behaviour %', ...
%     'Num. recovered behaviour', 'Recovered behaviour %', ...
%     'Avg. relearning cycles', ...
%     'Num. "dead" fields', '"Dead" fields %'});

% Display the table
disp(dataTable);

