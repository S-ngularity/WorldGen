#ifndef NOISE
#define NOISE

#include "Map.h"

class Noise
{
	public:
		virtual void setMap(Map *m) = 0;

		virtual void reset() = 0;
		
		virtual void runOnce() = 0;

		virtual int getPercentComplete() = 0;
};

#endif
