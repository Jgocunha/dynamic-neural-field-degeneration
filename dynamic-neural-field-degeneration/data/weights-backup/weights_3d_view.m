clear;
clc;

weights = load('per - dec_weights.txt');

[X, Y] = meshgrid(1:size(weights, 2), 1:size(weights, 1));

figure;
surf(X, Y, weights);
title('Synaptic connections weights between perceptual and output fields');
xlabel('Output field neural positions');
ylabel('Perceptual field neural positions');
zlabel('Weight strength');


