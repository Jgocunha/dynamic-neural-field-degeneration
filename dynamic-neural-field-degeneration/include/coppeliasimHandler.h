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

#define SHAPE_COLOR_SIGNAL "shapeColor"
#define SHAPE_BOX_SIGNAL "shapeBox"


struct Shape
{
	std::string name;
	std::string color;
};

enum BoxToPlaceShape
{
	BOX_1 = 1, // red
	BOX_2, // orange
	BOX_3, // yellow
	BOX_4, // green
	BOX_5, // blue
	BOX_6, // indigo
	BOX_7, // violet
};

extern std::map<BoxToPlaceShape, std::string> boxToPlaceShapeMap;

class CoppeliasimHandler
{
private:
	CoppeliaSimClient client;
	Shape cuboid;
	BoxToPlaceShape box;
	int numTrials;
public:
	CoppeliasimHandler(const int& numTrials);

	bool initialize();
	void run();
	void startStep(const int& currentTrial);
	void endStep();
	void stop();

	std::string getShapeColor();
	void setTargetBox(const std::string& box);

public:
	void setShapeHandle(const std::string& handle);
	void resetSignals();
	void createShape();
	void getShapeParameters();
	void pickUpShape();
	void placeShape();
	void computeTargetBox();
};