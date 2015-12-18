#ifndef NOISE
#define NOISE

#include "Map/Map.h"

#include <string>
#include <memory>

#ifndef NOISESTATES
#define NOISESTATES
typedef enum {running = 0, done} State;
#endif

class Noise
{
	public:
		const std::string name;

		Noise(std::string n) : name(n) {};
		virtual ~Noise() {};
		
		virtual void setMap(std::weak_ptr<Map> m) = 0;

		virtual void reset() = 0;
		
		virtual void runOnce() = 0;

		virtual int getPercentComplete() = 0;
};

#endif
