#ifndef MYNOISE
#define MYNOISE

#include "Map.h"

// ask for update at every X percent completed
#define UPDATE_AT_PERCENT 20

// askingForScreenUpdate return values
#define NO 0
#define WITHOUT_SEA 1
#define WITH_SEA 2

typedef enum {readTect = 0, doTect, readEro, doEro, done} State;

class MyNoise
{
	private:
		Map &map;

		State state;
		int shouldUpdateScreen;

		int numIts, iteration;
		int percentComplete;
		int highestH;

		void tectonics();
		void erosion();
		
		void readTectonics();
		void doTectonics();
		void readErosion();
		void doErosion();

		void updatePercent();
		void checkIfFinished();


		// funções de inserção de seeds -- retornam posição da seed inserida
		Pos insertSeedHigh(Pos seedPos, float rangeMultiplier);
		Pos insertSeedLow(Pos seedPos, float rangeMultiplier);

		// funções de inserção de artefatos
		void insertHighArtifact(Pos seedPos, float rangeMultiplier);
		void insertLowArtifact(Pos seedPos, float rangeMultiplier);

	public:
		MyNoise(Map &theMap);

		void runOnce();

		int askingForScreenUpdate();

		int getHighestH();
};

#endif