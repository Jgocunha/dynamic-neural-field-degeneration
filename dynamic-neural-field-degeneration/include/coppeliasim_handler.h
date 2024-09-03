//#pragma once
//
//#include <thread>
//
//#include "../lib/coppeliasim-cpp-client/include/client.h"
//#include "./macros.h"
//
//// define signals here.
//#define CREATE_SHAPE_SIGNAL "createShape"
//#define SHAPE_CREATED_SIGNAL "shapeCreated"
//
//#define GRASP_SHAPE_SIGNAL "graspShape"
//#define SHAPE_GRASPED_SIGNAL "shapeGrasped"
//
//#define PLACE_SHAPE_SIGNAL "placeShape"
//#define SHAPE_PLACED_SIGNAL "shapePlaced"
//
//#define SHAPE_HUE_SIGNAL "shapeHue"
//#define SHAPE_ANGLE_SIGNAL "targetAngle"
//
//constexpr bool log_info = false;
//
//struct Signals
//{
//	bool createShape = false;
//	bool isShapeCreated = false;
//
//	bool graspShape = false;
//	bool isShapeGrasped = false;
//
//	bool placeShape = false;
//	bool isShapePlaced = false;
//
//	double shapeHue = UNDEFINED;
//	double targetAngle = UNDEFINED;
//};
//
//class CoppeliasimHandler
//{
//private:
//	std::thread coppeliasimThread;
//	CoppeliaSimClient client;
//	Signals signals;
//	bool wereSignalsChanged = false;
//public:
//	CoppeliasimHandler();
//	
//	void init();
//	void step();
//	void close();
//
//	void setSignals(Signals signals);
//	Signals getSignals();
//
//	void resetSignals();
//private:
//	void writeSignals();
//	void readSignals();
//};