#ifndef DIAMSQNOISE
#define DIAMSQNOISE

#include "Map.h"

typedef enum {running = 0, done} State;

class DiamSqNoise
{
	private:
		Map &map;

		State state;

		bool alreadySaved; // PARA SALVAR IMAGEM UMA VEZ

		int totalIts, doneIts;
		int percentComplete;
		int highestH;

		int sideLength;
		int randRange;

		void updatePercent();
		void checkIfFinished();

		void diamondStep();
		void squareStep();

		// função adaptada da tgaLib
		int tgaSave(char const *filename, 
			 short int		width, 
			 short int		height, 
			 unsigned char	pixelDepth,
			 unsigned char	*imageData);

	public:
		DiamSqNoise(Map &theMap);

		void runOnce();

		int getPercentComplete();
		bool isDone();

		int getHighestH();
};

#endif