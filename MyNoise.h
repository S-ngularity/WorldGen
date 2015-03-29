#ifndef MYNOISE
#define MYNOISE

#include "Map.h"

typedef enum {readTect = 0, doTect, readEro, doEro, done} State;

class MyNoise
{
	private:
		bool alreadySaved; // PARA SALVAR IMAGEM UMA VEZ

		Map &map;

		State state;

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

		int getPercentComplete();
		bool isDone();

		int getHighestH();

		// função adaptada da tgaLib
		int tgaSave(char const *filename, 
			 short int		width, 
			 short int		height, 
			 unsigned char	pixelDepth,
			 unsigned char	*imageData);
};

#endif