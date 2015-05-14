#ifndef OSIMPLEXNOISE
#define OSIMPLEXNOISE

#include "../Noises/Noise.h"
#include "OpenSimplexC/open-simplex-noise.h"

class Map;

//typedef enum {running = 0, done} State;

class OpenSimplexNoise : public Noise
{
	private:
		Map *map;

		State state;

		bool alreadySaved; // PARA SALVAR IMAGEM UMA VEZ

		long int doneIts, totalIts;

		int nowX, nowY;

		int octaves;
		double frequency;
		double persistence;
		double freqDiv;

		osn_context *context;

		void checkIfFinished();


		// função adaptada da tgaLib
		int tgaSave(char const *filename, 
			 short int		width, 
			 short int		height, 
			 unsigned char	pixelDepth,
			 unsigned char	*imageData);

	public:
		OpenSimplexNoise(Map *theMap, int oct, double freq, double pers, double fdiv);
		~OpenSimplexNoise();

		void setMap(Map *m);

		void reset();

		void runOnce();

		int getPercentComplete();
};

#endif

// continentes
//OCTAVES 10
//FREQUENCY 0.004 //~2.5 para 4d tiling
//PERSISTENCE 0.6
//FREQ_DIV 1.9