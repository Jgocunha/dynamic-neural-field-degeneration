function [avgNumIterationsPercentage, avgIterationsMisbehaviorPercentage] = adaptToPercentage(experiment, avgNumIterations, avgIterationsMisbehavior)
    switch experiment
        case 'deactivate weights'
            size = 648;
        case 'reduce 0.4 weights' 
            size = 648;
        case 'reduce 0.6 weights' 
            size = 648;
        case 'reduce 0.8 weights' 
            size = 648;
        case 'randomize weights'
            size = 648;
        case 'deactivate pre-synaptic neurons'
            size = 360;
        case 'deactivate post-synaptic neurons'
            size = 180;
    end
    avgNumIterationsPercentage = (avgNumIterations*100)/size;
    avgIterationsMisbehaviorPercentage = (avgIterationsMisbehavior*100)/size;
end

