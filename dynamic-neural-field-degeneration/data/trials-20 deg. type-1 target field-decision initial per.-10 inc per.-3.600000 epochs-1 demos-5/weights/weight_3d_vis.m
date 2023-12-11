clear;
clc;

weights = load('per - dec_weights - copy.txt');

[X, Y] = meshgrid(1:size(weights, 2), 1:size(weights, 1));

figure;
surf(X, Y, weights);
title('Synaptic connections weights between perceptual and output fields');
subtitle('Before relearning')
xlabel('Output field neural positions');
ylabel('Perceptual field neural positions');
zlabel('Weight strength');


weights = load('per - dec_weights.txt');

[X, Y] = meshgrid(1:size(weights, 2), 1:size(weights, 1));

figure;
surf(X, Y, weights);
title('Synaptic connections weights between perceptual and output fields');
subtitle('After relearning')
xlabel('Output field neural positions');
ylabel('Perceptual field neural positions');
zlabel('Weight strength');

% Load the first matrix
weights1 = load('per - dec_weights - copy.txt');

% Load the second matrix
weights2 = load('per - dec_weights.txt');

% Calculate the difference
weight_diff = weights1 - weights2;

% Create meshgrid
[X, Y] = meshgrid(1:size(weight_diff, 2), 1:size(weight_diff, 1));

% Plot the 3D surface of the differences
figure;
surf(X, Y, weight_diff);
title('Differences in Synaptic Connection Weights');
xlabel('Output Field Neural Positions');
ylabel('Perceptual Field Neural Positions');
zlabel('Weight Strength Difference');