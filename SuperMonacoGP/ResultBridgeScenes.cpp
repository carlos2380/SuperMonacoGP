#include "ResultBridgeScenes.h"

ResultBridgeScenes* ResultBridgeScenes::instance = NULL;

ResultBridgeScenes::ResultBridgeScenes()
{
}

ResultBridgeScenes::~ResultBridgeScenes()
{
}

ResultBridgeScenes* ResultBridgeScenes::getInstance()
{
	if (instance == NULL) instance = new ResultBridgeScenes();

	return instance;
}

