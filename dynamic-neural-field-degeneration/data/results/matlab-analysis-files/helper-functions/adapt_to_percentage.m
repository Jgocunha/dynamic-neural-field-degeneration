function [avgNumIterationsPercentage, avgIterationsMisbehaviorPercentage] = adapt_to_percentage(experiment, avgNumIterations, avgIterationsMisbehavior)
    switch experiment
        case 'deactivate weights'
            size = 202; 
        case 'reduce 0.05 weights' 
            size = 202; 
        case 'randomize weights'
            size = 202;
        case 'deactivate pre-synaptic neurons'
            size = 720;
        case 'deactivate post-synaptic neurons'
            size = 280;
    end
    avgNumIterationsPercentage = (avgNumIterations*100)/size;
    avgIterationsMisbehaviorPercentage = (avgIterationsMisbehavior*100)/size;
end

% size of perceptual field = 360 / 0.5 = 720
% size of decision field = 28 / 0.1 = 280
% number of weights = 720 * 280 = 201600

% number of weights deactivated per iteration = 1000
% number of neurons deactivated per iteration = 1

% size of weights = 201,600/1000 = 201.6
% size of per neurons = 720/1 = 720
% size of out neurons = 280/1 = 280
