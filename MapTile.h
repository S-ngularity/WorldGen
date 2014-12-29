#ifndef MAPTILE
#define MAPTILE

#include "Pos.h" // pred

#define MULTIPLIER 1

class MapTile
{
	private:
		//Pos pos;
		int h;
		int chance;
		Pos pred;
		bool isSeed;
		bool skip;
		bool error;

		//void setPos(int x, int y);

	public:
		MapTile();

		int getH();
		void setH(int newH);

		int getChance();
		void setBaseChance(); // set chance de próximos tiles manterem altura
		void lowerChance(MapTile prevTile); // diminui a chance desse tile baseada na chance do tile passado

		Pos getPred();
		void setPred(Pos newPred);
		void setPred(int predX, int predY);

		bool getIsSeed();
		void setIsSeed(bool newIsSeed);

		bool getSkip();
		void setSkip(bool newSkip);

		bool getError();
		void setError(bool newError);
};

#endif