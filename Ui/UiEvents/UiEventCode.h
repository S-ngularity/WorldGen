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

#endif