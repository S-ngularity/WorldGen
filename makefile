worldGen: main.cpp map.cpp posQueue.cpp posBST.cpp
	g++ -o worldGen main.cpp Pos.cpp MapTile.cpp Map.cpp MyNoise.cpp PosQueue.cpp PosBST.cpp -lmingw32 -lSDL2main -lSDL2 -Wall