#ifndef MYNOISE
#define MYNOISE

#include "../Noises/Noise.h"

class Map;

class MyNoise : public Noise
{
	private:
		Map *map;

		State state;

		bool alreadySaved; // PARA SALVAR IMAGEM UMA VEZ

		int totalTecIts, totalErsIts, doneIts;

		void readIterations();

		void checkIfFinished();

		void tectonics();
		void erosion();

		// funções de inserção de seeds -- retornam posição da seed inserida
		Pos insertSeedHigh(Pos seedPos, float rangeMultiplier);
		Pos insertSeedLow(Pos seedPos, float rangeMultiplier);

		// funções de inserção de artefatos
		void insertHighArtifact(Pos seedPos, float rangeMultiplier);
		void insertLowArtifact(Pos seedPos, float rangeMultiplier);

		// função adaptada da tgaLib
		int tgaSave(char const *filename, 
			 short int		width, 
			 short int		height, 
			 unsigned char	pixelDepth,
			 unsigned char	*imageData);

	public:
		MyNoise(Map *theMap);

		void setMap(Map *m);

		void reset();

		void runOnce();

		int getPercentComplete();
};

#endif