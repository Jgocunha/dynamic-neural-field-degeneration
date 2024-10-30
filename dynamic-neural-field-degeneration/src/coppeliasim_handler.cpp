
#include "coppeliasim_handler.h"


CoppeliasimHandler::CoppeliasimHandler()
	:client("127.0.0.1", 19995, coppeliasim_cpp::LogMode::NO_LOGS)
{
}

void CoppeliasimHandler::init()
{
	if (log_info)
		std::cout << "Coppeliasim Handler: Thread will start.";
	coppeliasimThread = std::thread(&CoppeliasimHandler::step, this);
}

void CoppeliasimHandler::step()
{
	// keep trying to initialize the connection
	while (!client.initialize());

	resetSignals();
	client.startSimulation();

	while (true)
	{
		if (wereSignalsChanged)
			writeSignals();
		readSignals();
		//Sleep(10);
	}

	resetSignals();
	client.stopSimulation();
}

void CoppeliasimHandler::close()
{
	coppeliasimThread.join();
	if (log_info)
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

	if (log_info)
		client.log_msg("Coppeliasim Handler: New signals written.");
}

void CoppeliasimHandler::readSignals()
{
	signals.isShapeCreated = client.getIntegerSignal(SHAPE_CREATED_SIGNAL);
	signals.isShapeGrasped = client.getIntegerSignal(SHAPE_GRASPED_SIGNAL);
	signals.isShapePlaced = client.getIntegerSignal(SHAPE_PLACED_SIGNAL);

	signals.shapeHue = client.getFloatSignal(SHAPE_HUE_SIGNAL);

	if (log_info)
		client.log_msg("Coppeliasim Handler: Signals retrieved.");
}

void CoppeliasimHandler::resetSignals()
{
	client.setIntegerSignal(CREATE_SHAPE_SIGNAL, 0);
	client.setIntegerSignal(SHAPE_CREATED_SIGNAL, 0);
	client.setIntegerSignal(GRASP_SHAPE_SIGNAL, 0);
	client.setIntegerSignal(SHAPE_GRASPED_SIGNAL, 0);
	client.setIntegerSignal(PLACE_SHAPE_SIGNAL, 0);
	client.setIntegerSignal(SHAPE_PLACED_SIGNAL, 0);

	client.setFloatSignal(SHAPE_HUE_SIGNAL, -1);
	client.setFloatSignal(SHAPE_ANGLE_SIGNAL, -1);

	if (log_info)
		client.log_msg("Coppeliasim Handler: All signals were reset.");
}