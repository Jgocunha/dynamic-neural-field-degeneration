#pragma once

#include <thread>
#include <iostream>

#include "../lib/dynamic-neural-field-composer/include/application/application.h"
#include "./dnf-architecture.h"

class DnfcomposerHandler
{
private:
	std::thread dnfcomposerThread;
	std::unique_ptr<Application> application;

public:
	DnfcomposerHandler();
	~DnfcomposerHandler() = default;
	
	void init();
	void step();
	void close();
};