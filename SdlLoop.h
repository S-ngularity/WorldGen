#ifndef SDLLOOP
#define SDLLOOP

#define RENDERWHENMOD 20

typedef enum {readTectonics = 0, doTectonics, readErosion, doErosion, done} State;

class SdlLoop
{
	private:
		State state;
		
		void readTectonics();
		void doTectonics();
		void readErosion();
		void doErosion();

	public:
		SdlLoop();

		int numIts, iteration;
		int percentComplete;
		int highestH;

		void loop();
};

#endif