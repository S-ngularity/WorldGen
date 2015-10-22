#ifndef UIEVENTCODES
#define UIEVENTCODES

class UiEventCode
{
	public:
		UiEventCode(int c) { code = c;}
		int code;
};

// handled by NoiseWindow
#define UIEVT_RUNNOISEUPDATE 1

#define UIEVT_FREQINCREASE 2
#define UIEVT_FREQDECREASE 3
#define UIEVT_PERSINCREASE 4
#define UIEVT_PERSDECREASE 5
#define UIEVT_FDIVINCREASE 6
#define UIEVT_FDIVDECREASE 7

#define UIEVT_NOISEINFOUPDATED 8


#endif
