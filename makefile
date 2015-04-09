worldGen:
	g++ -o worldGen main.cpp Pos.cpp MapTile.cpp Map.cpp Noises/MyNoise.cpp Noises/DiamSqNoise.cpp SdlClasses/SdlTexture.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -Wall -std=c++11