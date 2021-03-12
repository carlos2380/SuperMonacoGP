#include "AICar.h"
#include "Line.h"
using namespace std;


AICar::AICar()
{
}


AICar::~AICar()
{}

int AICar::getNextLine() {
	++counter;
	if (counter % divisor <  speed%divisor) {
		return linePos + 1;
	}
	else return -1;
}