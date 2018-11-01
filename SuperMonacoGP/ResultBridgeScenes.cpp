#include "ResultBridgeScenes.h"

ResultBridgeScenes* ResultBridgeScenes::instance = nullptr;

ResultBridgeScenes::ResultBridgeScenes()
{
}

ResultBridgeScenes::~ResultBridgeScenes()
{
}

ResultBridgeScenes* ResultBridgeScenes::getInstance()
{
	if (instance == nullptr) instance = new ResultBridgeScenes();

	return instance;
}

void ResultBridgeScenes::resetInstance()
{
	delete instance;
	instance = nullptr;
}
