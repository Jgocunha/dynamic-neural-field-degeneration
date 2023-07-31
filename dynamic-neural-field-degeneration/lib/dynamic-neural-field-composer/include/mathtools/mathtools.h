#pragma once

//https://github.com/stevenlovegrove/Pangolin/issues/352
#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#include <vector>
#include <array>
#include <cmath>
#include <math.h>
#include <algorithm>
#include <numeric>
#include <functional>
#include <random>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

namespace mathtools {
	// https://stackoverflow.com/questions/24518989/how-to-perform-1-dimensional-valid-convolution
	template<typename T>
	std::vector<T> conv(std::vector<T> const& f, std::vector<T> const& g)
	{
		int const nf = f.size();
		int const ng = g.size();
		int const n = nf + ng - 1;
		std::vector<T> out(n, T());
		for (auto i(0); i < n; ++i) {
			int const jmn = (i >= ng - 1) ? i - (ng - 1) : 0;
			int const jmx = (i < nf - 1) ? i : nf - 1;
			for (auto j(jmn); j <= jmx; ++j) {
				out[i] += (f[j] * g[i - j]);
			}
		}
		//out.erase(std::remove(out.begin(), out.end(), 0), out.end());
		return out;
	}

	// https://stackoverflow.com/questions/24518989/how-to-perform-1-dimensional-valid-convolution
	template<typename T>
	std::vector<T> conv_valid(std::vector<T> const& f, std::vector<T> const& g)
	{
		int const nf = f.size();
		int const ng = g.size();
		std::vector<T> const& min_v = (nf < ng) ? f : g;
		std::vector<T> const& max_v = (nf < ng) ? g : f;
		int const n = std::max(nf, ng) - std::min(nf, ng) + 1;
		std::vector<T> out(n, T());
		for (auto i(0); i < n; ++i) {
			for (int j(min_v.size() - 1), k(i); j >= 0; --j) {
				out[i] += min_v[j] * max_v[k];
				++k;
			}
		}
		return out;
	}

	template<typename T>
	std::vector<T> gaussNorm(const std::vector<int>& rangeX, const T& position, const T& sigma)
	{
		std::vector<T> g(rangeX.size());
		if (sigma)
		{
			for (int i = 0; i < g.size(); i++)
				g[i] = exp(-0.5 * pow((rangeX[i] - position), 2) / pow(sigma, 2));
		}
		else
		{
			// to do!
			//for (int i = 0; i < g.size(); i++)
				//g[i] = exp(-0.5 * pow((rangeX[i] - position), 2) / pow(sigma, 2));
		}
		if (!g.empty())
		{
			double sumOfG = std::reduce(g.begin(), g.end());
			for (int i = 0; i < g.size(); i++)
				g[i] = g[i] / sumOfG;
		}

		return g;
	}

	template<typename T>
	std::vector<T> gauss(const std::vector<int>& rangeX, const T& position, const T& sigma)
	{
		std::vector<T> g(rangeX.size());
		if (sigma)
		{
			for (int i = 0; i < g.size(); i++)
				g[i] = exp(-0.5 * pow((rangeX[i] - position), 2) / pow(sigma, 2));
		}
		else
		{
			// to do!
			//for (int i = 0; i < g.size(); i++)
				//g[i] = exp(-0.5 * pow((rangeX[i] - position), 2) / pow(sigma, 2));
		}
		return g;
	}

	template<typename T>
	std::vector<T> circularGauss(const uint32_t& size, const T& sigma, const T& position)
	{
		uint32_t l = size - 2 * 1 + 2;
		uint32_t m = 1;
		T r = position - static_cast<T>(m); // Calculate the shift with fractional part
		T rem = std::fmod(r, static_cast<T>(l)); // Calculate the remainder
		T positionShifted = rem + static_cast<T>(m); // Apply the shifted position

		std::vector<T> g(size);
		std::vector<T> xRange(size);
		std::iota(xRange.begin(), xRange.end(), static_cast<T>(1));

		if (sigma)
		{
			std::vector<T> d(size);
			std::vector<T> lMinusd(size);
			std::transform(xRange.begin(), xRange.end(), d.begin(), [&positionShifted, &l](T element) { return std::abs(element - positionShifted); });
			std::transform(d.begin(), d.end(), lMinusd.begin(), [&l](T element) { return -1 * (element - l); });
			for (int i = 0; i < size; i++)
			{
				g[i] = std::exp(-0.5 * std::pow(std::min(d[i], lMinusd[i]), 2) / std::pow(sigma, 2));
			}
		}
		//else
		//{
		//    // if sigma not zero what to do?
		//}
		return g;
	}

	template<typename T>
	std::vector<T> obtainCircularVector(const std::vector<uint32_t>& indices, const std::vector<T>& contents)
	{
		std::vector<T> newContents(indices.size());
		for (int i = 0; i < indices.size(); i++)
			newContents[i] = contents[indices[i] - 1];
		return newContents;
	}

	template<typename T>
	std::vector<T> sigmoid(const std::vector<T>& x, T beta, uint32_t x0)
	{
		std::vector<T> s(x.size());
		for (int i = 0; i < (uint32_t)s.size(); i++)
			s[i] = 1 / (1 + exp(-beta * (x[i] - x0)));
		return s;
	}

	template<typename T>
	std::vector<T> heaviside(const std::vector<T>& x, T threshold)
	{
		std::vector<T> h(x.size());
		for (int i = 0; i < (uint32_t)h.size(); i++)
			h[i] = (x[i] >= threshold) ? 1 : 0;
		return h;
	}

	template<typename T>
	std::vector<T> sumGauss(const std::vector<T>& gauss1, const std::vector<T>& gauss2)
	{
		// to do VERIFY IF GAUSSIANS ARE SAME SIZE
		std::vector<T> gaussResult(gauss1.size());
		for (int i = 0; i < gauss1.size(); i++)
			gaussResult[i] = gauss1[i] + gauss2[i];
		return gaussResult;
	}

	template <typename T>
	void fillMatrixWithRandomValues(std::vector<std::vector<T>>& matrix, const double& minRange = -1.0, const double& maxRange = 1.0) {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(minRange, maxRange);
		for (auto& row : matrix)
			for (auto& element : row)
				element = dis(gen);
	}

	template <typename T>
	void resizeMatrix(std::vector<std::vector<T>>& matrix, int newRowSize, int newColSize)
	{
		matrix.resize(newRowSize);
		for (int i = 0; i < newRowSize; i++)
			matrix[i].resize(newColSize);
	}

	std::array<uint32_t, 2> computeKernelRange(const double& sigma, int cutOfFactor, const uint32_t& fieldSize, bool circular);
	std::vector<uint32_t> createExtendedIndex(int fieldSize, const std::array<uint32_t, 2>& kernelRange);

	// generates a vector of random numbers with a normal distribution
	std::vector<double> generateNormalVector(int size);

	// this function needs a look...
	template <typename T>
	std::vector<std::vector<T>> hebbLearningRule(const std::vector<T>& input, const std::vector<T>& targetOutput, const double& learningRate)
	{
		int inputSize = input.size();
		int outputSize = targetOutput.size();
		// Check if input, and targetOutput have the same size
		if (inputSize != outputSize)
			throw std::invalid_argument("Input and targetOutput must have the same size.");

		std::vector<std::vector<T>> weights(inputSize, std::vector<T>(outputSize));
		for (int i = 0; i < inputSize; i++)
			for (int j = 0; j < outputSize; j++)
				weights[i][j] += learningRate * input[i] * targetOutput[j];

		return weights;
	}

	// this function needs a look...
	template <typename T>
	std::vector<std::vector<T>> ojaLearningRule(const std::vector<T>& input, const std::vector<T>& targetOutput, const double& learningRate)
	{
		int inputSize = input.size();
		int outputSize = targetOutput.size();
		// Check if input, and targetOutput have the same size
		if (inputSize != outputSize)
			throw std::invalid_argument("Input and targetOutput must have the same size.");

		std::vector<std::vector<T>> weights(inputSize, std::vector<T>(outputSize));
		for (int i = 0; i < inputSize; i++)
			for (int j = 0; j < outputSize; j++)
				weights[i][j] += learningRate * targetOutput[j] * (input[i] - targetOutput[j] * weights[i][j]);

		return weights;
	}

	template <typename T>
	std::vector<std::vector<T>> deltaLearningRuleWidrowHoff(std::vector<std::vector<T>>& weights, const std::vector<T>& input, const std::vector<T>& targetOutput, const double& learningRate)
	{
		int inputSize = input.size();
		int outputSize = targetOutput.size();
		if (inputSize != outputSize)
			throw std::invalid_argument("Input and targetOutput must have the same size.");

		// Calculate the activation levels of the fields based on the input values and current weights
		std::vector<T> actualOutput(outputSize, 0.0);
		for (size_t j = 0; j < outputSize; ++j) {
			for (size_t i = 0; i < inputSize; ++i) {
				actualOutput[j] += input[i] * weights[i][j];
			}
		}

		// Calculate the error between the target output and the actual output
		std::vector<T> error(outputSize, 0.0);
		for (size_t j = 0; j < outputSize; ++j) {
			error[j] = targetOutput[j] - actualOutput[j];
		}

		// Update the weights based on the error and current activation levels of the fields
		for (size_t i = 0; i < inputSize; ++i) {
			for (size_t j = 0; j < outputSize; ++j) {
				weights[i][j] += learningRate * error[j] * input[i];
			}
		}

		return weights;
	}

	template <typename T>
	std::vector<std::vector<T>> deltaLearningRuleKroghHertz(std::vector<std::vector<T>>& weights, const std::vector<T>& input, const std::vector<T>& targetOutput, const double& learningRate)
	{

		 double deltaT = 1.0;
		 double tau_w = 5.0;
		 double eta = 0.5;

		int inputSize = input.size();
		int outputSize = targetOutput.size();

		// Calculate the activation levels of the fields based on the input values and current weights
		std::vector<T> actualOutput(outputSize, 0.0);
		for (size_t j = 0; j < outputSize; ++j) {
			for (size_t i = 0; i < inputSize; ++i) {
				actualOutput[j] += input[i] * weights[i][j];
			}
		}

		// Calculate the error between the target output and the actual output
		std::vector<T> error(outputSize, 0.0);
		for (size_t j = 0; j < outputSize; ++j) {
			error[j] = targetOutput[j] - actualOutput[j];
		}

		// Update the weights based on the error and current activation levels of the fields
		for (size_t i = 0; i < inputSize; ++i) {
			for (size_t j = 0; j < outputSize; ++j) {
				weights[i][j] += learningRate * (error[j] - eta * weights[i][j]) * input[i];
			}
		}

		return weights;
	}


	template <typename T>
	T generateRandomNumber(const T& min, const T& max)
	{
		// Seed the random number generator with a random device
		std::random_device rd;
		std::mt19937 gen(rd());
		// Create a uniform distribution from 1 to 2 (inclusive)
		std::uniform_real_distribution<> dis(min, max);
		// Generate a random integer between 1 and 2
		T randomNum = dis(gen);
		return randomNum;
	}

	// Function to count the number of lines in a file
	int countNumOfLinesInFile(const std::string& filename);
}