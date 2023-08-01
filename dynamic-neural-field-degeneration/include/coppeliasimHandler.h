#pragma once

#include "../lib/dynamic-neural-field-composer/include/application/application.h"
#include "../lib/coppeliasim-cpp-client/include/client.h"


// define signals here.
#define CREATE_SHAPE_SIGNAL "createShape"
#define SHAPE_CREATED_SIGNAL "shapeCreated"

#define GRASP_SHAPE_SIGNAL "graspShape"
#define SHAPE_GRASPED_SIGNAL "shapeGrasped"

#define PLACE_SHAPE_SIGNAL "placeShape"
#define SHAPE_PLACED_SIGNAL "shapePlaced"

#define SHAPE_HUE_SIGNAL "shapeHue"
#define SHAPE_ANGLE_SIGNAL "targetAngle"


struct Shape
{
	std::string name;
	double hue;
};


class CoppeliasimHandler
{
private:
	CoppeliaSimClient client;
	Shape cuboid;
	double robotTargetAngle;
	int numTrials;
public:
	CoppeliasimHandler(const int& numTrials);

	bool initialize();
	void stop();

	double getShapeHue();
	void setTargetAngle(const double& targetAngle);

public:
	void setShapeHandle(const std::string& handle);
	void resetSignals();
	void createShape();
	void setShapeHue();
	void pickUpShape();
	void placeShape();
};