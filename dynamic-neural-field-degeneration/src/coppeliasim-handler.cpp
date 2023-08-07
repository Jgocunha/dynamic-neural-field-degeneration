
#include "../include/coppeliasim-handler.h"

CoppeliasimHandler::CoppeliasimHandler()
{
}

void CoppeliasimHandler::init()
{
	if (DEBUG)
		std::cout << "Coppeliasim Handler: Thread will start.\n";
	coppeliasimThread = std::thread(&CoppeliasimHandler::step, this);
}

void CoppeliasimHandler::step()
{
	// keep trying to initialize the connection
	while (!client.initialize());

	resetSignals();
	client.startSimulation();

	while (1)
	{
		if(wereSignalsChanged)
			writeSignals();
		readSignals();
		Sleep(10);
	}

	resetSignals();
	client.stopSimulation();
}

void CoppeliasimHandler::close()
{
	coppeliasimThread.join();
	if (DEBUG)
		std::cout << "Coppeliasim Handler: Thread has finished its execution.\n";
}

void CoppeliasimHandler::setSignals(Signals signals)
{
	wereSignalsChanged = true;
	this->signals = signals;
}

Signals CoppeliasimHandler::getSignals()
{
	return signals;
}

void CoppeliasimHandler::writeSignals()
{
	client.setIntegerSignal(CREATE_SHAPE_SIGNAL, signals.createShape);
	client.setIntegerSignal(GRASP_SHAPE_SIGNAL, signals.graspShape);
	client.setIntegerSignal(PLACE_SHAPE_SIGNAL, signals.placeShape);
	client.setFloatSignal(SHAPE_ANGLE_SIGNAL, signals.targetAngle);

	wereSignalsChanged = false;

	if (DEBUG)
		client.log("Coppeliasim Handler: New signals written.\n");
}

void CoppeliasimHandler::readSignals()
{
	signals.isShapeCreated = client.getIntegerSignal(SHAPE_CREATED_SIGNAL);
	signals.isShapeGrasped = client.getIntegerSignal(SHAPE_GRASPED_SIGNAL);
	signals.isShapePlaced = client.getIntegerSignal(SHAPE_PLACED_SIGNAL);

	signals.shapeHue = client.getFloatSignal(SHAPE_HUE_SIGNAL);

	if (DEBUG)
		client.log("Coppeliasim Handler: Signals retrieved.\n");
}

void CoppeliasimHandler::resetSignals()
{
	client.setIntegerSignal(CREATE_SHAPE_SIGNAL, 0);
	client.setIntegerSignal(SHAPE_CREATED_SIGNAL, 0);
	client.setIntegerSignal(GRASP_SHAPE_SIGNAL, 0);
	client.setIntegerSignal(SHAPE_GRASPED_SIGNAL, 0);
	client.setIntegerSignal(PLACE_SHAPE_SIGNAL, 0);
	client.setIntegerSignal(SHAPE_PLACED_SIGNAL, 0);

	client.setFloatSignal(SHAPE_HUE_SIGNAL, UNDEFINED);
	client.setFloatSignal(SHAPE_ANGLE_SIGNAL, UNDEFINED);

	if (DEBUG)
		client.log("Coppeliasim Handler: All signals were reset.\n");
}
