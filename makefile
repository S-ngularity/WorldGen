# Makefile for Worldgen

# *****************************************************
# Variables to control Makefile operation

CXX = g++
#flags Linux:
#CXXFLAGS = -Wall -std=c++11 -lSDL2 -lSDL2_ttf -MMD -I .			
#flags Windows:
CXXFLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -Wall -std=c++11 -MMD -I .

SRC =	main.cpp Pos.cpp MapTile.cpp Map.cpp \
		Noises/MyNoise.cpp Noises/DiamSqNoise.cpp Noises/OpenSimplexNoise.cpp \
		Noises/OpenSimplexC/open-simplex-noise.cpp \
		SdlClasses/SdlTexture.cpp \
		SdlClasses/SdlTextures/MapTexture.cpp \
		SdlClasses/SdlWindow.cpp \
		SdlClasses/SdlWindows/NoiseWindow.cpp SdlClasses/SdlWindows/WalkWindow.cpp \
		Ui/UiObject.cpp Ui/UiEventAggregator.cpp \
		Ui/UiCustomObjects/MapFrame.cpp Ui/UiCustomObjects/MouseHeightText.cpp

OUT = Worldgen

# *****************************************************
# Targets needed to bring the executable up to date

all: ${SRC:.cpp=.o}
	${CXX} ${SRC:.cpp=.o} ${CXXFLAGS} -o ${OUT}

clean:
		rm ${SRC:.cpp=.o} ${SRC:.cpp=.d} ${SRC:.cpp=.h.gch} -f
 
uninstall:
		rm Worldgen -f

-include ${SRC:.cpp=.d}
