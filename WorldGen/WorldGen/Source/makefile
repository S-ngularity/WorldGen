# Makefile for Worldgen

# *****************************************************
# Variables to control Makefile operation

CXX = g++
#flags Linux:
#CXXFLAGS = -Wall -std=c++11 -lSDL2 -lSDL2_ttf -MMD -I .			
#flags Windows:
CXXFLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -Wall -std=c++14 -MMD -I .

SRC =	main.cpp \
		MyUtils.cpp \
		Map/Map.cpp \
		Map/Noises/DiamSqNoise.cpp Map/Noises/OpenSimplexNoise.cpp \
		Map/Noises/OpenSimplexC/open-simplex-noise.cpp \
		SdlClasses/SdlTexture.cpp \
		SdlClasses/SdlWindow.cpp \
		Ui/UiObject.cpp Ui/UiManager.cpp \
		CustomSdlTextures/MapTexture.cpp \
		CustomSdlWindows/NoiseWindow.cpp CustomSdlWindows/WalkWindow.cpp \
		CustomUiObjects/MapFrame.cpp \
		DefaultUiObjects/UiButton.cpp DefaultUiObjects/UiPanel.cpp DefaultUiObjects/UiLabel.cpp

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
