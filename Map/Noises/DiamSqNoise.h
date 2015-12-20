#ifndef DIAMSQNOISE
#define DIAMSQNOISE

#include "Map/Noises/Noise.h"

#include <string>
#include <memory>

class DiamSqNoise : public Noise
{
	private:
		Map &map;

		State state;

		bool alreadySaved; // PARA SALVAR IMAGEM UMA VEZ

		int doneIts, totalIts;

		int sideLength;
		int randRange;

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

		void reset();

		void runOnce();

		int getPercentComplete();
};

#endif