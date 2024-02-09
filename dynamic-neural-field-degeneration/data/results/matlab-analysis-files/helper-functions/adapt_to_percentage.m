function [avgNumIterationsPercentage, avgIterationsMisbehaviorPercentage] = adapt_to_percentage(experiment, avgNumIterations, avgIterationsMisbehavior)
    switch experiment
        case 'deactivate weights'
            size = 259; 
        case 'reduce 0.05 weights' 
            size = 259; 
        case 'randomize weights'
            size = 259;
        case 'deactivate pre-synaptic neurons'
            size = 720;
        case 'deactivate post-synaptic neurons'
            size = 360;
    end
    avgNumIterationsPercentage = (avgNumIterations*100)/size;
    avgIterationsMisbehaviorPercentage = (avgIterationsMisbehavior*100)/size;
end

% size of perceptual field = 360 / 0.5 = 720
% size of decision field = 28 / 0.1 = 360
% number of weights = 720 * 360 = 259,200

% number of weights deactivated per iteration = 1000
% number of neurons deactivated per iteration = 1

% size of weights = 201,600/1000 = 259.2
% size of per neurons = 720/1 = 720
% size of out neurons = 360/1 = 360
