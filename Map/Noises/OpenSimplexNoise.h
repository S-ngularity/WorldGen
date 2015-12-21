#ifndef OSIMPLEXNOISE
#define OSIMPLEXNOISE

#include <memory>

#include "../Noises/Noise.h"
#include "OpenSimplexC/open-simplex-noise.h"

#include "Ui/EvtAggr.h"
#include "Ui/UiEvents/UiCode.h"

class Map;

//typedef enum {running = 0, done} State;

class OpenSimplexNoise : public Noise
{
	private:
		std::shared_ptr<Map> map;

		State state;

		bool alreadySaved; // PARA SALVAR IMAGEM UMA VEZ

		long int doneIts, totalIts;

		int nowX, nowY;

		int octaves;
		double frequency;
		double persistence;
		double freqDiv;

		osn_context *context;

		long subscribeTkUiCode;
		long subscribeTkNoiseInfoRequest;

		void checkIfFinished();


		// função adaptada da tgaLib
		int tgaSave(char const *filename, 
			 short int		width, 
			 short int		height, 
			 unsigned char	pixelDepth,
			 unsigned char	*imageData);

	public:
		OpenSimplexNoise(std::shared_ptr<Map> theMap, int oct, double freq, double pers, double fdiv);
		~OpenSimplexNoise();

		void handleEvtCode(UiCode &c);

		void setMap(std::shared_ptr<Map> m);

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