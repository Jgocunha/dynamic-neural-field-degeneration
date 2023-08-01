#include "../include/coppeliasimHandler.h"

CoppeliasimHandler::CoppeliasimHandler(const int& numTrials)
	:numTrials(numTrials)
{
}

bool CoppeliasimHandler::initialize()
{
	if (client.initialize())
	{
		resetSignals();
		return true;
	}
	return false;
}

void CoppeliasimHandler::setShapeHandle(const std::string& handle)
{
	cuboid.name = handle;
}

void CoppeliasimHandler::resetSignals()
{
	client.setIntegerSignal(CREATE_SHAPE_SIGNAL, 0);
	client.setIntegerSignal(SHAPE_CREATED_SIGNAL, 0);
	client.setIntegerSignal(GRASP_SHAPE_SIGNAL, 0);
	client.setIntegerSignal(SHAPE_GRASPED_SIGNAL, 0);
	client.setIntegerSignal(PLACE_SHAPE_SIGNAL, 0);
	client.setIntegerSignal(SHAPE_PLACED_SIGNAL, 0);

	client.setFloatSignal(SHAPE_HUE_SIGNAL, 0.0);
	client.setFloatSignal(SHAPE_ANGLE_SIGNAL, 0.0);

	client.log("All signals were reset.");
}

void CoppeliasimHandler::stop()
{
	client.stopSimulation();
}

double CoppeliasimHandler::getShapeHue()
{
	return cuboid.hue;
}

void CoppeliasimHandler::setTargetAngle(const double& targetAngle)
{
	client.setFloatSignal(SHAPE_ANGLE_SIGNAL, targetAngle);
}

void CoppeliasimHandler::createShape()
{
	client.setIntegerSignal(CREATE_SHAPE_SIGNAL, 1);

	int wasShapeCreated = 0;
	do
	{
		wasShapeCreated = client.getIntegerSignal(SHAPE_CREATED_SIGNAL);
		Sleep(100);
	} while (!wasShapeCreated);
}

void CoppeliasimHandler::setShapeHue()
{
	cuboid.hue = client.getFloatSignal(SHAPE_HUE_SIGNAL);
}

void CoppeliasimHandler::pickUpShape()
{
	client.setIntegerSignal(GRASP_SHAPE_SIGNAL, 1);

	int wasShapeGrasped = 0;
	do
	{
		wasShapeGrasped = client.getIntegerSignal(SHAPE_GRASPED_SIGNAL);
		Sleep(100);
	} while (!wasShapeGrasped);
}

void CoppeliasimHandler::placeShape()
{
	client.setIntegerSignal(PLACE_SHAPE_SIGNAL, 1);

	int wasShapePlaced = 0;
	do
	{
		wasShapePlaced = client.getIntegerSignal(SHAPE_PLACED_SIGNAL);
		Sleep(100);
	} while (!wasShapePlaced);
}