#ifndef DIAMSQNOISE
#define DIAMSQNOISE

#include "../Map.h"
#include "../Noises/Noise.h"

//typedef enum {running = 0, done} State;
#ifndef NOISESTATES
#define NOISESTATES
typedef enum {readTect = 0, running, doTect, readEro, doEro, done} State;
#endif

class DiamSqNoise : public Noise
{
	private:
		Map *map;

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
		DiamSqNoise(Map *theMap);

		void setMap(Map *m);

		void reset();

		void runOnce();

		int getPercentComplete();
};

#endif