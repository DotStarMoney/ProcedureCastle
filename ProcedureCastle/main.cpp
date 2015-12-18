#include "debug.h"
//#include "painter.h"
#include "utility.h"
#include "linhash2.h"
#include <Windows.h>

//	TODO:
//		Create GraphicsCollection style bitmap fonts
//		Add in PC generation stuff



#if DEBUG_SHOULD_ERROR_CHECK
INITIALIZE_EASYLOGGINGPP
#endif

int main(int argv, const char** argc) 
{
#if DEBUG_SHOULD_ERROR_CHECK
	START_EASYLOGGINGPP(argv, argc);
	el::Configurations conf(DEBUG_LOG_CONFIG_FILE);
	el::Loggers::reconfigureAllLoggers(conf);
#endif
	/*
	uint64_t ts, te;
	int i;

	Painter::screen(640, 480);
	
	while (Painter::runApp())
	{

		Painter::cls();

		for (i = 0; i < 256; i++)
		{
			Painter::line(
				rnd_intRange(0, 639), rnd_intRange(0, 479),
				rnd_intRange(0, 639), rnd_intRange(0, 479),
				Painter::Color_t(1.0, rnd(), rnd(), 1.0), Painter::BF);
		}


		Painter::sync();

	}
	
	*/

}