
#include "../include/experiment-handler.h"

ExperimentHandler::ExperimentHandler(const ExperimentParameters& param)
	:param(param)
{
}

void ExperimentHandler::init()
{
	dnfcomposerHandler.init();
	coppeliasimHandler.init();
	experimentThread = std::thread(&ExperimentHandler::step, this);
}

void ExperimentHandler::step()
{
	// do a complete task

	// degenerate

	// do re-learning cycle
}

void ExperimentHandler::close()
{
	dnfcomposerHandler.close();
	coppeliasimHandler.close();
}

void ExperimentHandler::pickAndPlace()
{
	// set the create shape signal to true
	signals.createShape = true;
	coppeliasimHandler.setSignals(signals);
	signals.createShape = false;

	// wait for the shape created signal to be true
	while (!coppeliasimHandler.getSignals().isShapeCreated);

	// go pick up the cuboid
	signals.graspShape = true;
	coppeliasimHandler.setSignals(signals);
	signals.graspShape = false;

	// wait for the cuboid to be grasped
	while (!coppeliasimHandler.getSignals().isShapeGrasped);

	// wait for the hue of the cuboid
	while (!coppeliasimHandler.getSignals().shapeHue);

	// set the hue of the cuboid for dnfcomposer !!

	// wait for composer to give the target angle !!

	// if not 0 send the target angle to coppelia
	signals.targetAngle = 0.0;
	coppeliasimHandler.setSignals(signals);
	signals.targetAngle = -1.0;

	// and set place shape to true
	signals.placeShape = true;
	coppeliasimHandler.setSignals(signals);
	signals.placeShape = false;

	// when receive shape placed restart cycle
	while (!coppeliasimHandler.getSignals().isShapePlaced);
}
