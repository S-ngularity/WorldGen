#ifndef NOISEINFO
#define NOISEINFO

class NoiseInfoRequest
{
	public:
		NoiseInfoRequest()
		{}

		std::string noiseName;

		int oct;
		double freq, pers, fDiv;

		void setInfo(int o, double f, double p, double fd)
		{
			oct = o;
			freq = f;
			pers = p;
			fDiv = fd;
		}
};

#endif
