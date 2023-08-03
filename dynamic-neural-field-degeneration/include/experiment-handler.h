
#pragma once

#include <iostream>
#include <thread>

#include "./coppeliasim-handler.h"
#include "./dnfcomposer-handler.h"


struct ExperimentParameters
{

};

struct ExperimentData
{

};

class ExperimentHandler
{
private:
	CoppeliasimHandler coppeliasimHandler;
	DnfcomposerHandler dnfcomposerHandler;
	std::thread experimentThread;

	ExperimentParameters param;
	ExperimentData data;
	Signals signals;
public:
	ExperimentHandler(const ExperimentParameters& param);
	~ExperimentHandler() = default;

	void init();
	void step();
	void close();
private:
	void pickAndPlace();
};